#include "MonitorGraphicsTimeItem.h"
#include <QGraphicsView>
#include <QDebug>
#include "MonitorVideoManager.h"


MonitorGraphicsTimeItem::MonitorGraphicsTimeItem(QGraphicsItem* parent /*= 0*/)
	:QGraphicsItem(parent)
	, m_Rect(0, 0, 250, 25)
{
	setVisible(false);
}


void MonitorGraphicsTimeItem::init()
{
	m_GraphView = scene()->views().last();
	connect(m_GraphView, SIGNAL(sizeChanged(QSize)), this, SLOT(onViewSizeChanged(QSize)));

	onViewSizeChanged(QSize());
	setVisible(true);
}

void MonitorGraphicsTimeItem::update(size_t _frame)
{
	setTime(MonitorVideoManager::getInstance().getVideo()->getFrameTime(_frame));
	repaintRect();
	
#if SHOW_DEBUG_TIME
	m_DebugOffsetTime = offsetTimeToString(time);

	QRect m_DebugRect(m_Rect);
	m_DebugRect.moveTop(m_Rect.height() + 5);
	m_DebugMappedOffsetRect = m_GraphView->mapToScene(m_DebugRect).boundingRect();

	scene()->update();
#endif

}

void MonitorGraphicsTimeItem::repaintRect()
{
	scene()->update(m_MappedRect);
}

void MonitorGraphicsTimeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	//draw rectangle
	QFont font;
	font.setPixelSize(m_MappedRect.height() * 0.7);
	painter->setFont(font);
	painter->fillRect(m_MappedRect, Qt::gray);

	//draw text
	painter->setPen(Qt::black);
	painter->drawText(m_MappedRect, Qt::AlignCenter, timeToString(m_Time));

#if SHOW_DEBUG_TIME
	painter->fillRect(m_DebugMappedOffsetRect, Qt::gray);
	painter->drawText(m_DebugMappedOffsetRect, Qt::AlignCenter, m_DebugOffsetTime);
#endif

}


QRectF MonitorGraphicsTimeItem::boundingRect() const {
	return QRectF(0, 0, 0, 0);
}


void MonitorGraphicsTimeItem::onViewSizeChanged(const QSize& newSize){
	QRect real(m_Rect);
	real.moveLeft(m_GraphView->width() - m_Rect.width());
	m_MappedRect = m_GraphView->mapToScene(real).boundingRect();
}

QString MonitorGraphicsTimeItem::timeToString(const boost::posix_time::ptime& _time) {
	QString s_time = boost::posix_time::to_simple_string(_time).c_str();
	s_time = s_time.left(s_time.lastIndexOf(".") + 4);
	return s_time;
}

#if SHOW_DEBUG_TIME
QString MonitorGraphicsTimeItem::offsetTimeToString(const boost::posix_time::ptime& _time) {
	boost::posix_time::time_duration offset =  MonitorVideoManager::getInstance().getOffsetTime();
	QString s_time = boost::posix_time::to_simple_string(offset).c_str();
	s_time = "OFFSET: " + s_time.left(s_time.lastIndexOf(".") + 4);
	return s_time;
}

#endif
