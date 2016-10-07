#include "Process.h"
#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

#include <fstream>

namespace config
{

	Process& Process::getInstance()
	{
		static Process instance;
		return instance;
	}
	

	bool Process::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "process.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Process::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Process::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "process.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Process::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	template<class Archive>
	void Process::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_IdleTimeout;
		ar & m_AquisitionNode;
		ar & m_CompressionNodes;
		ar & m_PathDetectionNodes;
		ar & m_PathAnalysisNode;
		ar & m_GbhNodes;
		ar & m_MbhNodes;
		ar & m_SvmNodes;
	}
}
