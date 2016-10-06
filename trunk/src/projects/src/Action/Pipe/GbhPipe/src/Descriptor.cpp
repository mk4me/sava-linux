#include "Descriptor.h"

#include "Utils\PipeProcessUtils.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

typedef utils::PipeProcessUtils PPUtils;

const std::string Descriptor::c_InFileExt = "clu";
const std::string Descriptor::c_OutFileExt = "gbh";

Descriptor::Descriptor()
	: PipeProcess(c_InFileExt, 2)
{
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
	PPUtils::IndicesVectorSet allFiles;
	PPUtils::getFilesIndices(allFiles, { -1 }, getInputFolder(), getFilePattern(), c_InFileExt, 2);
	if (allFiles.empty())
	{
		waitForFile();
		return;
	}

	int videoIndex = (*allFiles.rbegin())[0];
	PPUtils::IndicesVectorSet files;
	PPUtils::getFilesIndices(files, { videoIndex, -1 }, getInputFolder(), getFilePattern(), c_InFileExt, 2);

	for (PPUtils::IndicesVector fileIndex : files)
	{
		if(boost::filesystem::exists(getOutFileName(fileIndex)))
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

void Descriptor::process()
{
	std::string inFileName = getInFileName(m_CurrentIndex);
	if (boost::filesystem::exists(inFileName + ".loc"))
	{
		waitForFile();
		return;
	}

	std::vector<float> fisherVector;
	m_Fv.computeFV(inFileName, fisherVector);

	std::ofstream ofs(getOutFileName(m_CurrentIndex));
	boost::archive::binary_oarchive oa(ofs);
	oa & fisherVector;

	m_FileLock.reset();

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
