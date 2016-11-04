#ifndef MONITORGRAPHICSWAITITEM_H
#define MONITORGRAPHICSWAITITEM_H

#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QBrush>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

class MonitorGraphicsWaitItem : public QObject, public QGraphicsItem
{
	Q_OBJECT
	//Q_INTERFACES(QGraphicsItem)

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
