#include "MonitorAlertSaver.h"


MonitorAlertSaver::MonitorAlertSaver()
	:m_SaveEnabled(false)
	,m_IsRunning(true)
{

}


MonitorAlertSaver::~MonitorAlertSaver()
{
	stop();
}

void MonitorAlertSaver::start()
{
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

	auto it = m_Videos.find(fileName);
	if (it == m_Videos.end())
		m_Videos.insert(std::make_pair(fileName, video));
}


void MonitorAlertSaver::add(const std::string& fileName, const std::shared_ptr<MonitorAlert>& alert)
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);

	auto it = m_Alerts.find(fileName);
	if (it == m_Alerts.end())
		m_Alerts.insert(std::make_pair(fileName, alert));
}

void MonitorAlertSaver::save()
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);

	m_SaveEnabled = true;
	m_WaitCondition.notify_all();
}

void MonitorAlertSaver::loop()
{
	while (m_IsRunning)
	{
		waitForSave();

		if (!m_Videos.empty() || !m_Alerts.empty() )
		{
			boost::lock_guard<boost::mutex> lock(m_Mutex);

			for (auto mapElem : m_Videos) {
				mapElem.second->getVideo()->save(m_SavePath + "/" + mapElem.first);
				MonitorPipe::getInstance().pushDeleteVideo(mapElem.second);
			}


			for (auto mapElem : m_Alerts) {
				mapElem.second->save(m_SavePath + "/" + mapElem.first);
				mapElem.second->saved();
			}

			m_Videos.clear();
			m_Alerts.clear();
		}

		m_SaveEnabled = false;
	}
}

void MonitorAlertSaver::waitForSave()
{
	boost::mutex::scoped_lock lock(m_Mutex);
	if (!m_SaveEnabled)
		m_WaitCondition.wait(lock);
}
