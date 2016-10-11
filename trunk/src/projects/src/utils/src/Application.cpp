#include "Application.h"
#include "ProgramOptions.h"

#include <QtCore/QCoreApplication>
#include "Version.h"

#ifdef WIN32
#include <Windows.h>
static BOOL exitHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		utils::Application::getInstance()->exit();
		return TRUE;

	case CTRL_C_EVENT:
		utils::Application::getInstance()->exit();
		return TRUE;

	default:
		return FALSE;
	}
}
#endif // WIN32

void utils::Application::registerExitFunction()
{
#ifdef WIN32
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)exitHandler, TRUE))
		std::cerr << "Cant register exit function!" << std::endl;
#else
//#error Not implemented yet
#endif // WIN32
}

utils::Application* utils::Application::getInstance()
{
	static Application app;
	return &app;
}

int utils::Application::runApp(int argc, const char* argv[])
{
	std::cout << "Version: " << Version::getFullVersion() << std::endl;

	QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");

	if (!recognizeParameters(argc, argv))
		return -1;

	if (m_Module != nullptr && m_Module->start())
		return m_QApp->exec();

	return 0;
}

int utils::Application::run(int argc, const char* argv[], const std::shared_ptr<QCoreApplication>& app)
{
	if (m_QApp)
		return -1;
	m_QApp = app;
	return runApp(argc, argv);
}

void utils::Application::exit()
{
	if (m_Module)
		m_Module->stop();

	if (m_QApp)
		m_QApp->quit();
}


bool utils::Application::recognizeParameters(int argc, const char* argv[])
{
	try
	{
		m_ProgramOptions.parse(argc, argv);

		if (m_ModulesMap.size() == 1)
		{
			m_Module.reset(m_ModulesMap.begin()->second());
		}
		else
		{
			for (auto it = m_ModulesMap.begin(); it != m_ModulesMap.end(); ++it)
			{
				if (m_ProgramOptions.exists(it->first))
				{
					m_Module.reset(it->second());
				}
			}
			if (!m_Module && !m_DefaultModule.empty())
				m_Module.reset(m_ModulesMap[m_DefaultModule]());
		}
		if (m_Module)
		{
			bool success = m_Module->loadParameters(m_ProgramOptions);
			if (success)
				return true;

			m_Module.reset();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (...) {}

	std::cout << m_ProgramOptions.printUsage();
	return false;
};

void utils::Application::setAppTitle(const std::string & title)
{
	m_ProgramOptions.setExecutable(title);
}
