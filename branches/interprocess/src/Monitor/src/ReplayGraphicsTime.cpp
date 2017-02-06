#include "ReplayGraphicsTime.h"


ReplayGraphicsTime::ReplayGraphicsTime(QGraphicsItem* parent)
	:MonitorGraphicsTimeItem(parent)
	, m_Video(nullptr)
{
}


/*ReplayGraphicsTime::~ReplayGraphicsTime()
{
}*/

boost::posix_time::ptime ReplayGraphicsTime::getTime(size_t _frame) const
{
	return m_Video->getFramesTimes()[_frame];
}
