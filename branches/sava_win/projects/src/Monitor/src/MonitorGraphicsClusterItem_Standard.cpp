#include "MonitorGraphicsClusterItem_Standard.h"


MonitorGraphicsClusterItem_Standard::MonitorGraphicsClusterItem_Standard(QGraphicsItem *parent)
	:MonitorGraphicsClusterItem(parent)
{
}

void MonitorGraphicsClusterItem_Standard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	painter->setRenderHint(QPainter::Antialiasing);

	if (m_ShowAlert)
	{
		painter->setPen(s_GraphicsParams.alertPen);
		painter->setFont(s_GraphicsParams.textFont);

		QRectF boundingBox;
		painter->setOpacity(0);
		painter->drawText(QRectF(0, 0, 100, 20), Qt::AlignVCenter, m_ActionName, &boundingBox);
		painter->setOpacity(1);

		boundingBox.moveTo(m_Rect.x(), m_Rect.y() - boundingBox.height() - 6);

		//draw text bounding box
		QPainterPath path;
		path.addRoundedRect(boundingBox.adjusted(-3, -3, 3, 3), 3, 3);
		painter->setOpacity(0.7);
		painter->fillPath(path, Qt::black);
		painter->setOpacity(1);

		//draw text
		painter->drawText(boundingBox, Qt::AlignVCenter, m_ActionName);
		painter->drawRect(m_Rect);

	}
	else if(m_ShowArea)
	{
		painter->setPen(s_GraphicsParams.areaPen);
		painter->drawRect(m_Rect);
	}
}




MonitorGraphicsClusterItem_Standard::GraphicsParams MonitorGraphicsClusterItem_Standard::createGraphicsParams()
{
	GraphicsParams p;

	p.areaPen.setColor(Qt::yellow);
	p.areaPen.setWidth(2);
	p.alertPen.setColor(Qt::red);
	p.alertPen.setWidth(2);
	p.textFont.setPixelSize(14);

	return p;
}

MonitorGraphicsClusterItem_Standard::GraphicsParams MonitorGraphicsClusterItem_Standard::s_GraphicsParams = createGraphicsParams();

