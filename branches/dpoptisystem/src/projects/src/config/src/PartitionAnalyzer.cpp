#include "PartitionAnalyzer.h"
#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>

#include <fstream>
#include <iostream>

template<class Archive>
void config::PartitionAnalyzer::serialize(Archive & ar, const unsigned int version)
{
	ar & m_PointMaxDistance;
	ar & m_PathMinMovement;
	ar & m_PathSpeedDiff;
	ar & m_PathAngleDiff;
	ar & m_ClusterMinPoints;
	ar & m_ClusterMarign;
	ar & m_SequenceMaxDistance;
	ar & m_SequenceSpeedDiff;
	ar & m_SequenceAngleDiff;
	ar & m_SequenceMinMovement;
	ar & m_SequenceCoverLevel;
	ar & m_SequenceContainLevel;
	ar & m_SequenceMinTime;
	ar & m_SequenceMinSize;
	ar & m_SequenceMaxSize;
}

config::PartitionAnalyzer::PartitionAnalyzer()
	: m_PointMaxDistance(40)
	, m_PathMinMovement(15)
	, m_PathSpeedDiff(0.4f)
	, m_PathAngleDiff(20)
	, m_ClusterMinPoints(10)
	, m_ClusterMarign(5)
	, m_SequenceMaxDistance(40)
	, m_SequenceSpeedDiff(0.6f)
	, m_SequenceAngleDiff(20)
	, m_SequenceMinMovement(50)
	, m_SequenceCoverLevel(50)
	, m_SequenceContainLevel(90)
	, m_SequenceMinTime(10)
	, m_SequenceMinSize(30)
	, m_SequenceMaxSize(350)
{

}

config::PartitionAnalyzer& config::PartitionAnalyzer::getInstance()
{
	static PartitionAnalyzer instance;
	return instance;
}

bool config::PartitionAnalyzer::load()
{
	try
	{
		std::ifstream ifs(utils::Filesystem::getConfigPath() + "partition_analyzer.cfg", std::ios::binary | std::ios::in);
		boost::archive::binary_iarchive ia(ifs);
		ia >> *this;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "config::PartitionAnalyzer::load() exception: " << e.what() << std::endl;
		return false;
	}
}

bool config::PartitionAnalyzer::save() const
{
	try
	{
		std::ofstream ofs(utils::Filesystem::getConfigPath() + "partition_analyzer.cfg", std::ios::binary | std::ios::out);
		boost::archive::binary_oarchive oa(ofs);
		oa << *this;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "config::PartitionAnalyzer::save() exception: " << e.what() << std::endl;
		return false;
	}
}

