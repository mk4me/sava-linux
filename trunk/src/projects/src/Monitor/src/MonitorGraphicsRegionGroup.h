#ifndef MONITORGRAPHICSALERTREGIONGROUP_H
#define MONITORGRAPHICSALERTREGIONGROUP_H

#include <QtWidgets/QGraphicsItem>
#include <QtGui/QKeyEvent>
#include "MonitorGraphicsRegion.h"
#include "MonitorGraphicsRegionGroup.h"
#include "MonitorVideoManager.h"

#include <vector>


class MonitorGraphicsRegionGroup : public QGraphicsItem
{
	friend class MonitorGraphicsRegion;

public:

	enum EState
	{
		NONE,
		CREATING_NEW_REGION,
	};

	MonitorGraphicsRegionGroup(MonitorRegion::EType regionType, QGraphicsItem *parent = 0);
	~MonitorGraphicsRegionGroup(){};

	void init();
	void update(size_t _frame) { };

	void setState(EState _state);
	EState getState() const { return m_State; }

	void deleteLastRegion();

	void onMouseTracking(QGraphicsSceneMouseEvent *event);
	void onKeyPressEvent(QKeyEvent *event);
	void onMousePressEvent(QGraphicsSceneMouseEvent *event);

private:
	MonitorGraphicsRegion* add(const RegionPoints& i_RegionPoints, MonitorRegionPtr regionSrc = nullptr);
	bool remove(MonitorGraphicsRegion* i_AlertRegion);

	MonitorGraphicsRegion* create(MonitorRegion::EType type, MonitorRegionPtr regionSrc = nullptr);

protected:
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override {}
	virtual void onChanged(MonitorGraphicsRegion* i_ChangedRegion);

private:
	MonitorVideoManager* m_CachedVideoManger;

	QVector<MonitorGraphicsRegion*> m_AlertRegions;
	MonitorGraphicsRegion* m_CurrentRegion;
	EState m_State;
	MonitorRegion::EType m_RegionType;
};

#endif // MONITORGRAPHICSALERTREGIONGROUP_H
