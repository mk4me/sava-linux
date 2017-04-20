#include "MonitorPipe.h"
#include "MonitorVideoManager.h"

#include <utils/PipeProcessUtils.h>
#include <utils/Filesystem.h>

#include <config/Diagnostic.h>
#include <config/Directory.h>
#include <config/Monitor.h>

#include "boost/timer/timer.hpp"

MonitorPipe::VisualizationCreator* MonitorPipe::ms_VisualizationCreator = nullptr;

MonitorPipe::MonitorPipe()
	: AbstractMonitor("cvs", 1, false)
	, m_NextIndex(-1)
	, m_LastToDeleteIndex(-1)
{
}

MonitorPipe::~MonitorPipe()
{
	
}

utils::IAppModule* MonitorPipe::create()
{
	return new MonitorPipe();
}

void MonitorPipe::registerParameters(ProgramOptions& programOptions)
{
	AbstractMonitor::registerParameters(programOptions);
}

bool MonitorPipe::loadParameters(const ProgramOptions& options)
{
	if (!AbstractMonitor::loadParameters(options))
		return false;

	if (ms_VisualizationCreator)
		ms_VisualizationCreator->createVisualization();

	config::Diagnostic::getInstance().load();
	if (config::Diagnostic::getInstance().getLogMonitorQueue())
	{
		utils::TimedLog::startTimer("queue", 60.0f);
	}

	return true;
}

void MonitorPipe::reserve()
{

	utils::PipeProcessUtils::IndicesVectorSet indices;
	utils::PipeProcessUtils::IndicesVector pattern = { -1 };
	utils::PipeProcessUtils::getFilesIndices(indices, pattern, getInputFolder(), getFilePattern(), "acn", 2);

	if (indices.size() > 1)
	{
		m_NextIndex = indices.rbegin()->at(0);
		setState(PipeProcess::PROCESS);
	}

	waitForFile();
}

void MonitorPipe::process()
{
	std::string nextAcnFileName = getInputFolder().string() + getFilePattern() + "." + std::to_string(m_NextIndex) + ".0.acn";
	if (!utils::Filesystem::exists(nextAcnFileName))
	{
		waitForFile();
		return;
	}

	std::string fileName = getInputFolder().string() + getFilePattern() + "." + std::to_string(m_NextIndex - 1);
	utils::FileLock fileLock(fileName + ".cvs.loc");
	if (!fileLock.lock())
	{
		waitForFile();
		return;
	}
		
	//std::cout << "processing " << fileName << std::endl;

	std::shared_ptr<sequence::MetaVideo> video = std::make_shared<sequence::MetaVideo>(fileName);
	//m_VideoToFileName[video] = fileName;
	m_InputQueue.push(video);
	++m_NextIndex;
}

bool MonitorPipe::start()
{
	if (!PipeProcess::start())
		return false;
	
	boost::thread t(&MonitorPipe::fileRemoveThread, this);
	m_FileRemoveThread.swap(t);

	return true;
}

void MonitorPipe::stop()
{
	m_InputQueue.release();
	m_DeleteQueue.release();

	if (m_FileRemoveThread.joinable())
		m_FileRemoveThread.join();

	PipeProcess::stop();
}

std::shared_ptr<sequence::MetaVideo> MonitorPipe::popInputVideo()
{
	std::shared_ptr<sequence::MetaVideo> inputVideo;
	m_InputQueue.pop(inputVideo);
	return inputVideo;
}

void MonitorPipe::pushDeleteVideo(const std::shared_ptr<sequence::MetaVideo>& video)
{
	m_DeleteQueue.push(video);
}

std::string MonitorPipe::getFileName(int index) const
{
	FilesystemPath inputFile = getInputFolder();
	inputFile += getFilePattern();
	return inputFile.string() + "." + std::to_string(index);
}

void MonitorPipe::fileRemoveThread()
{
	std::string backupDirectory = config::Directory::getInstance().getBackupPath();
	bool backupEnabled = config::Monitor::getInstance().isBackupEnabled();

	std::shared_ptr<sequence::MetaVideo> video;
	while (m_DeleteQueue.pop(video))
	{
		boost::filesystem::path filename = video->getFileName();
		//std::cout << "to delete: " << filename << std::endl;
		auto ext = filename.extension().string().substr(1);
		//std::cout << "\text: " << ext << std::endl;
		m_LastToDeleteIndex = std::stoi(ext);

		int upperIndex = m_LastToDeleteIndex;
		if (upperIndex == -1)
			continue;

		// delete files
		std::vector<boost::filesystem::path> filesToDelete;

		boost::filesystem::directory_iterator endIt;
		for (boost::filesystem::directory_iterator it(getInputFolder()); it != endIt; ++it)
		{
			auto e = it->path().extension().string();
			//std::cout << ext << std::endl;
			if (e != ".cvs" && e != ".clu" && e != ".acn")
				continue;
			//std::cout << "file" << std::endl;
			std::string file = it->path().filename().string();
			//std::cout << '\t' << file << std::endl;
			file = file.substr(getFilePattern().size() + 1);
			//std::cout << '\t' << file << std::endl;
			file = file.substr(0, file.find_first_of('.'));
			//std::cout << '\t' << file << std::endl;
			int index = std::stoi(file);
			if (index <= upperIndex)
			{
				
				filesToDelete.push_back(it->path());
			}
		}

		for (auto& p : filesToDelete)
		{
			try
			{
				utils::FileLock fileLock(p.string() + ".loc");
				if (!fileLock.lock())
				{
					continue;
				}
				if (p.extension() == ".clu")
				{
					utils::FileLock fileLockGbh(p.string() + ".gbh.loc");
					if (!fileLockGbh.lock())
					{
						continue;
					}
					utils::FileLock fileLockMbh(p.string() + ".mbh.loc");
					if (!fileLockMbh.lock())
					{
						continue;
					}
				}

				if (backupEnabled)
				{
					boost::filesystem::rename(p, backupDirectory + p.filename().string());
				}
				else
				{
					boost::filesystem::remove(p);
				}
				
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
}

void MonitorPipe::save(std::ostream& os)
{
	auto offset = MonitorVideoManager::getInstance().getOffsetTime();	

	os << getInputQueueSize() << ';' << offset.total_seconds() << '\n';
}

size_t MonitorPipe::getInputQueueSize() const
{
	return m_InputQueue.size();
}

