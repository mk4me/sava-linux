#include "PathDetection.h"
#include "Detector.h"

#include <utils/PipeProcessUtils.h>

namespace clustering
{
	const std::string PathDetection::INPUT_FILE_EXTENSION = "cvs";
	const std::string PathDetection::OUTPUT_FILE_EXTENSION = "path";


	PathDetection::PathDetection()
		: utils::PipeProcess(INPUT_FILE_EXTENSION)
		
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

		Detector::registerParameters(programOptions);
	}

	bool PathDetection::loadParameters(const ProgramOptions& options)
	{
		std::cout << "================================================================================\n";
		if (!PipeProcess::loadParameters(options))
			return false;

		bool success = true;

		m_Visualize = options.exists("visualize");

		m_Detector = std::make_unique<Detector>(m_Visualize);
		m_Detector->loadParameters(options);
		std::cout << "================================================================================\n\n";

		if (m_Visualize)
			startVisualize();

		return success;
	}

	void PathDetection::reserve()
	{
		m_FileLock.reset();

		utils::PipeProcessUtils::IndicesMap files;
		utils::PipeProcessUtils::getFilesIndices(files, getInputFolder(), INPUT_FILE_EXTENSION);

		for (auto& fileEntry : files)
		{
			m_FileName = fileEntry.second.stem().filename().string();

			m_FileLock = std::make_unique<utils::FileLock>(fileEntry.second.string() + ".loc");
			if (!m_FileLock->lock())
				continue;

			try
			{
				if (boost::filesystem::exists(getOutputFolder().string() + m_FileName + "." + OUTPUT_FILE_EXTENSION))
					continue;

				if (boost::filesystem::exists(getOutputFolder().string() + m_FileName + "." + OUTPUT_FILE_EXTENSION + ".loc"))
					continue;
			}
			catch (const boost::filesystem::filesystem_error&) { continue; }

			setState(PipeProcess::PROCESS);
			return;
		}

		waitForFile();
	}

	void PathDetection::process()
	{
		utils::FileLock fileLock(getOutputFolder().string() + m_FileName + "." + OUTPUT_FILE_EXTENSION + ".loc");
		if (!fileLock.lock())
		{
			setState(PipeProcess::RESERVE_FILE);
			return;
		}

		m_Detector->process(getInputFolder().string() + m_FileName + "." + INPUT_FILE_EXTENSION, getOutputFolder().string() + m_FileName + "." + OUTPUT_FILE_EXTENSION);		

		setState(PipeProcess::RESERVE_FILE);
	}

	void PathDetection::startVisualize()
	{
		if (m_Visualize)
		{
			m_Timer.setInterval(2);
			connect(&m_Timer, SIGNAL(timeout()), this, SLOT(show()));
			m_Timer.start();
		}
	}

	void PathDetection::show()
	{
		m_Detector->show();
	}

}