#ifndef Application_h__
#define Application_h__

#include "IAppModule.h"
#include "MemoryLog.h"

#include <QtWidgets/QApplication>

#include <map>
#include <memory>

namespace utils
{
	class Application
	{
	public:
		static Application* getInstance();

		static void registerExitFunction();

		template <typename T>
		int run(int& argc, const char* argv[])
		{
			if (m_QApp)
				return 0;

			createApplication<T>(argc, argv);
			return runApp(argc, argv);
		}
		int run(int& argc, const char* argv[]) { return run<QCoreApplication>(argc, argv); }
		int run(int& argc, const char* argv[], const std::shared_ptr<QCoreApplication>& app);
		void exit();

		void setAppTitle(const std::string & title);

		template<typename T>
		void registerModule(const std::string& launchOption, const std::string& description, bool isDefault = false)
		{
			m_ModulesMap.insert(ModuleEntry(launchOption, &T::create));
			m_ProgramOptions.add(launchOption, description);
			T::registerParameters(m_ProgramOptions);
			if (isDefault)
			{
				assert(m_DefaultModule.empty());
				m_DefaultModule = launchOption;
			}
		}

		void enableMomoryLogging();

	private:
		typedef std::map<std::string, IAppModule::CreateFunction> ModulesMap;
		typedef ModulesMap::value_type ModuleEntry;

		std::shared_ptr<IAppModule> m_Module;
		ModulesMap m_ModulesMap;
		std::string m_DefaultModule;

		ProgramOptions m_ProgramOptions;
		std::shared_ptr<QCoreApplication> m_QApp;

		Application() { }
		~Application() { }

		bool recognizeParameters(int& argc, const char* argv[]);
		int runApp(int& argc, const char* argv[]);

		template<typename T>
		void createApplication(int& argc, const char* argv[])
		{
			m_QApp.reset(new T(argc, const_cast<char**>(argv)));
		}

		std::shared_ptr<MemoryLog> m_MemoryLog;
	};
}

#endif // Application_h__
