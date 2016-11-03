#ifndef MONITORGRAPHICSWAITITEM_H
#define MONITORGRAPHICSWAITITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QBrush>
#include <QTimer>
#include <QDebug>

class MonitorGraphicsWaitItem : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	MonitorGraphicsWaitItem(QGraphicsItem *parent = 0);

	void init(){}

private slots:
	void onTimeout();

protected:
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

	virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
	QTimer m_Timer;
	ushort m_DotsCount;
	
};

#endif // MONITORGRAPHICSWAITITEM_H
