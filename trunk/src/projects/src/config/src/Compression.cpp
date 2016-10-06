#include "Compression.h"
#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>

namespace config
{
	Compression::Compression()
		: m_FilesInPackage(5)
		, m_ImageCompression(30)
		, m_BackgroundHistory(300)
		, m_DifferenceThreshold(20)
		, m_NewBackgroundMinPixels(0.2f)
		, m_MinCrumbleArea(100)
		, m_MergeCrumblesIterations(3)
	{
	}

	Compression::~Compression()
	{

	}

	bool Compression::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "compression.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Compression::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Compression::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "compression.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Compression::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

}