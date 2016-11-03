#pragma once

#include "utils/Singleton.h"
#include "config/Glossary.h"
#include "MonitorVideoManager.h"
#include "MonitorConfig.h"

#include <set>
#include <vector>


class IActionListener {
public:
	virtual void onActionStart(const MonitorVideoManager::ActionPair& i_ActionPair) = 0;
	virtual void onActionEnd(const MonitorVideoManager::ActionPair& i_ActionPair) = 0;
};


class MonitorActionManager: public Singleton<MonitorActionManager>, public IConfigListener
{

public:
	MonitorActionManager();

	std::set<size_t> getAlertsIds() const { return m_AlertIds; }
	void setAlertsIds(const std::set<size_t>& ids) { m_AlertIds = ids; }

	bool isAlert(int i_ActionId) const;
	void addAlert(int i_ActionId);
	void removeAlert(int i_ActionId);

	void addListener(IActionListener* i_Listener);
	void removeListener(IActionListener* i_Listener);

	void onVideoPreload();
	void onVideoLoaded();
	void update(size_t i_Frame);

protected:
	virtual void onLoad(MonitorConfig* config) override;
	virtual void onSave(MonitorConfig* config) override;

private:
	void notifyStartAlert(const MonitorVideoManager::ActionPair& i_ActionPair);
	void notifyEndAlert(const MonitorVideoManager::ActionPair& i_ActionPair);

private:
	MonitorVideoManager* m_CachedVideoManger;

	std::vector<IActionListener*> m_Listeners;
	std::set<size_t> m_AlertIds;

	MonitorVideoManager::ActionPairVec m_ActionsToShow;
	MonitorVideoManager::ActionPairVec m_ActionsInProgress;
};

