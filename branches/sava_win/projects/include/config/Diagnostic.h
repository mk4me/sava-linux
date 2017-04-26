#pragma once
#ifndef Diagnostic_h__
#define Diagnostic_h__

#include <string>

namespace boost
{
	namespace serialization
	{
		class access;
	}
}

namespace config
{
	class Diagnostic
	{
	public:
		static Diagnostic& getInstance();

		bool load();
		bool save() const;

		int getDebugHistory() const { return m_DebugHistory; }
		void setDebugHistory(int history) { m_DebugHistory = history; }

		bool getLogToFile() const { return m_LogToFile; }
		void setLogToFile(bool enabled) { m_LogToFile = enabled; }

		bool getLogMemoryUsage() const { return m_LogMemoryUsage; }
		void setLogMemoryUsage(bool enabled) { m_LogMemoryUsage = enabled; }

		bool getLogMonitorQueue() const { return m_LogMonitorQueue; }
		void setLogMonitorQueue(bool enabled) { m_LogMonitorQueue = enabled; }

		bool getLogProcessTime() const { return m_LogProcessTime; }
		void setLogProcessTime(bool enabled) { m_LogProcessTime = enabled; }

		bool getShowMonitorDiagnostics() const { return m_ShowMonitorDiagnostics; }
		void setShowMonitorDiagnostics(bool enabled) { m_ShowMonitorDiagnostics = enabled; }

	private:
		Diagnostic();
		~Diagnostic() { }

		int m_DebugHistory;
		bool m_LogToFile;
		bool m_LogMemoryUsage;
		bool m_LogMonitorQueue;
		bool m_LogProcessTime;
		bool m_ShowMonitorDiagnostics;

		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // Diagnostic_h__
