#include "Descriptor.h"

#include <config/Action.h>
#include <config/Diagnostic.h>

#include <utils/PipeProcessUtils.h>
#include <utils/Filesystem.h>
#include <utils/Application.h>

#include <sequence/IStreamedVideo.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <algorithm>

typedef utils::PipeProcessUtils PPUtils;

const std::string Descriptor::c_InFileExt = "clu";
std::string Descriptor::c_OutFileExt;

Descriptor::Descriptor()
	: PipeProcess(c_InFileExt, 2)
	, m_CurrentIndex({ -1, -1 })
{
	config::Action::getInstance().load();
	m_Fv.load();
}

Descriptor::~Descriptor()
{

}

utils::IAppModule* Descriptor::create()
{
	return new Descriptor();
}

void Descriptor::reserve()
{
	if (m_CurrentIndex[0] < 0)
	{
		PPUtils::IndicesVectorSet allFiles;
		PPUtils::getFilesIndices(allFiles, { -1 }, getInputFolder(), getFilePattern(), c_InFileExt, 2);
		if (allFiles.empty())
		{
			waitForFile();
			return;
		}
		m_CurrentIndex[0] = getWaitTimeout() > 0 ? (*allFiles.begin())[0] : (*allFiles.rbegin())[0];
	}

	if (getWaitTimeout() > 0)
		reserveAll();
	else
		reserveRealtime();
}

void Descriptor::reserveRealtime()
{
	PPUtils::IndicesVector fileIndex = { m_CurrentIndex[0] + 1, 0 };
	while (utils::Filesystem::exists(getInFileName(fileIndex)))
	{
		m_CurrentIndex = { fileIndex[0], -1 };
		++fileIndex[0];
	}

	fileIndex = m_CurrentIndex;
	while (true)
	{
		++fileIndex[1];
		if (!utils::Filesystem::exists(getInFileName(fileIndex)))
		{
			waitForFile();
			return;
		}

		if (reserveFile(fileIndex))
			return;
	}

	waitForFile();
}

void Descriptor::reserveAll()
{
	PPUtils::IndicesVector fileIndex = m_CurrentIndex;
	while (true)
	{
		++fileIndex[1];
		if (!utils::Filesystem::exists(getInFileName(fileIndex)))
		{
			if (utils::Filesystem::exists(getInFileName({ fileIndex[0] + 1, 0 })))
			{
				m_CurrentIndex = { fileIndex[0] + 1, -1 };
				return;
			}

			waitForFile();
			return;
		}

		if (reserveFile(fileIndex))
			return;
	}

	waitForFile();
}

bool Descriptor::reserveFile(const std::vector<int>& fileIndex)
{
	if (utils::Filesystem::exists(getOutFileName(fileIndex)))
		return false;

	m_OutFileLock = std::make_unique<utils::FileLock>(getOutFileName(fileIndex) + ".loc");
	if (!m_OutFileLock->lock())
		return false;

	m_CurrentIndex = fileIndex;
	setState(PipeProcess::PROCESS);
	return true;
}

bool Descriptor::checkVideo(const std::string& videoName) const
{
	config::Action& config = config::Action::getInstance();

	std::shared_ptr<sequence::IStreamedVideo> video = sequence::IStreamedVideo::create(videoName);
	if (video->getNumFrames() < config.getMinVideoFrames())
		return false;

	cv::Mat frame;
	video >> frame;

	if (frame.size().width > config.getMaxVideoWidth() || frame.size().height > config.getMaxVideoHeight())
		return false;

	return true;
}

void Descriptor::process()
{
	std::string inFileName = getInFileName(m_CurrentIndex);

	{
		auto inLock = std::make_unique<utils::FileLock>(inFileName + ".loc");
		if (!inLock->lock())
		{
			waitForFile();
			return;
		}
	}

	m_InFileLock = std::make_unique<utils::FileLock>(getInFileName(m_CurrentIndex) + '.' + c_OutFileExt + ".loc");
	if (!m_InFileLock->lock())
	{
		setState(PipeProcess::RESERVE_FILE);
		return;
	}

	std::cout << std::endl << "Processing file: " << inFileName << std::endl;

	std::vector<float> fisherVector;
	if (checkVideo(inFileName))
	{
		if (!m_Fv.computeFV(inFileName, fisherVector))
			fisherVector.clear();
	}

	std::ofstream ofs(getOutFileName(m_CurrentIndex), std::ios::binary);
	boost::archive::binary_oarchive oa(ofs);
	oa & fisherVector;

	m_OutFileLock.reset();

	setState(PipeProcess::RESERVE_FILE);
}

std::string Descriptor::getInFileName(const std::vector<int>& indexList) const
{
	std::string fileName = getInputFolder().string() + getFilePattern();
	for (int index : indexList)
		fileName += '.' + std::to_string(index);
	return fileName + '.' + c_InFileExt;
}

std::string Descriptor::getOutFileName(const std::vector<int>& indexList) const
{
	std::string fileName = getOutputFolder().string() + getFilePattern();
	for (int index : indexList)
		fileName += '.' + std::to_string(index);
	return fileName + '.' + c_OutFileExt;
}

bool Descriptor::loadParameters(const ProgramOptions& options)
{
	if (!utils::PipeProcess::loadParameters(options))
		return false;
	
	config::Diagnostic::getInstance().load();
	if (config::Diagnostic::getInstance().getLogMemoryUsage())
	{
		utils::Application::getInstance()->enableMomoryLogging();
	}

	return true;
}
