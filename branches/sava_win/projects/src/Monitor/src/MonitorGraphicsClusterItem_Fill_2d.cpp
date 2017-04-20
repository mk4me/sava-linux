#include "MonitorGraphicsClusterItem_Fill_2d.h"

MonitorGraphicsClusterItem_Fill_2d::MonitorGraphicsClusterItem_Fill_2d(QGraphicsItem *parent)
	:MonitorGraphicsClusterItem(parent)
{
}

void MonitorGraphicsClusterItem_Fill_2d::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
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

		painter->drawText(boundingBox, Qt::AlignVCenter, m_ActionName);

		painter->fillRect(m_Rect, s_GraphicsParams.alertBrush);
		painter->drawRect(m_Rect);
			
		
	}
	else if (m_ShowArea)
	{
		painter->setPen(s_GraphicsParams.areaPen);
		painter->fillRect(m_Rect, s_GraphicsParams.areaBrush);
		painter->drawRect(m_Rect);
		
	}
}


MonitorGraphicsClusterItem_Fill_2d::GraphicsParams MonitorGraphicsClusterItem_Fill_2d::createGraphicsParams()
{
	GraphicsParams p;

	QColor cArea = Qt::yellow;
	QColor cAlert = Qt::red;
	int alpha = 40;

	p.areaColor = cArea;
	p.alertColor = cAlert;

	p.areaPen.setColor(cArea);
	p.areaPen.setWidth(2);

	cArea.setAlpha(alpha);
	p.areaBrush.setColor(cArea);
	p.areaBrush.setStyle(Qt::SolidPattern);

	p.alertPen.setColor(cAlert);
	p.alertPen.setWidth(2);

	cAlert.setAlpha(alpha);
	p.alertBrush.setColor(cAlert);
	p.alertBrush.setStyle(Qt::SolidPattern);

	p.textFont.setPixelSize(18);
	p.textFont.setBold(true);

	return p;
}

MonitorGraphicsClusterItem_Fill_2d::GraphicsParams MonitorGraphicsClusterItem_Fill_2d::s_GraphicsParams = createGraphicsParams();

