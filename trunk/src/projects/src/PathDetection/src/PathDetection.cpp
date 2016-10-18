#include "PathDetection.h"
#include "DefaultSequenceDetector.h"
#include "OpticalFlowSequenceDetector.h"

#include <utils/PipeProcessUtils.h>
#include <utils/Filesystem.h>

#include <config/PathDetection.h>

namespace clustering
{
	const std::string PathDetection::INPUT_FILE_EXTENSION = "cvs";
	const std::string PathDetection::OUTPUT_FILE_EXTENSION = "path";


	PathDetection::PathDetection()
		: utils::PipeProcess(INPUT_FILE_EXTENSION)
		, m_LastIndex(-1)
	{

	}

	PathDetection::~PathDetection()
	{

	}

	utils::IAppModule* PathDetection::create()
	{
		return new PathDetection();
	}

	void PathDetection::registerParameters(ProgramOptions& programOptions)
	{
		PipeProcess::registerParameters(programOptions);

		programOptions.add("visualize", "  visualization enabled");
		programOptions.add<unsigned>("algorithm", "0 - optical flow, 1 - hungarian");

		DefaultSequenceDetector::registerParameters(programOptions);
		OpticalFlowSequenceDetector::registerParameters(programOptions);
	}

	bool PathDetection::loadParameters(const ProgramOptions& options)
	{
		config::PathDetection::getInstance().load();

		std::cout << "================================================================================\n";
		if (!PipeProcess::loadParameters(options))
			return false;

		bool success = true;

		m_Visualize = options.exists("visualize");
		//m_Visualize = true;

		unsigned algorithm = config::PathDetection::ALGORITHM_OPTFLOW;
		if (!options.get<unsigned>("algorithm", algorithm))
			algorithm = config::PathDetection::getInstance().getAlgorithm();

		if (config::PathDetection::ALGORITHM_DEFAULT == algorithm)
		{
			std::cout << "Algorithm: Hungarian\n";
			m_Detector = std::make_unique<DefaultSequenceDetector>(m_Visualize);
		}
		else if (config::PathDetection::ALGORITHM_OPTFLOW == algorithm)
		{
			std::cout << "Algorithm: Optical Flow\n";
			m_Detector = std::make_unique<OpticalFlowSequenceDetector>(m_Visualize);
		}
		else
		{
			throw std::runtime_error("PathDetection::loadParameters(): Invalid algorithm");
		}
		m_Detector->loadParameters(options);

		std::cout << std::setw(35) << "Visualize: " << m_Visualize << '\n';
		std::cout << "================================================================================\n\n";

		if (m_Visualize)
			startVisualize();

		return success;
	}

	void PathDetection::reserve()
	{
		m_FileLock.reset();

		if (m_LastIndex == -1)
		{
			utils::PipeProcessUtils::IndicesSet files;
			utils::PipeProcessUtils::getFilesIndices(files, getInputFolder(), getFilePattern(), INPUT_FILE_EXTENSION);

			if (files.empty())
			{
				waitForFile();
				return;
			}

			m_LastIndex = *(files.begin());
		}

		for (;; ++m_LastIndex)
		{			
			m_FileName = getFilePattern() + "." + std::to_string(m_LastIndex);
			
			try
			{
				if (utils::Filesystem::exists(getOutputFolder().string() + m_FileName + "." + OUTPUT_FILE_EXTENSION))
					continue;
			}
			catch (const boost::filesystem::filesystem_error&) { continue; }

			m_FileLock = std::make_unique<utils::FileLock>(getOutputFolder().string() + m_FileName + "." + OUTPUT_FILE_EXTENSION + ".loc");
			if (!m_FileLock->lock())
				continue;

			setState(PipeProcess::PROCESS);
			return;
		}

		waitForFile();
	}

	void PathDetection::process()
	{
		if (!utils::Filesystem::exists(getInputFolder().string() + m_FileName + "." + INPUT_FILE_EXTENSION))
		{
			waitForFile();
			return;
		}

		utils::FileLock fileLock(getInputFolder().string() + m_FileName + "." + INPUT_FILE_EXTENSION + ".loc");
		if (!fileLock.lock())
		{
			waitForFile();
			return;
		}

		m_Detector->process(getInputFolder().string() + m_FileName + "." + INPUT_FILE_EXTENSION, getOutputFolder().string() + m_FileName + "." + OUTPUT_FILE_EXTENSION);		

		setState(PipeProcess::RESERVE_FILE);
	}

	void PathDetection::startVisualize()
	{
		m_Timer.setInterval(2);
		connect(&m_Timer, SIGNAL(timeout()), this, SLOT(show()));
		m_Timer.start();
	}

	void PathDetection::show()
	{
		m_Detector->show();
	}
}