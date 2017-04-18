#include "Milestone.h"

#include "utils/Filesystem.h"

#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <iostream>

namespace config
{
	template<class Archive>
	void config::Milestone::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_User;
		ar & m_Password;
		ar & m_Ip;
	}

	Milestone& Milestone::getInstance()
	{
		static Milestone instance;
		return instance;
	}

	bool Milestone::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "milestone.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Milestone::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Milestone::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "milestone.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Milestone::save() exception: " << e.what() << std::endl;
			return false;
		}
	}
}
