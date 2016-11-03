#pragma once
#include "QGraphicsItem.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "sequence/IVideo.h"

#define SHOW_DEBUG_TIME 0

class MonitorGraphicsTimeItem : public QObject, public QGraphicsItem
{
	Q_OBJECT;
	Q_INTERFACES(QGraphicsItem)

public:
	MonitorGraphicsTimeItem(QGraphicsItem* parent = 0);

	void init();
	virtual void update(size_t _frame);

	void setRect(const QRect& rect) { m_Rect = rect; }
	void setTime(const boost::posix_time::ptime& _time) { m_Time = _time; }

private slots:
	void onViewSizeChanged(const QSize& newSize);

protected:
	void repaintRect();
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
	virtual QRectF boundingRect() const override;

private:
	QString timeToString(const boost::posix_time::ptime& _time);

	QGraphicsView *m_GraphView;
	boost::posix_time::ptime m_Time;
	QRect m_Rect;
	QRectF m_MappedRect;

#if SHOW_DEBUG_TIME
	QString m_DebugOffsetTime;
	QRectF m_DebugMappedOffsetRect;

	QString offsetTimeToString(const boost::posix_time::ptime& _time);
#endif

};

