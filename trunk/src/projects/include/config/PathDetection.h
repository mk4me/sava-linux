#pragma once
#ifndef Config_PathDetection_h__
#define Config_PathDetection_h__

#include <boost/serialization/access.hpp>

namespace config
{
	class PathDetection
	{
	public:
		PathDetection();
		~PathDetection();

		bool load();
		bool save() const;

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

	private:
		// general
		int m_MaxCostThresh;
		int m_MaxPathLength;
		int m_MaxMissedFramesInPath;
		float m_CurrMaxDistFromPredictedNextWorldPoint;

		int m_DescCostModifier;
		int m_DistanceModifier;
		int m_AngleModifier;

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
	}
}

#endif // Config_PathDetection_h__