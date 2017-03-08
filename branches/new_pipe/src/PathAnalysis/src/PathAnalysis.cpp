#include "PathAnalysis.h"

#include "PathAnalysisVisualizer.h"
#include "NullVisualizer.h"
#include "PathAnalysisAlgorithms/StatisticPathDistanceFactory.h"
#include "ConfigurationWindow.h"
#include "PathAnalysisAlgorithms/DefaultPathAnalyzer.h"
#include "PathAnalysisAlgorithms/Partition/PartitionAnalyzer.h"

#include <PathAnalysisAlgorithms/algorithms/FastPathClustering.h>
#include <PathAnalysisAlgorithms/algorithms/OptimizedPathDistanceFactory.h>

#include <sequence/PathStream.h>
#include <sequence/Cluster.h>
#include <sequence/CompressedVideo.h>

#include <config/PathAnalysis.h>
#include <config/Diagnostic.h>

#include <utils/PipeProcessUtils.h>
#include <utils/FileLock.h>
#include <utils/Filesystem.h>
#include <utils/Application.h>

#include <boost/timer/timer.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace clustering
{
	const std::string PathAnalysis::INPUT_FILE_EXTENSION("path");
	const std::string PathAnalysis::OUTPUT_FILE_EXTENSION("clu");

	PathAnalysis::PathAnalysis()
		: utils::PipeProcess(INPUT_FILE_EXTENSION)
		, m_FileNumber(-1)
	{

	}

	PathAnalysis::~PathAnalysis()
	{

	}

	utils::IAppModule* PathAnalysis::create()
	{
		return new PathAnalysis();
	}

	void PathAnalysis::registerParameters(ProgramOptions& programOptions)
	{
		PipeProcess::registerParameters(programOptions);

		programOptions.add("visualize", "  visualization enabled");
		programOptions.add<std::string>("vf", "  video folder");
		programOptions.add<std::string>("analyzer", "  path analyzer (default or partition)");

		//DefaultPathAnalyzer::registerParameters(programOptions);
		partition::Analyzer::registerParameters(programOptions);
	}

	bool PathAnalysis::loadParameters(const ProgramOptions& options)
	{
		std::cout << "================================================================================\n";
		if (!PipeProcess::loadParameters(options))
			return false;

		config::PathAnalysis& config = config::PathAnalysis::getInstance();
		config.load();

		std::string videoFolder;
		options.get<std::string>("vf", videoFolder);
		if (!videoFolder.empty())
			m_VideoFolder = videoFolder;
		else
			m_VideoFolder = getInputFolder();
		
		std::string analyzer = config.getAnalyzer();
		options.get<std::string>("analyzer", analyzer);
		std::cout << std::setw(35) << "Analyzer: " << analyzer << '\n';
		std::cout << std::setw(35) << "Visualize: " << m_Visualize << '\n';
		if (analyzer == "partition")
			m_PathAnalyzer = std::make_shared<partition::Analyzer>();
		else
			m_PathAnalyzer = std::make_shared<DefaultPathAnalyzer>();

		long minProcessTime = config.getMinProcessTime();
		m_PathAnalyzer->setMinProcessTime(minProcessTime);
		std::cout << "minProcessTime: " << minProcessTime << "\n";

		m_Visualize = options.exists("visualize");
		m_PathAnalyzer->setVisualize(m_Visualize);		
		m_PathAnalyzer->loadParameters(options);

		std::cout << "================================================================================\n";
		
		if (m_Visualize)
		{
			startVisualize();
		}

		config::Diagnostic::getInstance().load();
		if (config::Diagnostic::getInstance().getLogMemoryUsage())
		{
			utils::Application::getInstance()->enableMomoryLogging();
		}

		return true;
	}

	void PathAnalysis::reserve()
	{
		std::set<int> inFileNumbers;
		utils::PipeProcessUtils::getFilesIndices(inFileNumbers, getInputFolder(), getFilePattern(), INPUT_FILE_EXTENSION);

		if (!inFileNumbers.empty())
		{
			m_FileNumber = *(inFileNumbers.begin());
			setState(PipeProcess::PROCESS);
			return;
		}

		waitForFile();
	}

	void PathAnalysis::process()
	{
		if (!utils::Filesystem::exists(getInFileName()))
		{
			waitForFile();
			return;
		}
		m_FileLock = std::make_shared<utils::FileLock>(getInFileName() + ".loc");
		if (!m_FileLock->lock())
		{
			waitForFile();
			return;
		}
		if (m_Visualize)
		{
			if (!utils::Filesystem::exists(getVideoFileName()))
			{
				waitForFile();
				return;
			}
			m_VideoFileLock = std::make_shared<utils::FileLock>(getVideoFileName() + ".loc");
			if (!m_VideoFileLock->lock())
			{
				m_FileLock->unlock();
				waitForFile();
				return;
			}
		}

		FilesystemPath inputFile = getOutputFolder();
		inputFile += getFilePattern();

		m_PathAnalyzer->setFiles(getVideoFileName(), getInFileName(), inputFile.string() + "." + std::to_string(m_FileNumber) + ".", "." + OUTPUT_FILE_EXTENSION);
		m_PathAnalyzer->process();
		
		m_FileLock.reset();
		++m_FileNumber;
	}

	std::string PathAnalysis::getInFileName() const
	{
		FilesystemPath inputFile = getInputFolder();
		inputFile += getFilePattern();
		return inputFile.string() + "." + std::to_string(m_FileNumber) + "." + INPUT_FILE_EXTENSION;
	}

	std::string PathAnalysis::getVideoFileName() const
	{
		FilesystemPath inputFile = m_VideoFolder;
		inputFile += getFilePattern();
		return inputFile.string() + "." + std::to_string(m_FileNumber) + ".cvs";
	}

	std::string PathAnalysis::getOutFileName(unsigned subId) const
	{
		FilesystemPath inputFile = getOutputFolder();
		inputFile += getFilePattern();
		return inputFile.string() + "." + std::to_string(m_FileNumber) + "." + std::to_string(subId) + "." + OUTPUT_FILE_EXTENSION;
	}

	void PathAnalysis::startVisualize()
	{
		m_VisualizationTimer.setInterval(2);
		connect(&m_VisualizationTimer, SIGNAL(timeout()), this, SLOT(show()));
		m_VisualizationTimer.start();		
	}

	void PathAnalysis::show()
	{
		m_PathAnalyzer->show();
	}

}