#include "scheduler.h"
#include <utils/WindowsUtilities.h>

#include <iostream>
#include <fstream>
#include <cstdio>

#include "QtCore/QStringList"
#include "QtCore/QDir"
#include "QtNetwork/QNetworkInterface"

#include "boost/bind.hpp"

#include "utils/Filesystem.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>

namespace aq_qtutils
{
	CAction::CAction(const std::string& name, const std::string& startTime, const std::string& endTime)
		: m_Name(name)
	{
		startTimeFromString(startTime);
		endTimeFromString(endTime);
	}

	std::string CAction::startTimeToString() const
	{
		return timeToString(m_StartTime);
	}

	void CAction::startTimeFromString(const std::string& time)
	{
		try
		{
			m_StartTime = boost::posix_time::time_from_string(time);
		}
		catch (...) {}
	}

	std::string CAction::endTimeToString() const
	{
		return timeToString(m_EndTime);
	}

	void CAction::endTimeFromString(const std::string& time)
	{
		try
		{
			m_EndTime = boost::posix_time::time_from_string(time);
		}
		catch (...) {}
	}

	std::string CAction::timeToString(const boost::posix_time::ptime& time) const
	{
		boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
		facet->format("%Y-%m-%d %H:%M:%S");
		std::stringstream ss;
		ss.imbue(std::locale(std::locale::classic(), facet));
		ss << time;
		return ss.str();
	}

	void CAqScheduler::run()
	{
		findLocalIp();

		for (size_t i = 0; i < m_Actions.size(); ++i)
		{
			boost::posix_time::ptime currentTime = boost::posix_time::second_clock::local_time();
			if (m_Actions[i].getEndTime() <= currentTime)
				continue;

			if (m_Actions[i].getStartTime() <= currentTime)
			{
				m_Timers.push_back(std::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(m_Service)));
				runAction(boost::system::error_code(), m_Timers.size() - 1, i);
			}
			else
			{
				m_Timers.push_back(std::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(m_Service)));
				m_Timers[m_Timers.size() - 1]->expires_from_now(m_Actions[i].getStartTime() - currentTime);
				m_Timers[m_Timers.size() - 1]->async_wait(boost::bind(&aq_qtutils::CAqScheduler::runAction, this, boost::asio::placeholders::error, m_Timers.size() - 1, i));
			}
		}

		m_Service.run();
	}

	void CAqScheduler::stop()
	{
		for (auto it = m_Timers.begin(); it != m_Timers.end(); ++it)
			(*it)->cancel();

	}

	void CAqScheduler::runAction(boost::system::error_code ec, size_t timer, size_t action)
	{

		if (ec.value() != boost::system::errc::success)
		{
			std::cerr << "CAqScheduler::runAction(): can't run action \"" << m_Actions[action].getName() << "\" at " << m_Actions[action].getStartTime();
			return;
		}

		processList* processes = new processList();
		auto itEnd = m_Actions[action].processEnd();
		for (auto it = m_Actions[action].processBegin(); it != itEnd; ++it)
		{
			if (m_LocalIpList.find(it->getMachineIp()) == m_LocalIpList.end())
				continue;

			QStringList args;
			for (int i = 0; i < it->parameterCount(); ++i)
			{
				if (it->parameter(i).first.empty())
					continue;

				args.append(("--" + it->parameter(i).first).c_str());
				if (!it->parameter(i).second.empty())
					args.append(it->parameter(i).second.c_str());
			}

			QString p = /*QDir::currentPath() + '/' + */it->getApplication().c_str();
			processes->push_back(startProcess(p, args));
		}

		m_Timers[timer]->expires_from_now(m_Actions[action].getEndTime() - boost::posix_time::second_clock::local_time());
		m_Timers[timer]->async_wait(boost::bind(&aq_qtutils::CAqScheduler::endAction, this, boost::asio::placeholders::error, processes));
	}

	void CAqScheduler::endAction(boost::system::error_code ec, processList* processes)
	{
		for (auto it = processes->begin(); it != processes->end(); ++it)
			terminateProcess(*it);

		delete processes;
	}

	qint64 CAqScheduler::startProcess(QString processName, QStringList arguments)
	{
		qint64 pid;
		QProcess::startDetached(processName, arguments, processName.left(processName.lastIndexOf('/') + 1), &pid);

		std::cout << "started " << processName.toStdString() << "   id=" << pid << std::endl;
		return pid;
	}

	void CAqScheduler::terminateProcess(qint64 pid)
	{
#ifdef WIN32
		TerminateApp(pid, 10000);
#else
#error Not implemented yet
#endif
	}

	void CAqScheduler::findLocalIp()
	{
		QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
		for (auto it = addresses.begin(); it != addresses.end(); ++it)
			m_LocalIpList.insert(it->toString().toStdString());
	}

	CQtUtils* CQtUtils::getInstance()
	{
		static CQtUtils instance;
		return &instance;
	}

	void CQtUtils::Scheduler(CAqScheduler* val)
	{
		delete m_scheduler;
		m_scheduler = val;
	}

	void CDataLoader::saveAqScheduler()
	{
		//FIXME - gdzie to ma byc zapisywane?
		const std::string filerPath = utils::Filesystem::getDataPath() + "scheduler.aq";
 		//m_pEstimationSet->loadCloadFromFile(pWorkinPath.getApplicationDataPath ("tempcloud.cld", "camera/navigation"));
		std::ofstream ofsFile(filerPath);
 		//boost::archive::text_iarchive ia(ifsFile);
 		boost::archive::text_oarchive oa(ofsFile);
 		oa << *aq_qtutils::CQtUtils::getInstance()->Scheduler();
	}

	void CDataLoader::loadAqScheduler()
	{
		//FIXME - gdzie to ma byc zapisywane?
		const std::string filerPath = utils::Filesystem::getDataPath() + "scheduler.aq";
		std::ifstream ifsFile(filerPath);

		if (!ifsFile.bad()) {
			try {
				boost::archive::text_iarchive ia(ifsFile);
				CAqScheduler *as = new CAqScheduler();
				ia >> *as;
				aq_qtutils::CQtUtils::getInstance()->Scheduler(as);
			}
			catch (...) {
			}

		}
	}

}	//namespace aq_qtutils
