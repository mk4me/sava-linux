#include "Directory.h"

#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>

#include <fstream>
#include <iostream>

BOOST_CLASS_VERSION(config::Directory, 1)

namespace config
{
	template<class Archive>
	void config::Directory::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_VideosPath;
		ar & m_TemporaryPath;
		ar & m_AlertsPath;

		if (version > 0)
			ar & m_BackupPath;
	}

	Directory& Directory::getInstance()
	{
		static Directory instance;
		return instance;
	}
	
	bool Directory::load()
	{
		try
		{
			std::string path = utils::Filesystem::getConfigPath() + "directory.cfg";
			std::ifstream ifs(path, std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Directory::load() exception: " << e.what() << std::endl;
			return false;
		}
	}
	
	bool Directory::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "directory.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Directory::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	void Directory::setVideosPath(const std::string& path)
	{
		m_VideosPath = utils::Filesystem::unifyPath(path);
	}

	void Directory::setTemporaryPath(const std::string& path)
	{
		m_TemporaryPath = utils::Filesystem::unifyPath(path);
	}

	void Directory::setAlertsPath(const std::string& path)
	{
		m_AlertsPath = utils::Filesystem::unifyPath(path);
	}


	void Directory::setBackupPath(const std::string& path)
	{
		m_BackupPath = utils::Filesystem::unifyPath(path);
	}

}
