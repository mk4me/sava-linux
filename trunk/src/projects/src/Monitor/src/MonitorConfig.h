#pragma once

#include "utils/Singleton.h"
#include <utils/Filesystem.h>
#include "MonitorRegion.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <map>
#include <set>
#include <vector>

#define MONITOR_CONFIG_DIR_NAME "monitor"
#define MONITOR_CONFIG_FILE_NAME "MonitorConfig.cfg"

class MonitorConfig;

class IConfigListener
{
public:
	virtual void onLoad(MonitorConfig* config) = 0;
	virtual void onSave(MonitorConfig* config) = 0;
};

class MonitorConfig : public Singleton<MonitorConfig>
{

public:
	MonitorConfig();
	~MonitorConfig();

	bool load(const std::string& filename);
	bool save(const std::string& filename);

	bool load();
	bool save();

	void addListener(IConfigListener* i_Listener);
	void removeListener(IConfigListener* i_Listener);

	bool isMilestoneEnabled() const { return m_MilestoneEnabled; }
	void setMilestoneEnabled(bool enabled) { m_MilestoneEnabled = enabled; }

	bool isClustersVisibled() const { return m_ClustersVisibled; }
	void setClustersVisibled(bool visibled) { m_ClustersVisibled = visibled; }

	void setAlertsIds(const std::set<size_t>& ids) { m_AlertsIds = ids; }
	std::set<size_t> getAlertsIds() const { return m_AlertsIds; }

	void setRegions(const std::map<MonitorRegion::EType, std::vector<MonitorRegionPtr>>& regions) { m_Regions = regions; }
	std::map<MonitorRegion::EType, std::vector<MonitorRegionPtr>> getRegions() const { return m_Regions; }

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Regions;
		ar & m_MilestoneEnabled;
		ar & m_ClustersVisibled;
		ar & m_AlertsIds;
	};

	void notifySave();
	void notifyLoad();

	std::string getConfigFilePath() const;

private:
	bool m_MilestoneEnabled;
	bool m_ClustersVisibled;
	std::set<size_t> m_AlertsIds;
	std::map<MonitorRegion::EType, std::vector<MonitorRegionPtr>> m_Regions;

	std::vector<IConfigListener*> m_Listeners;

};

