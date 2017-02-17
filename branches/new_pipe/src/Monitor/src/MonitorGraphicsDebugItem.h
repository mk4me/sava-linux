#pragma once
#include "QtWidgets/QGraphicsItem"
#include "sequence/IVideo.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include "MonitorGraphicsTextItem.h"

class MonitorGraphicsDebugItem : public QGraphicsItem
{

public:
	MonitorGraphicsDebugItem(QGraphicsItem* parent = 0);

	void init();
	void update(size_t _frame);

protected:
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override {};

private:
	MonitorGraphicsTextItem* m_OffsetItem;
	MonitorGraphicsTextItem* m_QueueItem;
	MonitorGraphicsTextItem* m_SpeedItem;
};

