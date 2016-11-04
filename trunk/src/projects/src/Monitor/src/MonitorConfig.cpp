#include "MonitorConfig.h"
#include "boost/filesystem/operations.hpp"


MonitorConfig::MonitorConfig()
	: m_MilestoneEnabled(false)
	, m_ClustersVisibled(false)
{
}


MonitorConfig::~MonitorConfig()
{
}

bool MonitorConfig::load(const std::string& filename)
{
	try
	{
		std::ifstream ifs(filename, std::ios::binary | std::ios::in);
		boost::archive::binary_iarchive ia(ifs);
		ia >> *this;

		notifyLoad();

		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "MonitorConfig::load() exception: " << e.what() << std::endl;
		return false;
	}
}


bool MonitorConfig::save(const std::string& filename)
{
	boost::filesystem::create_directories(utils::Filesystem::getUserPath() + MONITOR_CONFIG_DIR_NAME);

	try
	{
		notifySave();

		std::ofstream ofs(filename, std::ios::binary | std::ios::out);
		boost::archive::binary_oarchive oa(ofs);
		oa << *this;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "MonitorConfig::save() exception: " << e.what() << std::endl;
		return false;
	}
}


bool MonitorConfig::load(){
	return load(getConfigFilePath());
}


bool MonitorConfig::save(){
	return save(getConfigFilePath());
}

void MonitorConfig::addListener(IConfigListener* i_Listener){
	auto foundIt = std::find(m_Listeners.begin(), m_Listeners.end(), i_Listener);
	if (foundIt == m_Listeners.end())
		m_Listeners.push_back(i_Listener);
}

void MonitorConfig::removeListener(IConfigListener* i_Listener)
{
	auto foundIt = std::find(m_Listeners.begin(), m_Listeners.end(), i_Listener);
	if (foundIt != m_Listeners.end())
		m_Listeners.erase(foundIt);
}


void MonitorConfig::notifySave()
{
	for (IConfigListener* listener : m_Listeners)
		listener->onSave(this);
}

void MonitorConfig::notifyLoad()
{
	for (IConfigListener* listener : m_Listeners)
		listener->onLoad(this);
}

std::string MonitorConfig::getConfigFilePath() const
{
	return utils::Filesystem::getUserPath() + MONITOR_CONFIG_DIR_NAME + "/" + MONITOR_CONFIG_FILE_NAME;
}
