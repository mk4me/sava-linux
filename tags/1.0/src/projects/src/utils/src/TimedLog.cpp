#include "TimedLog.h"

#include "Filesystem.h"

#include <QtCore/QCoreApplication>

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fstream>

namespace utils
{

	TimedLog::TimedLog(QObject* parent /*= nullptr*/) : QObject(parent)
	{
	}

	void TimedLog::timeout()
	{
		std::ofstream file(m_Filename, std::ios::app);
		auto time = boost::posix_time::microsec_clock::local_time();
		file << time << ';';
		save(file);
	}

	void TimedLog::startTimer(const std::string& name, float interval)
	{
		std::string memoryDir = Filesystem::unifyPath(Filesystem::getUserPath() + "debugLogs");

		boost::filesystem::create_directories(memoryDir);

		m_Filename = memoryDir + name + "_" + QCoreApplication::applicationName().toStdString() + ".csv";

		std::ofstream(m_Filename, std::ios::out);

		m_Thread = new QThread(this);
		m_Timer = new QTimer(0);
		m_Timer->moveToThread(m_Thread);
		m_Timer->setInterval(interval * 1000);
		connect(m_Timer, SIGNAL(timeout()), this, SLOT(timeout()));
		m_Timer->connect(m_Thread, SIGNAL(started()), SLOT(start()));
		m_Thread->start();
	}

}