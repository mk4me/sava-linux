#include "PipeProcess.h"
#include "PipeProcessUtils.h"
#include "Application.h"
#include "Filesystem.h"

#include <boost/filesystem.hpp>

namespace utils
{
	PipeProcess::PipeProcess(const std::string& inputExtension, unsigned inputNumIndices /*= 1*/) 
		: m_InputExtension(inputExtension)
		, m_InputNumIndices(inputNumIndices)
		, m_State(SPECIFY_PATTERN)
		, m_FileSystemChanged(false)
	{

	}

	PipeProcess::PipeProcess(unsigned inputNumIndices /*= 1*/)
		: m_InputNumIndices(inputNumIndices)
		, m_State(INIT)
		, m_FileSystemChanged(false)
	{

	}

	PipeProcess::~PipeProcess()
	{

	}

	void PipeProcess::registerParameters(ProgramOptions& programOptions)
	{
		programOptions.add<std::string>("if", "input folder");
		programOptions.add<std::string>("of", "output folder");
	}

	bool PipeProcess::loadParameters(const ProgramOptions& options)
	{
		try
		{
			bool success = true;

			std::string inputFolder, outputFolder;
			success &= options.get<std::string>("if", inputFolder);
			success &= options.get<std::string>("of", outputFolder);

			m_InputFolder = Filesystem::unifyPath(inputFolder);
			m_OutputFolder = Filesystem::unifyPath(outputFolder);

			std::cout << "            Input folder: " << m_InputFolder.string() << "\n";
			std::cout << "           Output folder: " << m_OutputFolder.string() << "\n";

			return success;
		}
		catch (const std::exception& e)
		{
			std::cerr << "PipeProcess::loadParameters(): " << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << "PipeProcess::loadParameters(): Unknown error." << std::endl;
		}

		return false;
	}

	bool PipeProcess::start()
	{
		if (!boost::filesystem::exists(m_InputFolder) || !boost::filesystem::is_directory(m_InputFolder))
		{
			try
			{
				boost::filesystem::create_directories(m_InputFolder);
			}
			catch (...)
			{
				std::cout << "PipeProcess::start(): Can't create input directory " << m_InputFolder << std::endl;
				return false;
			}
		}

		try
		{
			boost::filesystem::create_directories(m_OutputFolder);
		}
		catch (...)
		{
			std::cout << "PipeProcess::start(): Can't create output directory " << m_OutputFolder << std::endl;
			return false;
		}		
		m_FileSystemWatcher.addPath(QString::fromStdString(m_InputFolder.string()));
		connect(&m_FileSystemWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(onInputDirChanged(const QString&)));

		m_StateLoopThread = boost::thread(&PipeProcess::stateLoop, this);
		return true;
	}

	void PipeProcess::stop()
	{
		setState(EXIT);
		m_WaitCondition.notify_all();

		if (m_StateLoopThread.joinable())
			m_StateLoopThread.join();
	}

	PipeProcess::ProcessState PipeProcess::getState()
	{
		boost::lock_guard<boost::mutex> lock(m_StateMutex);
		return m_State;
	}

	void PipeProcess::setState(ProcessState state)
	{
		boost::lock_guard<boost::mutex> lock(m_StateMutex);
		if (m_State != EXIT)
			m_State = state;
	}

	void PipeProcess::waitForFile()
	{
		boost::mutex::scoped_lock lock(m_StateMutex);
		if (m_State != EXIT && !m_FileSystemChanged)
			m_WaitCondition.wait(lock);
		m_FileSystemChanged = false;
	}

	void PipeProcess::onInputDirChanged(const QString& path)
	{
		boost::mutex::scoped_lock lock(m_StateMutex);
		m_FileSystemChanged = true;
		m_WaitCondition.notify_all();
	}

	void PipeProcess::stateLoop()
	{
		while (true)
		{
			try
			{
				switch (getState())
				{
				case SPECIFY_PATTERN:
					specifyPattern();
					break;
				case INIT:
					init();
					break;
				case RESERVE_FILE:
					reserve();
					break;
				case PROCESS:
					process();
					break;
				case FINALIZE:
					finalize();
					break;
				case EXIT:
					std::cout << "Quitting..." << std::endl;
					return;
				}
			}
			catch (const std::exception& e)
			{
				std::cerr << "PipeProcess::stateLoop() error: " << e.what() << std::endl;
				std::cout << "Quitting..." << std::endl;
				return;
			}
			catch (...)
			{
				std::cerr << "PipeProcess::stateLoop() Unknown error." << std::endl;
				std::cout << "Quitting..." << std::endl;
				return;
			}
		}
	}

	void PipeProcess::init()
	{
		setState(RESERVE_FILE);
	}

	void PipeProcess::reserve()
	{
		setState(PROCESS);
	}

	void PipeProcess::process()
	{
		setState(FINALIZE);
	}

	void PipeProcess::finalize()
	{
		setState(RESERVE_FILE);
	}

	void PipeProcess::specifyPattern()
	{
		if (m_InputExtension.empty())
		{
			// pattern not specified.
			setState(INIT);
			return;
		}

		FilesystemPath file;
		if (PipeProcessUtils::getFirstFile(file, m_InputFolder, m_InputExtension, m_InputNumIndices))
		{
			file = file.filename().stem();
			for (unsigned i = 0; i < m_InputNumIndices; ++i)
			{
				file = file.stem();
			}
			m_FilePattern = file.string();
			std::cout << "file pattern: " << m_FilePattern << std::endl;
			setState(INIT);
			return;
		}
		waitForFile();
	}
}