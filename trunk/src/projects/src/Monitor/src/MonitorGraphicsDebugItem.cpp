#include "MonitorGraphicsDebugItem.h"
#include "MonitorVideoManager.h"

#include <QtWidgets/QGraphicsView>
#include <QtCore/QDebug>


MonitorGraphicsDebugItem::MonitorGraphicsDebugItem(QGraphicsItem* parent /*= 0*/)
	:QGraphicsItem(parent)
{
	m_OffsetItem = new MonitorGraphicsTextItem(this);
	m_OffsetItem->setRect(QRect(0, 0, 250, 25));
	m_OffsetItem->setAlignmentFlag(Qt::AlignLeft);

	m_QueueItem = new MonitorGraphicsTextItem(this);
	m_QueueItem->setRect(QRect(0, 30, 250, 25));
	m_QueueItem->setAlignmentFlag(Qt::AlignLeft);

	m_SpeedItem = new MonitorGraphicsTextItem(this);
	m_SpeedItem->setRect(QRect(0, 60, 250, 25));
	m_SpeedItem->setAlignmentFlag(Qt::AlignLeft);
}

void MonitorGraphicsDebugItem::init()
{
	m_OffsetItem->init();
	m_QueueItem->init();
	m_SpeedItem->init();
}

void MonitorGraphicsDebugItem::update(size_t _frame)
{

	//offset
	auto offset = MonitorVideoManager::getInstance().getOffsetTime();
	QString offsetText = boost::posix_time::to_simple_string(offset).c_str();
	m_OffsetItem->setText(QString("Offset: %1").arg(offsetText));
	

	//quene size
	size_t queneSize = MonitorVideoManager::getInstance().getQueneSpeeder().getQueueSize();
	m_QueueItem->setText(QString("Quene Size: %1").arg(queneSize));


	//frame speed
	float speed = MonitorVideoManager::getInstance().getQueneSpeeder().getSpeed();
	m_SpeedItem->setText(QString("Frame Speed: %1").arg(speed));


	m_OffsetItem->update(_frame);
	m_QueueItem->update(_frame);
	m_SpeedItem->update(_frame);

}


QRectF MonitorGraphicsDebugItem::boundingRect() const{
	return QRectF();
}

