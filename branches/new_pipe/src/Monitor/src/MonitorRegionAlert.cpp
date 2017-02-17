#include "MonitorRegionAlert.h"
#include "MonitorFrames.h"


MonitorRegionAlert::MonitorRegionAlert(const MonitorRegionPtr& i_Data)
	:MonitorAlertData<MonitorRegionPtr>(i_Data)
{
	initRegion();
}


MonitorRegionAlert::MonitorRegionAlert(const std::string& filename) 
	: MonitorAlertData<MonitorRegionPtr>(filename)
{
	initRegion();
}

MonitorRegionAlert::MonitorRegionAlert()
	: MonitorAlertData<MonitorRegionPtr>()
{
	initRegion();
}

MonitorRegionAlert::~MonitorRegionAlert()
{
}

void MonitorRegionAlert::initRegion()
{
	if (m_Data)
		m_Region = *m_Data.get();
}


void MonitorRegionAlert::start()
{
	setName("Region Collision");

	Timestamp startTime = MonitorVideoManager::getInstance().getMetaVideo()->getVideo()->getFrameTime(MonitorFrames::getInstance().getCurrentFrame());
	setStartTime(startTime);

	MonitorAlert::start();
}

void MonitorRegionAlert::finish()
{
	Timestamp endTime = MonitorVideoManager::getInstance().getMetaVideo()->getVideo()->getFrameTime(MonitorFrames::getInstance().getCurrentFrame());
	setEndTime(endTime);

	MonitorAlert::finish();
}

QPolygonF MonitorRegionAlert::getRegion(size_t _frame) {
	return m_Region.getPolygon();
}

QPolygonF MonitorRegionAlert::getRegion() {
	return m_Region.getPolygon();
}


