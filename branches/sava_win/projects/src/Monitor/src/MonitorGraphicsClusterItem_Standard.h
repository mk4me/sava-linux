#pragma once

#include "MonitorGraphicsClusterItem.h"

#include <QtGui/QPen>
#include <QtGui/QFont>

class MonitorGraphicsClusterItem_Standard :
	public MonitorGraphicsClusterItem
{

	struct GraphicsParams
	{
		QPen areaPen;
		QPen alertPen;
		QFont textFont;
	};

public:
	MonitorGraphicsClusterItem_Standard(QGraphicsItem *parent = nullptr);
	
protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private:
	static GraphicsParams s_GraphicsParams;
	static GraphicsParams createGraphicsParams();
};

