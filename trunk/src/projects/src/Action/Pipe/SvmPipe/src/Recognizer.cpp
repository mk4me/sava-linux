#include "Recognizer.h"
#include "svm.h"

#include "utils/Filesystem.h"
#include "utils/PipeProcessUtils.h"

#include "sequence/Action.h"

#include "config/Action.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

//#include <xutility>

typedef utils::PipeProcessUtils PPUtils;

Recognizer::Recognizer()
	: PipeProcess("(gbh)|(mbh)", 2)
	, m_LastVideoIndex(-1)
{
	config::Action::getInstance().load();
	if (config::Action::getInstance().getDescriptorType() & config::Action::MBH)
		m_Extensions.push_back("mbh");
	if (config::Action::getInstance().getDescriptorType() & config::Action::GBH)
		m_Extensions.push_back("gbh");

	load();
}

Recognizer::~Recognizer()
{
	for (svm_model* model : m_Model)
		svm_free_and_destroy_model(&model);
}

utils::IAppModule* Recognizer::create()
{
	return new Recognizer();
}

void Recognizer::reserve()
{
	if (getWaitTimeout() > 0)
		reserveAll();
	else
		reserveRealtime();
}

void Recognizer::process()
{
	std::string inFilePattern = getInFilePattern(m_CurrentIndex);

	for (const std::string& extension : m_Extensions)
	{
		m_InFileLocks.emplace_back(new utils::FileLock(inFilePattern + '.' + extension + ".loc"));
		if (!m_InFileLocks.back()->lock())
		{
			m_InFileLocks.clear();
			setState(PipeProcess::RESERVE_FILE);
			return;
		}
	}

	processEmptyActions();

	std::vector<float> fisherVector;
	for (int i = m_Extensions.size() - 1; i >= 0; --i) // First insert GBH fv then MBH
	{
		std::vector<float> fv = loadFV(inFilePattern + '.' + m_Extensions[i]);
		fisherVector.insert(fisherVector.end(), fv.begin(), fv.end());
	}

	std::string outFileName = getOutFileName(m_CurrentIndex);
	int actionId = -1;
	if (fisherVector.size() == config::Action::getInstance().getDescriptorSize())
		actionId = predict(fisherVector);
	sequence::Action action(actionId, sequence::Action::GENERATED);
	action.save(outFileName);
	std::cout << outFileName << " recognized as class " << actionId << std::endl;

	m_LastVideoIndex = m_CurrentIndex[0];
	m_FileLock.reset();
	setState(PipeProcess::FINALIZE);
}

void Recognizer::processEmptyActions() const
{
	for (int i = m_LastVideoIndex + 1; i < m_CurrentIndex[0]; ++i)
	{
		std::string outFileName = getOutFileName({ i, 0 });
		if (utils::Filesystem::exists(outFileName))
			continue;

		std::unique_ptr<utils::FileLock> lock = std::make_unique<utils::FileLock>(outFileName + ".loc");
		if (!lock->lock())
			continue;

		if (utils::Filesystem::exists(outFileName))
			continue;

		sequence::Action action(-1, sequence::Action::GENERATED);
		action.save(outFileName);
		std::cout << outFileName << " saved empty" << std::endl;
	}
}

void Recognizer::finalize()
{
	if (getWaitTimeout() > 0)
		finalizeAll();
	else
		finalizeRealTime();
}

std::string Recognizer::getInFilePattern(const std::vector<int>& indexList) const
{
	std::string fileName = getInputFolder().string() + getFilePattern();
	for (int index : indexList)
		fileName += '.' + std::to_string(index);
	return fileName;
}

std::string Recognizer::getOutFileName(const std::vector<int>& indexList) const
{
	std::string fileName = getOutputFolder().string() + getFilePattern();
	for (int index : indexList)
		fileName += '.' + std::to_string(index);
	return fileName + ".acn";
}

void Recognizer::load()
{
	std::string dataPath = utils::Filesystem::getDataPath() + "action/svm/";

	for (int i = 0;; ++i)
	{
		std::string modelFile = dataPath + "model_" + std::to_string(i) + ".dat";
		if (!utils::Filesystem::exists(modelFile))
			return;

		svm_model* model = svm_load_model(modelFile.c_str());
		if (model == nullptr)
		{
			std::cerr << "Can't load model: " << modelFile << std::endl;
			return;
		}
		m_Model.push_back(model);
		std::cout << "Loaded class " << i << std::endl;
	}
}

