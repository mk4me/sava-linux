#pragma once
#ifndef Config_PathDetection_h__
#define Config_PathDetection_h__

#include <boost/serialization/access.hpp>

namespace config
{
	class PathDetection
	{
	public:
		static PathDetection& getInstance();

		bool load();
		bool save() const;

		enum Algorithm
		{
			ALGORITHM_OPTFLOW,
			ALGORITHM_DEFAULT,
		};

		Algorithm getAlgorithm() const { return m_Algorithm; }
		void setAlgorithm(Algorithm val) { m_Algorithm = val; }

		enum RoiFilter
		{
			ROI_FILTER_NULL,
			ROI_FILTER_VARIANCE,
			ROI_FILTER_GRADIENT_MAGNITUDE,
		};

		size_t getMaxPathsCount() const { return m_MaxPathsCount; }
		void setMaxPathsCount(size_t val) { m_MaxPathsCount = val; }

		// general
		int getMaxCostThresh() const { return m_MaxCostThresh; }
		void setMaxCostThresh(int val) { m_MaxCostThresh = val; }
		int getMaxPathLength() const { return m_MaxPathLength; }
		void setMaxPathLength(int val) { m_MaxPathLength = val; }
		int getMaxMissedFramesInPath() const { return m_MaxMissedFramesInPath; }
		void setMaxMissedFramesInPath(int val) { m_MaxMissedFramesInPath = val; }
		float getCurrMaxDistFromPredictedNextWorldPoint() const { return m_CurrMaxDistFromPredictedNextWorldPoint; }
		void setCurrMaxDistFromPredictedNextWorldPoint(float val) { m_CurrMaxDistFromPredictedNextWorldPoint = val; }

		int getDescCostModifier() const { return m_DescCostModifier; }
		void setDescCostModifier(int val) { m_DescCostModifier = val; }
		int getDistanceModifier() const { return m_DistanceModifier; }
		void setDistanceModifier(int val) { m_DistanceModifier = val; }
		int getAngleModifier() const { return m_AngleModifier; }
		void setAngleModifier(int val) { m_AngleModifier = val; }

		// detector
		int getDetectorId() const { return m_DetectorId; }
		void setDetectorId(int val) { m_DetectorId = val; }

		int getSIFTnfeatures() const { return m_SIFTnfeatures; }
		void setSIFTnfeatures(int val) { m_SIFTnfeatures = val; }
		int getSIFTnOctaveLayers() const { return m_SIFTnOctaveLayers; }
		void setSIFTnOctaveLayers(int val) { m_SIFTnOctaveLayers = val; }
		double getSIFTContrastThreshold() const { return m_SIFTContrastThreshold; }
		void setSIFTContrastThreshold(double val) { m_SIFTContrastThreshold = val; }
		double getSIFTEdgeThreshold() const { return m_SIFTEdgeThreshold; }
		void setSIFTEdgeThreshold(double val) { m_SIFTEdgeThreshold = val; }
		double getSIFTSigma() const { return m_SIFTSigma; }
		void setSIFTSigma(double val) { m_SIFTSigma = val; }

		double getSURFHessianThreshold() const { return m_SURFHessianThreshold; }
		void setSURFHessianThreshold(double val) { m_SURFHessianThreshold = val; }
		int getSURFnOctaves() const { return m_SURFnOctaves; }
		void setSURFnOctaves(int val) { m_SURFnOctaves = val; }
		int getSURFnOctaveLayers() const { return m_SURFnOctaveLayers; }
		void setSURFnOctaveLayers(int val) { m_SURFnOctaveLayers = val; }

		int getMaxProcessTime() const { return m_MaxProcessTime; }
		void setMaxProcessTime(int val) { m_MaxProcessTime = val; }

