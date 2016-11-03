#include "Monitor.h"

#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>

#include <fstream>
#include <iostream>

namespace config
{
	template<class Archive>
	void config::Monitor::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_AlertsLifetime;
		ar & m_MaxAlertsCount;
	}

	Monitor::Monitor()
		: m_AlertsLifetime(1)
		, m_MaxAlertsCount(100)
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
}