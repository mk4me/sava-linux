#include "MonitorRegionsManager.h"



MonitorRegionsManager::MonitorRegionsManager()
	: m_CollisionEnabled(true)
{
	m_CachedVideoManger = MonitorVideoManager::getPointer();

	m_RegionsMap[MonitorRegion::EType::ALERT] = std::vector<MonitorRegionPtr>();
	m_RegionsMap[MonitorRegion::EType::MASK] = std::vector<MonitorRegionPtr>();

	MonitorConfig::getInstance().addListener(this);
}


void MonitorRegionsManager::onVideoLoaded(){
}

void MonitorRegionsManager::onVideoPreload(){
}

void MonitorRegionsManager::update(size_t _frame)
{
	if (!m_CollisionEnabled)
		return;

	//get all cluster-action pairs from this frame
	MonitorVideoManager::ActionPairVec _pairs = m_CachedVideoManger->getClusterActionPairs(_frame);

	//iterate by all regions types
	for (auto& mapIt : m_RegionsMap)
	{
		if (mapIt.first != MonitorRegion::ALERT)
			continue;

		//iterate by all regions
		for (auto& region : mapIt.second)
		{
			bool isCollided = false;

			//iterate by all clusters and check if collides with region
			for (auto pairIt = _pairs.begin(); pairIt != _pairs.end(); ++pairIt)
			{
				cv::Rect rect = pairIt->first->getFrameAt(_frame);
				QRectF qRect = QRectF(rect.x, rect.y, rect.width, rect.height);

				if (region->isCollidedWith(qRect))
				{
					isCollided = true;
					break;
				}
			}

			//set collision in region
			if (isCollided != region->isCollided())
			{
				region->setCollided(isCollided);
				if (isCollided)
					notifyEnter(region);
				else
					notifyLeave(region);
			}
		}
	}
}


void MonitorRegionsManager::addListener(IRegionListener* i_Listener){
	auto foundIt = std::find(m_Listeners.begin(), m_Listeners.end(), i_Listener);
	if (foundIt == m_Listeners.end())
		m_Listeners.push_back(i_Listener);
}


void MonitorRegionsManager::removeListener(IRegionListener* i_Listener)
{
	auto foundIt = std::find(m_Listeners.begin(), m_Listeners.end(), i_Listener);
	if (foundIt != m_Listeners.end())
		m_Listeners.erase(foundIt);
}


void MonitorRegionsManager::setCollisionEnabled(bool enabled)
{
	m_CollisionEnabled = enabled;
	if (!m_CollisionEnabled)
		for (auto& mapIt : m_RegionsMap)
			for (auto& region : mapIt.second)
			{
				region->setCollided(false);
				notifyLeave(region);
				
			}
}

bool MonitorRegionsManager::isCollidedWith(MonitorRegion::EType type, const QRectF& rect)
{
	auto regionMap = m_RegionsMap.find(type);
	if (regionMap == m_RegionsMap.end())
		return false;

	for (auto elem : regionMap->second)
		if (elem->isCollidedWith(rect))
			return true;

	return false;
}

bool MonitorRegionsManager::isCollidedWith(MonitorRegion::EType type, sequence::Cluster* cluster, size_t frame)
{
	cv::Rect rect = cluster->getFrameAt(frame);
	QRectF qRect(rect.x, rect.y, rect.width, rect.height);

	return isCollidedWith(type, qRect);
}


std::vector<MonitorRegionPtr> MonitorRegionsManager::getRegions(MonitorRegion::EType type) const
{
	auto it = m_RegionsMap.find(type);
	if (it != m_RegionsMap.end())
		return it->second;

	return std::vector<MonitorRegionPtr>();
}

void MonitorRegionsManager::add(const MonitorRegionPtr& region)
{
	auto mapIt = m_RegionsMap.find(region->getType());
	if (mapIt != m_RegionsMap.end())
	{
		auto& regions = mapIt->second;
		auto regIt = std::find(regions.begin(), regions.end(), region);
		if (regIt == regions.end())
			regions.push_back(region);
	}
}


MonitorRegionPtr MonitorRegionsManager::add(MonitorRegion::EType type)
{
	auto region = MonitorRegionPtr(new MonitorRegion(type));
	add(region);
	return region;
}

void MonitorRegionsManager::remove(const MonitorRegionPtr& region)
{
	auto mapIt = m_RegionsMap.find(region->getType());
	if (mapIt != m_RegionsMap.end())
	{
		auto& regions = mapIt->second;
		auto regIt = std::find(regions.begin(), regions.end(), region);
		if (regIt != regions.end())
			regions.erase(regIt);
	}
}

void MonitorRegionsManager::notifyEnter(const MonitorRegionPtr& i_Region)
{
	for (IRegionListener* listener : m_Listeners)
		listener->onRegionEnter(i_Region);
}

void MonitorRegionsManager::notifyLeave(const MonitorRegionPtr& i_Region)
{
	for (IRegionListener* listener : m_Listeners)
		listener->onRegionLeave(i_Region);
}

void MonitorRegionsManager::onLoad(MonitorConfig* config)
{
	auto regions = config->getRegions();
	for (auto& mapIt : regions)
		for (auto& region : mapIt.second)
			add(region);
}

void MonitorRegionsManager::onSave(MonitorConfig* config)
{
	config->setRegions(m_RegionsMap);
}
