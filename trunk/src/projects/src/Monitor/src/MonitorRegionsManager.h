#pragma once

#include <QtGui/QPolygonF>
#include <QtCore/QRectF>
#include <QtCore/QDebug>

#include "MonitorConfig.h"
#include "MonitorVideoManager.h"
#include "MonitorGraphicsRegion.h"
#include "MonitorRegion.h"
#include "utils/Singleton.h"
#include "sequence/Cluster.h"
#include "sequence/Action.h"

#include <vector>
#include <map>


/// <summary>
/// Klasa zarz¹dzaj¹ca wszystkimi regionami Monitora.
/// </summary>
/// <seealso cref="Singleton{MonitorRegionsManager}" />
/// <seealso cref="IConfigListener" />
class MonitorRegionsManager : public Singleton<MonitorRegionsManager>, public IConfigListener
{

public:
	MonitorRegionsManager();
	virtual ~MonitorRegionsManager(){ };

	void onVideoLoaded();
	void onVideoPreload();
	void update(size_t _frame);

	MonitorRegionPtr add(MonitorRegion::EType type);
	void add(const MonitorRegionPtr& region);
	void remove(const MonitorRegionPtr& region);

	void addListener(IRegionListener* i_Listener);
	void removeListener(IRegionListener* i_Listener);

	void setCollisionEnabled(bool enabled);
	bool isCollisionEnabled() const { return m_CollisionEnabled; }

	bool isCollidedWith(MonitorRegion::EType type, const QRectF& rect);
	bool isCollidedWith(MonitorRegion::EType type, sequence::Cluster* cluster, size_t frame);

	std::vector<MonitorRegionPtr> getRegions(MonitorRegion::EType type) const;

protected:	
	virtual void onLoad(MonitorConfig* config) override;
	virtual void onSave(MonitorConfig* config) override;

private:
	void notifyEnter(const MonitorRegionPtr& i_Region);
	void notifyLeave(const MonitorRegionPtr& i_Region);

private:
	MonitorVideoManager* m_CachedVideoManger;

	std::map<MonitorRegion::EType, std::vector<MonitorRegionPtr>> m_RegionsMap;
	std::vector<IRegionListener*> m_Listeners;

	bool m_CollisionEnabled;
};


