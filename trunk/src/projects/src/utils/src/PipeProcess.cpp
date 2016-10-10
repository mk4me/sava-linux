#include "PipeProcess.h"
#include "PipeProcessUtils.h"
#include "Application.h"
#include "Filesystem.h"

#include <boost/filesystem.hpp>

namespace utils
{
	PipeProcess::PipeProcess(const std::string& patternExtension, unsigned inputNumIndices /*= 1*/, bool hasOutput /*= true*/)
		: m_PatternExtension(patternExtension)
		, m_InputNumIndices(inputNumIndices)
		, m_State(SPECIFY_PATTERN)
		, m_FileSystemChanged(false)
		, m_HasOutput(hasOutput)
		, m_WaitTimeout(0)
	{

	}

	PipeProcess::PipeProcess(unsigned inputNumIndices /*= 1*/, bool hasOutput /*= true*/)
		: m_InputNumIndices(inputNumIndices)
		, m_State(INIT)
		, m_FileSystemChanged(false)
		, m_HasOutput(hasOutput)
		, m_WaitTimeout(0)
	{

	}

	PipeProcess::~PipeProcess()
	{
		stop();
	}

	void PipeProcess::registerParameters(ProgramOptions& programOptions)
	{
		static bool registered = false;
		if (!registered)
		{
			programOptions.add<std::string>("if", "input folder");
			programOptions.add<std::string>("of", "output folder");
			programOptions.add<int>("timeout", "wait for file timeout (in seconds), 0 means infinity", 0);
			registered = true;
		}
	}

	bool PipeProcess::loadParameters(const ProgramOptions& options)
	{
		try
		{
			bool success = true;
			std::string inputFolder, outputFolder;

			success &= options.get<std::string>("if", inputFolder);
			m_InputFolder = Filesystem::unifyPath(inputFolder);
			std::cout << "            Input folder: " << m_InputFolder.string() << "\n";

			if (m_HasOutput)
			{
				success &= options.get<std::string>("of", outputFolder);
				m_OutputFolder = Filesystem::unifyPath(outputFolder);
				std::cout << "           Output folder: " << m_OutputFolder.string() << "\n";
			}

			options.get<int>("timeout", m_WaitTimeout);
			std::cout << "   Wait for file timeout: ";
			if (m_WaitTimeout != 0)
				std::cout << m_WaitTimeout << " s\n";
			else
				std::cout << "infinity\n";

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
		if (!utils::Filesystem::exists(m_InputFolder) || !boost::filesystem::is_directory(m_InputFolder))
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

		if (m_HasOutput)
		{
			try
			{
				boost::filesystem::create_directories(m_OutputFolder);
			}
			catch (...)
			{
				std::cout << "PipeProcess::start(): Can't create output directory " << m_OutputFolder << std::endl;
				return false;
			}
		}
		m_FileSystemWatcher.addPath(QString::fromStdString(m_InputFolder.string()));
		connect(&m_FileSystemWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(onInputDirChanged(const QString&)));
		connect(this, SIGNAL(finished()), this, SLOT(onFinished()), Qt::QueuedConnection);

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
		{
			if (m_WaitTimeout > 0)
			{
				const boost::chrono::seconds timeout(m_WaitTimeout);
				if (m_WaitCondition.wait_for(lock, timeout) == boost::cv_status::timeout)
					emit finished();
			}
			else
			{
				m_WaitCondition.wait(lock);
			}
		}
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
#ifndef _DEBUG
			try
			{
#endif
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
					emit finished();
					return;
				}
#ifndef _DEBUG
			}
			catch (const std::exception& e)
			{
				std::cerr << "PipeProcess::stateLoop() error: " << e.what() << std::endl;
				std::cout << "Quitting..." << std::endl;
				emit finished();
				return;
			}
			catch (...)
			{
				std::cerr << "PipeProcess::stateLoop() Unknown error." << std::endl;
				std::cout << "Quitting..." << std::endl;
				emit finished();
				return;
			}
#endif
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
		if (m_PatternExtension.empty())
		{
			// pattern not specified.
			setState(INIT);
			return;
		}

		FilesystemPath file;
		if (PipeProcessUtils::getFirstFile(file, m_InputFolder, m_PatternExtension, m_InputNumIndices))
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

	void PipeProcess::onFinished()
	{
		Application::getInstance()->exit();
	}

}