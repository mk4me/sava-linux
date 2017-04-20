#ifndef MONITORGRAPHICSCLUSTERS_H
#define MONITORGRAPHICSCLUSTERS_H

#include <QtWidgets/QGraphicsItem>
#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>

#include "config/Glossary.h"
#include "sequence/Cluster.h"
#include "sequence/Action.h"

#include "MonitorVideoManager.h"
#include "MonitorActionManager.h"
#include "MonitorGraphicsClusterItem.h"

/// <summary>
/// Obiekt graficzny na scenie, zajmuj¹cy siê wywietlaniem wykrytych obszarów na obrazie wideo, w których system wykry³ akcje.
/// </summary>
class MonitorGraphicsClusters : public QGraphicsItem
{

public:

	enum DecorationType {
		STANDARD,
		FILL_2D,
		FLOOR_3D,
	};

	MonitorGraphicsClusters(QGraphicsItem *parent = 0);

	void init(){}
	void update(size_t _frame);

	void onVideoLoaded();

protected:
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override { } ;

private:
	MonitorGraphicsClusterItem* createDecoratorByType(DecorationType type);

private:
	MonitorVideoManager* m_CachedVideoManger;

	std::vector<MonitorGraphicsClusterItem*> m_Items;
};

#endif // MONITORGRAPHICSCLUSTERS_H
