#include "MonitorGraphicsWaitItem.h"

MonitorGraphicsWaitItem::MonitorGraphicsWaitItem(QGraphicsItem *parent)
	: QGraphicsItem(parent)
	, m_DotsCount(0)
{
	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
	m_Timer.setInterval(300);
	m_Timer.start();
}

void MonitorGraphicsWaitItem::onTimeout()
{
	m_DotsCount = (++m_DotsCount) % 4;
	update();
}

QRectF MonitorGraphicsWaitItem::boundingRect() const
{
	if (scene())
		return scene()->sceneRect();

	return QRectF();
}

void MonitorGraphicsWaitItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	//set font
	QFont font;
	font.setPixelSize(40);
	font.setItalic(true);
	painter->setFont(font);

	//set color
	painter->setPen(QColor(239, 239, 239));

	//set text
	QString text = "Waiting for stream ";
	QRectF textBounding;
	painter->setOpacity(0);
	painter->drawText(scene()->sceneRect(), Qt::AlignCenter, text, &textBounding);
	painter->setOpacity(1);

	for (int i = 0; i < m_DotsCount; i++)
		text += ".";

	textBounding.setWidth(textBounding.width() * 2);
	painter->drawText(textBounding, Qt::AlignLeft, text);
}

QVariant MonitorGraphicsWaitItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == QGraphicsItem::ItemVisibleChange)
	{
		bool isVisible = value.toBool();
		if (isVisible)
			m_Timer.start();
		else
			m_Timer.stop();
	}

	return QGraphicsItem::itemChange(change, value);
}
