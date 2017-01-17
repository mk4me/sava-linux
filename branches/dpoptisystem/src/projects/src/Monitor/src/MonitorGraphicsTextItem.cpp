#include "MonitorGraphicsTextItem.h"
#include <QtWidgets/QGraphicsView>
#include <QtCore/QDebug>


MonitorGraphicsTextItem::MonitorGraphicsTextItem(QGraphicsItem* parent /*= 0*/)
	:QGraphicsItem(parent)
	, m_Rect(0, 0, 250, 25)
	, m_AlignmentFlag(Qt::AlignRight)
{
	setVisible(false);
}


void MonitorGraphicsTextItem::init()
{
	m_GraphView = scene()->views().last();
	connect(m_GraphView, SIGNAL(sizeChanged(QSize)), this, SLOT(onViewSizeChanged(QSize)));

	onViewSizeChanged(QSize());
	setVisible(true);
}

void MonitorGraphicsTextItem::update(size_t _frame)
{
	repaintRect();
}

void MonitorGraphicsTextItem::repaintRect() {
	scene()->update(m_MappedRect);
}

void MonitorGraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	//draw rectangle
	QFont font;
	font.setPixelSize(m_MappedRect.height() * 0.7);
	painter->setFont(font);
	painter->fillRect(m_MappedRect, Qt::gray);

	//draw text
	painter->setPen(Qt::black);
	painter->drawText(m_MappedRect, Qt::AlignCenter, m_Text);
}


QRectF MonitorGraphicsTextItem::boundingRect() const {
	return QRectF(0, 0, 0, 0);
}


void MonitorGraphicsTextItem::onViewSizeChanged(const QSize& newSize){
	QRect real(m_Rect);

	if (m_AlignmentFlag == Qt::AlignRight)
	{
		real.moveLeft(m_GraphView->width() - m_Rect.width());
	}
	else if (m_AlignmentFlag == Qt::AlignLeft)
	{
		//nothing
	}

	m_MappedRect = m_GraphView->mapToScene(real).boundingRect();
}