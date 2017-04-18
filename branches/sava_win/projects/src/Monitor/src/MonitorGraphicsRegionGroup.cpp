#include "MonitorGraphicsRegionGroup.h"
#include "sequence/Cluster.h"
#include "MonitorRegionsManager.h"

MonitorGraphicsRegionGroup::MonitorGraphicsRegionGroup(MonitorRegion::EType regionType, QGraphicsItem *parent)
	: QGraphicsItem(parent)
	, m_RegionType(regionType)
	, m_State(NONE)
	, m_CurrentRegion(nullptr)
{
	m_CachedVideoManger = MonitorVideoManager::getPointer();
}

void MonitorGraphicsRegionGroup::init()
{
	auto regions = MonitorRegionsManager::getInstance().getRegions(m_RegionType);
	for (auto& region : regions)
	{
		MonitorGraphicsRegion* graphRegion = add(region->getPolygon(), region);
		graphRegion->setCloseState(MonitorGraphicsRegion::CLOSE);
	}

}

void MonitorGraphicsRegionGroup::setState(EState _state)
{
	if (m_State != _state) {
		m_State = _state;

		switch (m_State) {

			case NONE:
				if (m_CurrentRegion)
				{
					if (m_CurrentRegion->getCloseState() != MonitorGraphicsRegion::CLOSE)
					{
						remove(m_CurrentRegion);
					}
					else
					{
						m_CurrentRegion->setState(MonitorGraphicsRegion::NONE);
					}

					m_CurrentRegion = nullptr;
				}
			
				break;

			case CREATING_NEW_REGION:
				m_CurrentRegion = add(RegionPoints() << QPointF(-100, -100));
				m_CurrentRegion->setState(MonitorGraphicsRegion::CREATING);
				break;
		}
	}
}


void MonitorGraphicsRegionGroup::deleteLastRegion()
{
	if (m_AlertRegions.size() != 0)
		remove(m_AlertRegions.back());
}


void MonitorGraphicsRegionGroup::onMouseTracking(QGraphicsSceneMouseEvent *event)
{
	if (m_State != NONE && m_CurrentRegion)
		m_CurrentRegion->onMouseTracking(event);
}


void MonitorGraphicsRegionGroup::onMousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_State != NONE && m_CurrentRegion)
		m_CurrentRegion->onMousePressEvent(event);
}


void MonitorGraphicsRegionGroup::onKeyPressEvent(QKeyEvent *event)
{
	if (m_State != NONE && event->key() == Qt::Key_Delete)
	{
		if (m_CurrentRegion && m_CurrentRegion->getPointCount() > 1)
		{
			while (m_CurrentRegion->getPointCount() != 1)
				m_CurrentRegion->removePoint((size_t)0);
		}
		else if (m_AlertRegions.size() > 1)
		{
			remove(*(m_AlertRegions.end() - 2));
		}

		scene()->update();
	}
}



MonitorGraphicsRegion* MonitorGraphicsRegionGroup::add(const RegionPoints& i_RegionPoints, MonitorRegionPtr regionSrc)
{
	MonitorGraphicsRegion* graphRegion = create(m_RegionType, regionSrc);
	graphRegion->setPoints(i_RegionPoints);
	m_AlertRegions.push_back(graphRegion);

	return graphRegion;
}


bool MonitorGraphicsRegionGroup::remove(MonitorGraphicsRegion* i_AlertRegion)
{
	int index = m_AlertRegions.indexOf(i_AlertRegion);
	if (index == -1)
		return false;

	MonitorGraphicsRegion* graphRegion = m_AlertRegions.at(index);
	m_AlertRegions.remove(index);

	MonitorRegionPtr reg = graphRegion->getSource();
	MonitorRegionsManager::getInstance().remove(reg);

	delete graphRegion;

	return true;
}


MonitorGraphicsRegion* MonitorGraphicsRegionGroup::create(MonitorRegion::EType type, MonitorRegionPtr regionSrc)
{
	if (regionSrc == nullptr)
		regionSrc = MonitorRegionsManager::getInstance().add(type);

	switch (type)
	{
		case MonitorRegion::ALERT: return new MonitorGraphicsRegionAlert(regionSrc, this);
		case MonitorRegion::MASK: return new MonitorGraphicsRegionMask(regionSrc, this);
	}

	return nullptr;
}

void MonitorGraphicsRegionGroup::onChanged(MonitorGraphicsRegion* i_ChangedRegion)
{
	if (m_CurrentRegion == i_ChangedRegion) {
		if (m_CurrentRegion->getCloseState() == MonitorGraphicsRegion::CLOSE)
		{
			setState(NONE);
			setState(CREATING_NEW_REGION);
		}
	}
}


QRectF MonitorGraphicsRegionGroup::boundingRect() const
{
	if (scene())
		return scene()->sceneRect();
	return QRectF();
}