int Recognizer::predict(const std::vector<float>& fv) const
{
	std::vector<svm_node> x(fv.size() + 1);
	std::vector<double> values(m_Model.size());
	double maxVal;

	int j0;
	for (j0 = 0; j0 < fv.size(); j0++)  //feature dimensions
	{
		x[j0].index = j0 + 1;  //feature id beginning from "1" while j0 beginning from "0"
		x[j0].value = fv[j0];
	}
	x[j0].index = -1;

	svm_predict_values(m_Model[0], &x[0], &values[0]);
	maxVal = values[0];
	int label = 0;
	//classify x with nCls models and save the results into values[i]

	//For binary libsvm(before version 3.17), 
	//since internally class labels are ordered by their first occurrence in the training set, 
	//we need to set the classified values to negative after the 1st class
	for (int i = 1; i < values.size(); i++)
	{
		svm_predict_values(m_Model[i], &x[0], &values[i]);
		values[i] = -values[i];

		if (maxVal < values[i])
		{
			maxVal = values[i];
			label = i;
		}
	}

	return label;
}

void Recognizer::reserveRealtime()
{
	PPUtils::IndicesVectorSet videoFiles;
	PPUtils::getFilesIndices(videoFiles, { -1 }, getInputFolder(), getFilePattern(), m_Extensions[0], 2);

	if (m_LastVideoIndex < 0 && !videoFiles.empty())
		m_LastVideoIndex = videoFiles.rbegin()->at(0);

	for (auto it = videoFiles.rbegin(); it != videoFiles.rend(); ++it)
	{
		if ((*it)[0] < m_LastVideoIndex)
			break;

		PPUtils::IndicesVectorSet allFiles;
		PPUtils::getFilesIndices(allFiles, { (*it)[0], -1 }, getInputFolder(), getFilePattern(), m_Extensions[0], 2);

		for (PPUtils::IndicesVector fileIndex : allFiles)
		{
			if (utils::Filesystem::exists(getOutFileName(fileIndex)))
				continue;

			std::string inFilePattern = getInFilePattern(fileIndex);
			if (m_Extensions.size() > 1 && !utils::Filesystem::exists(inFilePattern + '.' + m_Extensions[1]))
					continue;

			m_FileLock = std::make_unique<utils::FileLock>(getOutFileName(fileIndex) + ".loc");
			if (!m_FileLock->lock())
				continue;

			if (utils::Filesystem::exists(getOutFileName(fileIndex)))
			{
				m_FileLock.reset();
				continue;
			}

			m_CurrentIndex = fileIndex;
			setState(PipeProcess::PROCESS);
			return;
		}
	}

	waitForFile();
}

void Recognizer::reserveAll()
{
	PPUtils::IndicesVectorSet allFiles;
	PPUtils::getFilesIndices(allFiles, { -1, -1 }, getInputFolder(), getFilePattern(), m_Extensions[0], 2);
	if (m_LastVideoIndex < 0 && !allFiles.empty())
		m_LastVideoIndex = allFiles.begin()->at(0);

	for (PPUtils::IndicesVector fileIndex : allFiles)
	{
		if (utils::Filesystem::exists(getOutFileName(fileIndex)))
			continue;

		std::string inFilePattern = getInFilePattern(fileIndex);
		if (m_Extensions.size() > 1 && !utils::Filesystem::exists(inFilePattern + '.' + m_Extensions[1]))
			continue;

		m_FileLock = std::make_unique<utils::FileLock>(getOutFileName(fileIndex) + ".loc");
		if (!m_FileLock->lock())
			continue;

		m_CurrentIndex = fileIndex;
		setState(PipeProcess::PROCESS);
		return;
	}

	waitForFile();
}

std::vector<float> Recognizer::loadFV(const std::string& file) const
{
	try
	{
		std::ifstream ifs(file, std::ios::binary);
		boost::archive::binary_iarchive ia(ifs);
		std::vector<float> fisherVector;
		ia & fisherVector;
		return fisherVector;
	}
	catch (...)
	{
		return std::vector<float>();
	}
}

void Recognizer::finalizeRealTime()
{
	m_InFileLocks.clear();

	for (std::string extension : {"mbh", "gbh"})
	{
		PPUtils::IndicesVectorSet allFiles;
		PPUtils::getFilesIndices(allFiles, { -1, -1 }, getInputFolder(), getFilePattern(), extension, 2);

		for (PPUtils::IndicesVector index : allFiles)
		{
			if (index[0] > m_LastVideoIndex - 2)
				break;

			std::string fileName = getInFilePattern(index) + '.' + extension;
			std::unique_ptr<utils::FileLock> lock = std::make_unique<utils::FileLock>(fileName + ".loc");
			if (lock->lock())
				boost::filesystem::remove(fileName);
		}
	}

	setState(PipeProcess::RESERVE_FILE);
}

void Recognizer::finalizeAll()
{
	for (std::string extension : m_Extensions)
		boost::filesystem::remove(getInFilePattern(m_CurrentIndex) + '.' + extension);

	m_InFileLocks.clear();
	setState(PipeProcess::RESERVE_FILE);
}
