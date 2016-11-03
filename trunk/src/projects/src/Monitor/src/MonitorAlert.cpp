#include "MonitorAlert.h"
#include "MonitorAlertManager.h"


MonitorAlert::MonitorAlert()
	: m_Name("")
	, m_State(NONE)
{
	
}


bool MonitorAlert::addVideo(const std::string& _video)
{
	bool insertEnabled = (std::find(m_Videos.begin(), m_Videos.end(), _video) == m_Videos.end());
	if (insertEnabled)
		m_Videos.push_back(_video);

	return insertEnabled;
}

bool MonitorAlert::isValid() const
{
	return isFinished() && !m_Videos.empty();
}

std::string MonitorAlert::timeToString(const Timestamp& _time)
{
	boost::posix_time::time_facet* time_format = new boost::posix_time::time_facet("%H:%M:%S%F");

	std::stringstream ss;
	ss.imbue(std::locale(std::locale::classic(), time_format));
	ss << _time;

	std::string s = ss.str().c_str();
	s.erase(s.length() - 3, 3);

	return s;
}