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

class MonitorGraphicsClusters : public QGraphicsItem
{
	struct ClusterInfo
	{
		QString name;
		QRectF rect;
		bool isAlert;
	};

	static QColor CLUSTER_COLOR;
	static QColor ALERT_COLOR;

public:
	MonitorGraphicsClusters(QGraphicsItem *parent = 0);
	~MonitorGraphicsClusters();

	void init(){}
	void update(size_t _frame);
	void clear();

protected:
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private:
	MonitorVideoManager* m_CachedVideoManger;
	MonitorActionManager* m_CachedActionManager;

	//vector of actions info
	std::vector<ClusterInfo> m_ClustersInfo;

	//pen to draw action rectangle on scene
	QPen m_ActionPen;

	//pen to draw alert rectangle action on scene
	QPen m_AlertPen;

	//font to draw action text on scene
	QFont m_TextFont;
	
};

#endif // MONITORGRAPHICSCLUSTERS_H
