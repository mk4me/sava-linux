#ifndef scheduler_h__
#define scheduler_h__

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/version.hpp"
#include "boost/asio.hpp"

#include "QtCore/QProcess"

#include <iostream>
#include <string>
#include <set>

namespace aq_qtutils
{
	class CActionProcess
	{
	public:
		typedef std::pair<std::string, std::string> Parameter;
		typedef std::vector<Parameter> ParameterList;

		CActionProcess() { }
		CActionProcess(const std::string& application) : m_Application(application) { }

		void addParameter(const std::string& name, const std::string& value) { m_Parameters.push_back(Parameter(name, value)); }
		void removeParameter(int index) { m_Parameters.erase(m_Parameters.begin() + index); }
		Parameter& parameter(int index) { return m_Parameters[index]; }
		int parameterCount() const { return static_cast<int>(m_Parameters.size()); }

		const std::string& getApplication() const { return m_Application; }

		const std::string& getMachineIp() const { return m_MachineIp; }
		void setMachineIp(const std::string& machineIp) { m_MachineIp = machineIp; }

	private:
		std::string m_Application;
		std::string m_MachineIp;
		ParameterList m_Parameters;

		friend class boost::serialization::access;

		template<class Archive>
		void save(Archive & ar, const unsigned int version) const
		{
			ar << m_Application;
			ar << m_MachineIp;
			ar << m_Parameters;
		}

		template<class Archive>
		void load(Archive & ar, const unsigned int version)
		{
			ar >> m_Application;
			ar >> m_MachineIp;
			ar >> m_Parameters;
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER();
	};

	class CAction
	{
	public:
		typedef std::vector<CActionProcess>::iterator ProcessIterator;

		CAction() { }
		CAction(const std::string& name) : m_Name(name) { }
		CAction(const std::string& name, const std::string& startTime, const std::string& endTime);

		ProcessIterator processBegin() { return m_Processes.begin(); }
		ProcessIterator processEnd() { return m_Processes.end(); }
		void addProcess(const CActionProcess& action) { m_Processes.push_back(action); }
		void removeProcess(int index) { m_Processes.erase(m_Processes.begin() + index); }
		CActionProcess& process(int index) { return m_Processes[index]; }

		const std::string& getName() const { return m_Name; }
		void setName(const std::string& name) { m_Name = name; }

		std::string startTimeToString() const;
		void startTimeFromString(const std::string& time);
		const boost::posix_time::ptime& getStartTime() const { return m_StartTime; }

		std::string endTimeToString() const;
		void endTimeFromString(const std::string& time);
		const boost::posix_time::ptime& getEndTime() const { return m_EndTime; }

	private:
		std::string m_Name;
		std::vector<CActionProcess> m_Processes;

		boost::posix_time::ptime m_StartTime;
		boost::posix_time::ptime m_EndTime;

		friend class boost::serialization::access;

		template<class Archive>
		void save(Archive & ar, const unsigned int version) const
		{
			ar << m_Name;
			ar << startTimeToString();
			ar << endTimeToString();
			ar << m_Processes;
		}

		template<class Archive>
		void load(Archive & ar, const unsigned int version)
		{
			std::string startTime, endTime;

			ar >> m_Name;
			ar >> startTime;
			ar >> endTime;
			ar >> m_Processes;

			startTimeFromString(startTime);
			endTimeFromString(endTime);
		}

		std::string timeToString(const boost::posix_time::ptime& time) const;

		BOOST_SERIALIZATION_SPLIT_MEMBER();
	};

	class CAqScheduler
	{
	public:
		typedef std::vector<CAction> actionsList;
		typedef actionsList actionsTemplateList;

		CAqScheduler() { }
		~CAqScheduler() { }

		void run();
		void stop();

		const actionsTemplateList& getActionsTemplateList() const { return m_ActionTemplates; }
		void setActionsTemplateList(const actionsTemplateList& actionsTemplates) { m_ActionTemplates = actionsTemplates; }

		const actionsList& getActionsList() const { return m_Actions; }
		void setActionsList(const actionsList& actions) { m_Actions = actions; }

	private:
		typedef std::vector<qint64> processList;
		typedef std::vector<std::shared_ptr<boost::asio::deadline_timer>> timerList;

		actionsTemplateList m_ActionTemplates;
		actionsList m_Actions;

		boost::asio::io_service m_Service;
		timerList m_Timers;
		std::set<std::string> m_LocalIpList;

		void runAction(boost::system::error_code ec, size_t timer, size_t action);
		void endAction(boost::system::error_code ec, processList* processes);

		static qint64 startProcess(QString processName, QStringList arguments);
		static void terminateProcess(qint64 pid);

		void findLocalIp();

		friend class boost::serialization::access;

		template<class Archive>
		void save(Archive & ar, const unsigned int version) const
		{
			ar << m_ActionTemplates;
			ar << m_Actions;
		}

		template<class Archive>
		void load(Archive & ar, const unsigned int version)
		{
			if (version < 1)
				return;

			ar >> m_ActionTemplates;
			ar >> m_Actions;
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER();

		//std::vector<rd_aquisition::CCameraRetriever*> m_retrievers;
	};

	class CQtUtils
	{
	public:
		static CQtUtils* getInstance();

		CAqScheduler* Scheduler() const { return m_scheduler; }
		void Scheduler(CAqScheduler* val);

	private:
		CAqScheduler* m_scheduler;

		CQtUtils()
		{
			m_scheduler = new CAqScheduler();
		};

		~CQtUtils()
		{
			if (m_scheduler != nullptr) {
				delete m_scheduler;
			}
		}

		CQtUtils(const CQtUtils& cpy);
		CQtUtils& operator= (const CQtUtils& rhs);
	};

	class CDataLoader
	{
	public:
		static void saveAqScheduler();
		static void loadAqScheduler();
	};

} //namespace aq_qtutils

BOOST_CLASS_VERSION(aq_qtutils::CAqScheduler, 1)

#endif // scheduler_h__
