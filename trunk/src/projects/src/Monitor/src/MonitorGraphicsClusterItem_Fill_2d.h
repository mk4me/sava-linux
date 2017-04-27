#pragma once

#include "MonitorGraphicsClusterItem.h"

#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QBrush>

class MonitorGraphicsClusterItem_Fill_2d :
	public MonitorGraphicsClusterItem
{
	struct GraphicsParams
	{
		QColor areaColor;
		QPen areaPen;
		QBrush areaBrush;

		QColor alertColor;
		QPen alertPen;
		QBrush alertBrush;

		QFont textFont;
	};

public:
	MonitorGraphicsClusterItem_Fill_2d(QGraphicsItem *parent = nullptr);

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

	static GraphicsParams s_GraphicsParams;
	static GraphicsParams createGraphicsParams();
};

