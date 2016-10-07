#include "PathAnalysis.h"
#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>

namespace config
{
	PathAnalysis& PathAnalysis::getInstance()
	{
		static PathAnalysis instance;
		return instance;
	}

	PathAnalysis::PathAnalysis()
		: m_UseOpenMP(true)
		, m_Alpha(0.5f)
		, m_MinimumPathLength(10)
		, m_PathThresholds(100, 3, 100)
		, m_ClusterThresholds(100, 3, 100)
		, m_WeightX(20)
		, m_WeightY(40)
		, m_WeightTau(0.5f)
		, m_EnableMerging(1)

		, m_EnableMovingPaths(1)
		, m_PathsPercentageMovesPerFrame(0.05f)
		, m_MinPathsMovesPerFrame(10)
		, m_CreateNewCluster(true)
		, m_TryMovingOnlyWhenCurrentIsTooFar(true)
		, m_EnableClusterRemoving(1)

		, m_EnableMergingGaps(1)
		, m_MergingThresholds(25, 50, 50)
		, m_MergingOldestClusterTime(50)

		, m_MinProcessTime(0)
		, m_MaxClusterWidth(300)
		, m_MaxClusterHeight(500)

		, m_MinPathsInCluster(0)
	{

	}

	bool PathAnalysis::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "path_analysis.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::PathAnalysis::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool PathAnalysis::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "path_analysis.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::PathAnalysis::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

}