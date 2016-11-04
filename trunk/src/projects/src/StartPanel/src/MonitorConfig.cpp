#include "MonitorConfig.h"

#include "config/Monitor.h"

MonitorConfig::MonitorConfig(QWidget *parent)
{
	ui.setupUi(this);
}

MonitorConfig::~MonitorConfig()
{

}

bool MonitorConfig::load()
{
	config::Monitor& monitorConfig = config::Monitor::getInstance();
	monitorConfig.load();

	ui.m_AlertsTime->blockSignals(true);
	ui.m_AlertsTime->setValue(monitorConfig.getAlertsLifetime());
	ui.m_AlertsTime->blockSignals(false);

	ui.m_MaxAlertsCount->blockSignals(true);
	ui.m_MaxAlertsCount->setValue(monitorConfig.getMaxAlertsCount());
	ui.m_MaxAlertsCount->blockSignals(false);

	return true;
}

bool MonitorConfig::save()
{
	return config::Monitor::getInstance().save();
}

void MonitorConfig::onAlertsTimeChanged(int time)
{
	config::Monitor::getInstance().setAlertsLifetime(time);
}

void MonitorConfig::onMaxAlertsChanged(int count)
{
	config::Monitor::getInstance().setMaxAlertsCount(count);
}
