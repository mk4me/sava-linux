#include "MonitorActionManager.h"
#include <QtCore/QString>

#include "config/Glossary.h"
#include "sequence/Cluster.h"
#include "sequence/Action.h"
#include "MonitorRegionsManager.h"

MonitorActionManager::MonitorActionManager(){	
	m_CachedVideoManger = MonitorVideoManager::getPointer();
	config::Glossary::getInstance().load();

	MonitorConfig::getInstance().addListener(this);
}


bool MonitorActionManager::isAlert(int i_ActionId) const{
	return (m_AlertIds.find(i_ActionId) != m_AlertIds.end());
}


void MonitorActionManager::addAlert(int i_ActionId){
	m_AlertIds.insert(i_ActionId);
}


void MonitorActionManager::removeAlert(int i_ActionId){
	auto it = m_AlertIds.find(i_ActionId);
	if (it != m_AlertIds.end())
		m_AlertIds.erase(it);
}

void MonitorActionManager::addListener(IActionListener* i_Listener){
	auto foundIt = std::find(m_Listeners.begin(), m_Listeners.end(), i_Listener);
	if (foundIt == m_Listeners.end())
		m_Listeners.push_back(i_Listener);
}

void MonitorActionManager::removeListener(IActionListener* i_Listener)
{
	auto foundIt = std::find(m_Listeners.begin(), m_Listeners.end(), i_Listener);
	if (foundIt != m_Listeners.end())
		m_Listeners.erase(foundIt);
}

void MonitorActionManager::onVideoPreload()
{
	//add actions if stay from last sequence
	for (MonitorVideoManager::ActionPair _pair : m_ActionsToShow){
		notifyStartAlert(_pair);
		notifyEndAlert(_pair);
	}

	for (MonitorVideoManager::ActionPair _pair : m_ActionsInProgress){
		notifyEndAlert(_pair);
	}
}

void MonitorActionManager::onVideoLoaded()
{
	m_ActionsToShow.clear();
	m_ActionsInProgress.clear();

	//filter
	MonitorVideoManager::ActionPairVec pairsVec = m_CachedVideoManger->getClusterActionPairs();
	for (MonitorVideoManager::ActionPair& _pair : pairsVec)
	{
		auto cluster = _pair.first;
		auto action = _pair.second;

		//action filter
		if (
			cluster != nullptr 
			&& action != nullptr
			&& !cluster->getFramesPositions().empty()
			&& action->getActionId() >= 0
			&& isAlert(action->getActionId())
			)
		{
			m_ActionsToShow.push_back(_pair);
		}	
	}
}

void MonitorActionManager::update(size_t i_Frame)
{
	//check if action should be shown as begin one
	auto showIt = m_ActionsToShow.begin();
	while (showIt != m_ActionsToShow.end())
	{
		if (i_Frame >= showIt->first->getStartFrame())
		{
			if (!MonitorRegionsManager::getInstance().isCollidedWith(MonitorRegion::MASK, showIt->first.get(), i_Frame))
				notifyStartAlert(*showIt);

			m_ActionsInProgress.push_back(*showIt);
			showIt = m_ActionsToShow.erase(showIt);
		}
		else{
			++showIt;
		}
	}

	//check if action should be shown as end one
	auto progressIt = m_ActionsInProgress.begin();
	while (progressIt != m_ActionsInProgress.end())
	{
		if (i_Frame >= progressIt->first->getEndFrame()) {
			notifyEndAlert(*progressIt);
			progressIt = m_ActionsInProgress.erase(progressIt);
		}
		else{
			++progressIt;
		}
	}
}

void MonitorActionManager::notifyStartAlert(const MonitorVideoManager::ActionPair& i_ActionPair)
{
	for (IActionListener* listener : m_Listeners)
		listener->onActionStart(i_ActionPair);
}


void MonitorActionManager::notifyEndAlert(const MonitorVideoManager::ActionPair& i_ActionPair)
{
	for (IActionListener* listener : m_Listeners)
		listener->onActionEnd(i_ActionPair);
}

void MonitorActionManager::onLoad(MonitorConfig* config)
{
	m_AlertIds = config->getAlertsIds();
}

void MonitorActionManager::onSave(MonitorConfig* config)
{
	config->setAlertsIds(m_AlertIds);
}
