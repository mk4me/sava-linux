#include "Action.h"
#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>

#include <fstream>

namespace config
{

	Action::Action() 
		: m_DescriptorType(GBH_MBH)
		, m_MinVideoFrames(20)
		, m_TrainSetLength(70)
		, m_TestSetLength(30)
		, m_MaxVideoWidth(250)
		, m_MaxVideoHeight(250)
	{

	}

	const std::map<Action::DescriptorType, Action::Descriptor> Action::c_Descriptors
	{
		{ GBH, Descriptor(24576, "gbh", "gbh/split1Rs0/") },
		{ MBH, Descriptor(49152, "mbh", "mbh/split1Rs0/") },
		{ GBH_MBH, Descriptor(73728, "gbh mbh", "mergedData/") },
	};

	Action& Action::getInstance()
	{
		static Action instance;
		return instance;
	}

	bool Action::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "action.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Action::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Action::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "action.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			//std::cerr << "config::Action::load() exception: " << e.what() << std::endl;
			std::cerr << "config::Action::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	std::string Action::getDescriptorPath() const
	{
		auto it = c_Descriptors.find(m_DescriptorType);
		if (it == c_Descriptors.end())
			return std::string();

		return utils::Filesystem::getDataPath() + "action/" + it->second.path;
	}


	std::string Action::getDescriptorName() const
	{
		auto it = c_Descriptors.find(m_DescriptorType);
		if (it == c_Descriptors.end())
			return std::string();

		return it->second.name;
	}

	int Action::getDescriptorSize() const
	{
		auto it = c_Descriptors.find(m_DescriptorType);
		if (it == c_Descriptors.end())
			return 0;

		return it->second.size;
	}

	const Action::Descriptor& Action::getDescriptor(DescriptorType type) const
	{
		auto it = c_Descriptors.find(type);
		return it->second;
	}

	template<class Archive>
	void Action::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_DescriptorType;
		ar & m_MinVideoFrames;
		ar & m_TrainSetLength;
		ar & m_TestSetLength;
		ar & m_MaxVideoWidth;
		ar & m_MaxVideoHeight;
	}

}