		bool isPredictedRoiDetector() const { return m_PredictedRoiDetector; }
		void setPredictedRoiDetector(bool val) { m_PredictedRoiDetector = val; }
		bool isPredictedRoiEnabled() const { return m_PredictedRoiEnabled; }
		void setPredictedRoiEnabled(bool val) { m_PredictedRoiEnabled = val; }
		int getPredictedRoiRadius() const { return m_PredictedRoiRadius; }
		void setPredictedRoiRadius(int val) { m_PredictedRoiRadius = val; }
		int getRoiFilter() const { return m_RoiFilter; }
		void setRoiFilter(int val) { m_RoiFilter = val; }
		int getRoiFilterWindow() const { return m_RoiFilterWindow; }
		void setRoiFilterWindow(int val) { m_RoiFilterWindow = val; }
		int getRoiFilterThreshold() const { return m_RoiFilterThreshold; }
		void setRoiFilterThreshold(int val) { m_RoiFilterThreshold = val; }
		bool isRoiFilterSobel() const { return m_RoiFilterSobel; }
		void setRoiFilterSobel(bool val) { m_RoiFilterSobel = val; }
		int getDistance0MinPathLenght() const { return m_Distance0MinPathLenght; }
		void setDistance0MinPathLenght(int val) { m_Distance0MinPathLenght = val; }
		int getDistance0MinPathDistance() const { return m_Distance0MinPathDistance; }
		void setDistance0MinPathDistance(int val) { m_Distance0MinPathDistance = val; }
		int getDistance1MinPathLenght() const { return m_Distance1MinPathLenght; }
		void setDistance1MinPathLenght(int val) { m_Distance1MinPathLenght = val; }
		int getDistance1MinPathDistance() const { return m_Distance1MinPathDistance; }
		void setDistance1MinPathDistance(int val) { m_Distance1MinPathDistance = val; }
		int getPathRemoveFilterT1() const { return m_PathRemoveFilterT1; }
		void setPathRemoveFilterT1(int val) { m_PathRemoveFilterT1 = val; }
		int getPathRemoveFilterT2() const { return m_PathRemoveFilterT2; }
		void setPathRemoveFilterT2(int val) { m_PathRemoveFilterT2 = val; }
		int getPathRemoveFilterN() const { return m_PathRemoveFilterN; }
		void setPathRemoveFilterN(int val) { m_PathRemoveFilterN = val; }

	private:
		PathDetection();
		~PathDetection();

		Algorithm m_Algorithm;
		size_t m_MaxPathsCount;

		// general
		int m_MaxCostThresh;
		int m_MaxPathLength;
		int m_MaxMissedFramesInPath;
		float m_CurrMaxDistFromPredictedNextWorldPoint;

		int m_DescCostModifier;
		int m_DistanceModifier;
		int m_AngleModifier;

		int m_MaxProcessTime;

		// detector 0 - SIFT, 1 - SURF
		int m_DetectorId;

		int m_SIFTnfeatures;
		int m_SIFTnOctaveLayers;
		double m_SIFTContrastThreshold;
		double m_SIFTEdgeThreshold;
		double m_SIFTSigma;

		double m_SURFHessianThreshold;
		int m_SURFnOctaves;
		int m_SURFnOctaveLayers;

		bool m_PredictedRoiDetector;
		bool m_PredictedRoiEnabled;
		int m_PredictedRoiRadius;
		int m_RoiFilter;
		int m_RoiFilterWindow;
		int m_RoiFilterThreshold;
		bool m_RoiFilterSobel;
		int m_Distance0MinPathLenght;
		int m_Distance0MinPathDistance;
		int m_Distance1MinPathLenght;
		int m_Distance1MinPathDistance;
		int m_PathRemoveFilterT1;
		int m_PathRemoveFilterT2;
		int m_PathRemoveFilterN;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void PathDetection::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_MaxCostThresh;
		ar & m_MaxPathLength;
		ar & m_MaxMissedFramesInPath;
		ar & m_CurrMaxDistFromPredictedNextWorldPoint;
		ar & m_DescCostModifier;
		ar & m_DistanceModifier;
		ar & m_AngleModifier;

		ar & m_DetectorId;

		ar & m_SIFTnfeatures;
		ar & m_SIFTnOctaveLayers;
		ar & m_SIFTContrastThreshold;
		ar & m_SIFTEdgeThreshold;
		ar & m_SIFTSigma;

		ar & m_SURFHessianThreshold;
		ar & m_SURFnOctaves;
		ar & m_SURFnOctaveLayers;

		ar & m_MaxProcessTime;

		ar & m_Algorithm;
		ar & m_MaxPathsCount;
	}
}

#endif // Config_PathDetection_h__