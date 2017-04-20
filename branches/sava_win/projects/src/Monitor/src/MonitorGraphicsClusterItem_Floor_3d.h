#pragma once

#include "MonitorGraphicsClusterItem_Fill_2d.h"

#include <QtGui/QPainter>
#include <QtCore/QVector>


class MonitorGraphicsClusterItem_Floor_3d :
	public MonitorGraphicsClusterItem_Fill_2d
{

	enum BoxType {
		FLOOR,
		WALL,
	};

public:
	MonitorGraphicsClusterItem_Floor_3d(QGraphicsItem* parent = nullptr);

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

	QVector<QPointF> createBoxVertexes();

private:
	void drawWall(QVector<QPointF>& points, BoxType boxType, QPainter* painter);

};

