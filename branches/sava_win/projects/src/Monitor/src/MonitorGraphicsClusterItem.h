#ifndef MONITORGRAPHICSGRAPNICSITEM_H
#define MONITORGRAPHICSGRAPNICSITEM_H

#include <QtWidgets/QGraphicsItem>
#include <QtGui/QPainter>

#include "sequence/Cluster.h"
#include "sequence/Action.h"

#include "MonitorVideoManager.h"
#include "MonitorActionManager.h"
#include "MonitorRegionsManager.h"

class MonitorGraphicsClusterItem : public QGraphicsItem
{

	friend class MonitorGraphicsClusters;

public:
	MonitorGraphicsClusterItem(QGraphicsItem *parent = nullptr);
	virtual ~MonitorGraphicsClusterItem() {}

	int getId() const { return m_Id; };

	void setModified(bool _modified) { m_IsModified = _modified; }
	bool isModified() const { return m_IsModified; }

protected:
	virtual QRectF boundingRect() const override;
	virtual void init(const MonitorVideoManager::ActionPair& _source);
	virtual void update(size_t _frame);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) = 0;


	QRectF m_Rect;
	int m_Id;
	bool m_ShowArea;
	bool m_ShowAlert;
	QString m_ActionName;

private:
	MonitorVideoManager::ActionPair m_Source;
	bool m_IsModified;

	static MonitorActionManager* s_ActionManager;
	static MonitorConfig* s_Config;
	static MonitorRegionsManager* s_RegionManager;
	
};

#endif // MONITORGRAPHICSGRAPNICSITEM_H
