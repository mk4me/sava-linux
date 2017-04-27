#pragma once

#include <map>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "MonitorVideoManager.h"
#include "MonitorAlert.h"

#include <deque>

/// <summary>
/// Klasa zarządza zapisem alertów na dysk. Dzieje się to w osobnym wątku, który ów klasa uruchamia.
/// </summary>
class MonitorAlertSaver
{

public:
	MonitorAlertSaver();
	~MonitorAlertSaver();

	void setSavePath(const std::string& path) { m_SavePath = path; }

	void start();
	void stop();

	void add(const std::string& fileName, const std::shared_ptr<sequence::MetaVideo>& video);
	void add(const std::string& fileName, const std::shared_ptr<MonitorAlert>& alert);
	void add(const std::vector< std::pair<std::string, std::shared_ptr<MonitorAlert>>>& alerts);
	void save();

private:
	void loop();

private:
	std::string m_SavePath;
	bool m_IsRunning;

	boost::mutex m_Mutex;
	boost::thread m_Thread;
	boost::condition_variable m_WaitCondition;

	std::deque<std::pair<std::string, std::shared_ptr<sequence::MetaVideo>>> m_Videos;
	std::deque<std::pair<std::string, std::shared_ptr<MonitorAlert>>> m_Alerts;
};

