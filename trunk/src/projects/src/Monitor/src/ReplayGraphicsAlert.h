#pragma once
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include "MonitorAlert.h"

/// <summary>
/// Klasa wywietlaj¹ca alert w oknie podgl¹du alertów.
/// </summary>
class ReplayGraphicsAlert : public QGraphicsItem
{

public:
	ReplayGraphicsAlert(QGraphicsItem * parent = 0);
	~ReplayGraphicsAlert();

	void setAlert(const MonitorAlertPtr& _alert);
	void update(size_t _frame);

protected:
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private:
	MonitorAlertPtr m_Alert;
	QPolygonF m_Polygon;
	QPen m_PolygonPen;
};

