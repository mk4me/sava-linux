#include "MonitorGraphicsTimeItem.h"
#include "MonitorVideoManager.h"


MonitorGraphicsTimeItem::MonitorGraphicsTimeItem(QGraphicsItem* parent /*= 0*/)
	:MonitorGraphicsTextItem(parent)
{

}

void MonitorGraphicsTimeItem::update(size_t _frame)
{
	auto time = getTime(_frame);
	QString text = timeToString(time);
	setText(text);

	MonitorGraphicsTextItem::update(_frame);
}


boost::posix_time::ptime MonitorGraphicsTimeItem::getTime(size_t _frame) const
{
	return MonitorVideoManager::getInstance().getMetaVideo()->getVideo()->getFrameTime(_frame);
}


QString MonitorGraphicsTimeItem::timeToString(const boost::posix_time::ptime& _time) {
	QString s_time = boost::posix_time::to_simple_string(_time).c_str();
	s_time = s_time.left(s_time.lastIndexOf(".") + 4);
	return s_time;
}

