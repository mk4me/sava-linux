#include "MonitorAlertSaver.h"


MonitorAlertSaver::MonitorAlertSaver()
	:m_IsRunning(false)
{

}


MonitorAlertSaver::~MonitorAlertSaver()
{
	stop();
}

void MonitorAlertSaver::start()
{
	m_IsRunning = true;
	m_Thread = boost::thread(&MonitorAlertSaver::loop, this);
}

void MonitorAlertSaver::stop()
{
	m_IsRunning = false;
	m_WaitCondition.notify_all();

	if (m_Thread.joinable())
		m_Thread.join();
}


void MonitorAlertSaver::add(const std::string& fileName, const std::shared_ptr<sequence::MetaVideo>& video)
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);

	m_Videos.push_back(std::make_pair(fileName, video));
}


void MonitorAlertSaver::add(const std::string& fileName, const std::shared_ptr<MonitorAlert>& alert)
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);

	m_Alerts.push_back(std::make_pair(fileName, alert));
}

void MonitorAlertSaver::add(const std::vector< std::pair<std::string, std::shared_ptr<MonitorAlert>>>& alerts)
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);

	for (auto elem: alerts)
		m_Alerts.push_back(std::make_pair(elem.first, elem.second));
}

void MonitorAlertSaver::save()
{
	m_WaitCondition.notify_all();
}

void MonitorAlertSaver::loop()
{
	static int PACKAGE_COUNT = 200;

	while (m_IsRunning || !m_Videos.empty() || !m_Alerts.empty())
	{
		{
			boost::unique_lock<boost::mutex> lock(m_Mutex);

			if (m_Videos.empty() )
				m_WaitCondition.wait(lock);

			while (!m_Videos.empty())
			{
				auto elem = m_Videos.front();
				m_Videos.pop_front();

				if (!elem.first.empty())
					elem.second->getVideo()->save(m_SavePath + "/" + elem.first);

				MonitorPipe::getInstance().pushDeleteVideo(elem.second);
			}

			int count = 0;
			while (!m_Alerts.empty() && count < PACKAGE_COUNT)
			{
				auto elem = m_Alerts.front();
				m_Alerts.pop_front();

				elem.second->save(m_SavePath + "/" + elem.first);
				elem.second->saved();

				count++;
			}

		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
}
