#include "MilestoneMonitorPipe.h"

#include <sequence/Action.h>
#include <sequence/Cluster.h>
#include <sequence/CompressedVideo.h>

#include <utils/PipeProcessUtils.h>
#include <utils/AlertSender.h>

#include <config/Glossary.h>

#include <boost/timer/timer.hpp>

MilestoneMonitorPipe::MilestoneMonitorPipe()
	: AbstractMonitor("cvs", 1, false)
{
}

MilestoneMonitorPipe::~MilestoneMonitorPipe()
{

}

utils::IAppModule* MilestoneMonitorPipe::create()
{
	return new MilestoneMonitorPipe();
}

void MilestoneMonitorPipe::reserve()
{
	utils::PipeProcessUtils::IndicesVectorSet indices;
	utils::PipeProcessUtils::IndicesVector pattern = { -1 };
	utils::PipeProcessUtils::getFilesIndices(indices, pattern, getInputFolder(), getFilePattern(), "acn", 2);

	if (indices.size() > 1)
	{
		m_NextIndex = indices.rbegin()->at(0);
		setState(PipeProcess::PROCESS);
		return;
	}

	waitForFile();
}

void MilestoneMonitorPipe::process()
{
	// delete files
	std::vector<boost::filesystem::path> filesToDelete;	
	std::set<FileDesc> files;

	findFiles(files);

	if (files.empty())
	{		
		waitForFile();
		return;
	}
	
	processFiles(files, filesToDelete);
	deleteFiles(filesToDelete);
}

void MilestoneMonitorPipe::findFiles(std::set<FileDesc> &files)
{
	std::string p = ".*" + getFilePattern() + "\\.(\\d+)\\.";
	std::regex cvsRx(p + "cvs$", std::regex::icase);
	std::regex gvsRx(p + "gvs$", std::regex::icase);
	std::regex cluRx(p + "(\\d+)\\.clu$", std::regex::icase);
	std::regex acnRx(p + "(\\d+)\\.acn$", std::regex::icase);

	boost::filesystem::directory_iterator endIt;
	for (boost::filesystem::directory_iterator it(getInputFolder()); it != endIt; ++it)
	{
		auto e = it->path().extension().string();
		std::regex rx;
		FileDesc fileDesc;
		if (e == ".cvs")
		{
			fileDesc.type = FileDesc::VIDEO;
			rx = cvsRx;
		}		
		else if (e == ".clu")
		{
			fileDesc.type = FileDesc::CLUSTER;
			rx = cluRx;
		}
		else if (e == ".acn")
		{
			fileDesc.type = FileDesc::ACTION;
			rx = acnRx;
		}
		else
			continue;

		std::smatch match;
		const std::string& search = it->path().string();
		if (!std::regex_search(search, match, rx))
			continue;

		fileDesc.majorId = std::stoi(match[1]);
		if (fileDesc.type != FileDesc::VIDEO)
			fileDesc.minorId = std::stoi(match[2]);
		else
			fileDesc.minorId = std::numeric_limits<int>::max();

		files.insert(fileDesc);
	}
}

void MilestoneMonitorPipe::processFiles(const std::set<FileDesc>& files, std::vector<boost::filesystem::path> &filesToDelete)
{
	for (auto& fileDesc : files)
	{
		auto path = getInputFolder();
		path += getFilePattern();
		path += fileDesc.suffix();

		if (fileDesc.majorId < m_NextIndex)
		{
			filesToDelete.push_back(path);
		}
		else if (fileDesc.type == FileDesc::ACTION && fileDesc.majorId == m_NextIndex)
		{
			auto actionFile = path;
			auto clusterFile = getInputFolder();
			clusterFile += actionFile.stem();
			clusterFile += ".clu";
			auto videoFile = getInputFolder();
			videoFile += actionFile.stem().stem();
			videoFile += ".cvs";


			utils::FileLock actionLock(actionFile.string() + ".loc");
			if (!actionLock.lock())
			{
				break;
			}
			utils::FileLock clusterLock(clusterFile.string() + ".loc");
			if (!clusterLock.lock())
			{
				break;
			}
			utils::FileLock videoLock(videoFile.string() + ".loc");
			if (!videoLock.lock())
			{				
				break;
			}
			
			sequence::CompressedVideo video;
			if(!video.load(videoFile.string()))
				std::cerr << "Can't load video " << videoFile << std::endl;
			sequence::Cluster cluster;
			if(!cluster.load(clusterFile.string()))
				std::cerr << "Can't load cluster " << clusterFile << std::endl;
			sequence::Action action;
			if(!action.load(actionFile.string()))
				std::cerr << "Can't load action " << actionFile << std::endl;

			sendMilestoneAlert(video, cluster, action);

			try
			{
				boost::filesystem::remove(actionFile);
			}
			catch (const std::exception& e)
			{
				std::cout << "Can't remove file " << actionFile << ". " << e.what() << std::endl;
			}
			catch (...)
			{
				std::cout << "Can't remove file " << actionFile << ". Unkwnown error." << std::endl;
			}
		}
		else if (fileDesc.type == FileDesc::ACTION && fileDesc.majorId > m_NextIndex)
		{
			m_NextIndex = fileDesc.majorId;
			break;
		}
	}
}

void MilestoneMonitorPipe::deleteFiles(const std::vector<boost::filesystem::path>& filesToDelete)
{
	for (auto& p : filesToDelete)
	{
		try
		{
			utils::FileLock fileLock(p.string() + ".loc");
			if (!fileLock.lock())
			{
				break;
			}
			if (p.extension() == ".clu")
			{
				utils::FileLock fileLockGbh(p.string() + ".gbh.loc");
				if (!fileLockGbh.lock())
				{
					break;
				}
				utils::FileLock fileLockMbh(p.string() + ".mbh.loc");
				if (!fileLockMbh.lock())
				{
					break;
				}
			}
			boost::filesystem::remove(p);
		}
		catch (const std::exception& e)
		{
			std::cout << "Can't remove file " << p << ". " << e.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "Can't remove file " << p << ". Unkwnown error." << std::endl;
		}
	}
}

std::string MilestoneMonitorPipe::FileDesc::suffix() const
{
	std::string s = "." + std::to_string(majorId) + ".";
	switch (type)
	{
	case FileDesc::ACTION:
		s += std::to_string(minorId) + ".acn";
		break;
	case FileDesc::CLUSTER:
		s += std::to_string(minorId) + ".clu";
		break;
	case FileDesc::VIDEO:
		s += "cvs";
		break;
	}
	return s;
}

bool MilestoneMonitorPipe::FileDesc::operator<(const FileDesc& rhs) const
{
	if (majorId < rhs.majorId) return true;
	if (majorId > rhs.majorId) return false;
	if (minorId < rhs.minorId) return true;
	if (minorId > rhs.minorId) return false;
	if (type < rhs.type) return true;
	if (type > rhs.type) return false;
	return false;
}
