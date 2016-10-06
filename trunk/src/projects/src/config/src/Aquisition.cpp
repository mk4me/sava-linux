#include "Aquisition.h"
#include "utils/Filesystem.h"
#include <utils/CAxisRawReader.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>

namespace config
{

	Aquisition::Aquisition()
		: m_SeqLength(25)
		, m_Fps(utils::camera::CAxisRawReader::DEFAULT_FPS)
		, m_Compression(utils::camera::CAxisRawReader::DEFAULT_COMPRESSION)
	{
	}

	bool Aquisition::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "aquisition.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Aquisition::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Aquisition::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "aquisition.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Aquisition::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

}