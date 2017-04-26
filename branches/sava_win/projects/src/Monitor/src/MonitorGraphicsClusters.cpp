#include "MonitorGraphicsClusters.h"
#include "MonitorRegionsManager.h"

#include "MonitorGraphicsClusterItem_Standard.h"
#include "MonitorGraphicsClusterItem_Fill_2d.h"
#include "MonitorGraphicsClusterItem_Floor_3d.h"

#include "config/Monitor.h"


MonitorGraphicsClusters::MonitorGraphicsClusters(QGraphicsItem *parent)
	: QGraphicsItem(parent)
{
	m_CachedVideoManger = MonitorVideoManager::getPointer();
}


void MonitorGraphicsClusters::onVideoLoaded()
{
	auto& pairs = m_CachedVideoManger->getClusterActionPairs();
	config::Monitor::DecorationType decorationType = (config::Monitor::DecorationType)config::Monitor::getInstance().getDecorationType();

	for (auto item : m_Items)
		item->setModified(false);

	for (auto pair : pairs)
	{
		MonitorGraphicsClusterItem* item = nullptr;

		int id = pair.first->getClusterId();
		auto it = std::find_if(m_Items.begin(), m_Items.end(), [id](const MonitorGraphicsClusterItem* item){ return id == item->getId(); });

		if (it != m_Items.end())
		{
			item = *it;
		}
		else
		{
			item = createDecoratorByType(decorationType);
			m_Items.push_back(item);
		}

		item->init(pair);
		item->setModified(true);
	}

	for (auto it = m_Items.begin(); it != m_Items.end();)
	{
		if (!(*it)->isModified())
		{
			delete *it;
			it = m_Items.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void MonitorGraphicsClusters::update(size_t _frame)
{
	for (auto item: m_Items)
		item->update(_frame);
}


QRectF MonitorGraphicsClusters::boundingRect() const
{
	if (scene())
		return scene()->sceneRect();
	return QRectF();
}

MonitorGraphicsClusterItem* MonitorGraphicsClusters::createDecoratorByType(config::Monitor::DecorationType type)
{
	using namespace config;

	switch (type)
	{
		case Monitor::DecorationType::STANDARD: return new MonitorGraphicsClusterItem_Standard(this);
		case Monitor::DecorationType::FILL_2D: return new MonitorGraphicsClusterItem_Fill_2d(this);
		case Monitor::DecorationType::FLOOR_3D: return new MonitorGraphicsClusterItem_Floor_3d(this);
	}

	assert(false && "Wrong decoration type");
	return nullptr;
}
