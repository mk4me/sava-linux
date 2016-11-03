#include "ReplayGraphicsTime.h"


ReplayGraphicsTime::ReplayGraphicsTime(QGraphicsItem* parent)
	:MonitorGraphicsTimeItem(parent)
	, m_Video(nullptr)
{
}


ReplayGraphicsTime::~ReplayGraphicsTime()
{
}

void ReplayGraphicsTime::update(size_t _frame)
{
	if (!m_Video)
		return;

	setTime(m_Video->getFramesTimes()[_frame]);
	repaintRect();
}
