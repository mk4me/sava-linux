#include "ReplayGraphicsAlert.h"


ReplayGraphicsAlert::ReplayGraphicsAlert(QGraphicsItem * parent)
	: QGraphicsItem(parent)
	, m_Alert(nullptr)
{
	m_PolygonPen.setColor(QColor(255, 0, 0, 255));
	m_PolygonPen.setWidth(4);
}


ReplayGraphicsAlert::~ReplayGraphicsAlert()
{
}

void ReplayGraphicsAlert::setAlert(const MonitorAlertPtr& _alert)
{
	m_Alert = _alert;
}

void ReplayGraphicsAlert::update(size_t _frame)
{
	if (m_Alert)
	{
		m_Polygon = m_Alert->getRegion(_frame);
	}
}

QRectF ReplayGraphicsAlert::boundingRect() const
{
	if (scene())
		return scene()->sceneRect();
	return QRectF();
}

void ReplayGraphicsAlert::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	if (m_Alert)
	{
		painter->setRenderHint(QPainter::Antialiasing);
		painter->setPen(m_PolygonPen);
		painter->drawPolygon(m_Polygon);
	}
}
