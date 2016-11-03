#include "MonitorActionAlert.h"
#include "MonitorFrames.h"
#include "config/Glossary.h"


MonitorActionAlert::MonitorActionAlert(const MonitorVideoManager::ActionPair& i_Data)
	:MonitorAlertData<MonitorVideoManager::ActionPair>(i_Data){
	
}


MonitorActionAlert::MonitorActionAlert(const std::string& filename)
	:MonitorAlertData<MonitorVideoManager::ActionPair>(filename){
	
}

MonitorActionAlert::MonitorActionAlert()
	: MonitorAlertData<MonitorVideoManager::ActionPair>()
{

}

MonitorActionAlert::~MonitorActionAlert()
{
}


void MonitorActionAlert::start()
{
	std::string name = config::Glossary::getInstance().getTrainedActionName(m_Data.second->getActionId());
	setName(name);

	Timestamp startTime = MonitorVideoManager::getInstance().getVideo()->getFrameTime(m_Data.first->getStartFrame());
	setStartTime(startTime);

	MonitorAlert::start();
}

void MonitorActionAlert::finish()
{
	Timestamp endTime = MonitorVideoManager::getInstance().getVideo()->getFrameTime(m_Data.first->getEndFrame());
	setEndTime(endTime);

	MonitorAlert::finish();
}

QPolygonF MonitorActionAlert::getRegion(size_t _frame)
{
	cv::Rect roi = m_Data.first->getFrameAt(_frame);
	QRectF qRect(roi.x, roi.y, roi.width, roi.height);

	QPolygonF polygon;
	polygon << qRect.topLeft();
	polygon << qRect.topRight();
	polygon << qRect.bottomRight();
	polygon << qRect.bottomLeft();

	return polygon;
}

QPolygonF MonitorActionAlert::getRegion()
{
	auto cluster = m_Data.first;
	cv::Rect roi = cluster->getFrameAt(cluster->getStartFrame());
	for (auto i = cluster->getStartFrame() + 1; i < cluster->getEndFrame(); ++i)
	{
		roi = roi | cluster->getFrameAt(i);
	}

	QRectF qRect(roi.x, roi.y, roi.width, roi.height);

	QPolygonF polygon;
	polygon << qRect.topLeft();
	polygon << qRect.topRight();
	polygon << qRect.bottomRight();
	polygon << qRect.bottomLeft();

	return polygon;
}