#include "Monitor.h"

#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

#include <fstream>
#include <iostream>

namespace config
{
	template<class Archive>
	void config::Monitor::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_AlertsLifetime;
		ar & m_MaxAlertsCount;
		ar & m_MinQueueSize;
		ar & m_MaxQueueSize;
		ar & m_TimeScale;
		ar & m_Aliases;
	}

	Monitor::Monitor()
		: m_AlertsLifetime(1)
		, m_MaxAlertsCount(100)
		, m_MinQueueSize(1)
		, m_MaxQueueSize(8)
		, m_TimeScale(1.1f)
	{

	}

	Monitor& Monitor::getInstance()
	{
		static Monitor instance;
		return instance;
	}

	bool Monitor::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "monitor.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Monitor::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Monitor::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "monitor.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Monitor::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	const std::string& Monitor::getAlias(const std::string& baseName) const
	{
		auto it = m_Aliases.find(baseName);
		if (it == m_Aliases.end())
			return baseName;

		return it->second;
	}
}