#include "Diagnostic.h"
#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

#include <fstream>
#include <iostream>

namespace config
{

	Diagnostic::Diagnostic()
		: m_DebugHistory(1000)
		, m_LogToFile(false)
		, m_LogMemoryUsage(false)
		, m_LogMonitorQueue(false)
		, m_LogProcessTime(false)
		, m_ShowMonitorDiagnostics(false)
	{

	}

	Diagnostic& Diagnostic::getInstance()
	{
		static Diagnostic instance;
		return instance;
	}

	bool Diagnostic::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "diagnostic.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Diagnostic::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Diagnostic::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "diagnostic.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Diagnostic::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	template<class Archive>
	void Diagnostic::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_DebugHistory;
		ar & m_LogToFile;
		ar & m_LogMemoryUsage;
		ar & m_LogMonitorQueue;
		ar & m_LogProcessTime;
		ar & m_ShowMonitorDiagnostics;
	}
}

