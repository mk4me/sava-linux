#include "PathDetection.h"
#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <iostream>

namespace config
{

	PathDetection& PathDetection::getInstance()
	{
		static PathDetection instance;
		return instance;
	}

	PathDetection::PathDetection()
		// general
		: m_MaxCostThresh(250)
		, m_MaxPathLength(20)
		, m_MaxMissedFramesInPath(5)
		, m_CurrMaxDistFromPredictedNextWorldPoint(29.5f)

		, m_DescCostModifier(50)
		, m_DistanceModifier(2)
		, m_AngleModifier(3)

		, m_MaxProcessTime(0)

		, m_DetectorId(0)

		, m_SIFTnfeatures(0)
		, m_SIFTnOctaveLayers(3)
		, m_SIFTContrastThreshold(0.1)
		, m_SIFTEdgeThreshold(40.0)
		, m_SIFTSigma(1.6)

		, m_SURFHessianThreshold(400)
		, m_SURFnOctaves(3)
		, m_SURFnOctaveLayers(4)

		, m_Algorithm(ALGORITHM_OPTFLOW)
		, m_MaxPathsCount(0)

		, m_PredictedRoiDetector(true)
		, m_PredictedRoiEnabled(true)
		, m_PredictedRoiRadius(5)
		, m_RoiFilter(ROI_FILTER_GRADIENT_MAGNITUDE)
		, m_RoiFilterWindow(5)
		, m_RoiFilterThreshold(127)
		, m_RoiFilterSobel(true)
		, m_Distance0MinPathLenght(10)
		, m_Distance0MinPathDistance(3)
		, m_Distance1MinPathLenght(30)
		, m_Distance1MinPathDistance(6)
		, m_PathRemoveFilterT1(5)
		, m_PathRemoveFilterT2(6)
		, m_PathRemoveFilterN(5)
	{

	}

	PathDetection::~PathDetection()
	{

	}

	bool PathDetection::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "path_detection.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::PathDetection::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool PathDetection::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "path_detection.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::PathDetection::load() exception: " << e.what() << std::endl;
			return false;
		}
	}
}