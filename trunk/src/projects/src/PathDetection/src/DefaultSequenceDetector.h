#pragma once
#ifndef Detector_h__
#define Detector_h__

#include "ISequenceDetector.h"
#include "DefaultPathDetector.h"

#include <utils/Hungarian.h>
#include <utils/ProgramOptions.h>
#include <utils/CvZoom.h>

#include <opencv2/core.hpp>

#include <mutex>

namespace cv
{
	class Feature2D;
	typedef Feature2D FeatureDetector;
	typedef Feature2D DescriptorExtractor;
}

namespace sequence
{
	class PathStream;
}

namespace config
{
	class PathDetection;
}

class path;

namespace clustering
{
	class DefaultSequenceDetector : public ISequenceDetector
	{
	public:
		DefaultSequenceDetector(bool visualize);
		~DefaultSequenceDetector();

		static void registerParameters(ProgramOptions& programOptions);
		bool loadParameters(const ProgramOptions& options);
		void process(const std::string& inputSequence, const std::string& outputPaths);

		void show();

	private:
		bool m_Visualize;
		std::mutex m_VisualizeMutex;
		cv::Mat m_VisualizationFrame;
		utils::ZoomObjectCollection m_ZoomObjects;

		/*
		struct ThreadTmpData
		{
			hungarian_problem_t m_HungarianProblem;
			float** m_CostMatrix;

			cv::Mat m_TmpDescs;
			std::vector<cv::KeyPoint> m_TmpKeypoints;
			std::vector<cv::Point2f> m_TmpWorldPoints;
			std::vector<path*> m_TmpPaths;

			

			int threadNum;
		};

		std::vector<ThreadTmpData> m_ThreadsTmpData;

		//cv::Ptr<cv::DescriptorExtractor> m_Descriptor;

		*/
		cv::Mat m_VideoFrame;
		/*
		//cv::Mat m_VideoFrameGray;
		//cv::Mat m_VideoFrameCompare;
		//cv::Mat m_FinalVideoFrameCompare;
		std::vector<path*> m_Paths;
		std::vector<cv::Rect> m_ROIRegions;

#pragma region Parameters
		int m_MaxCostThresh;
		int m_MaxPathLength;
		int m_MaxMissedFramesInPath;

		int m_DescCostModifier;
		int m_DistanceModifier;
		int m_AngleModifier;

		int m_InPlaceMode;
		int m_UseROIForDetection;
		cv::Rect m_UserDefinedROI;
		float m_CurrMaxDistFromPredictedNextWorldPoint;

		int m_MaxProcessTime;

		int m_DetectorID;

		// SIFT
		int SIFTnfreatures;
		int SIFTnOctaveLayers;
		double SIFTThreshold;
		double SIFTEdgeThreshold;
		double SIFTSigma;

		// SURF
		int SURFnOctaves;
		int SURFnOctaveLayers;
		int SURFThreshold;

		template<typename T>
		T getParameter(const ProgramOptions& options, const std::string& paramName, const T& defVal);
		
		static void registerGeneralParameters(ProgramOptions& programOptions);
		static void registerDetectorParameters(ProgramOptions& programOptions);
		bool loadGeneralParameters(const ProgramOptions& options, const config::PathDetection& config);
		bool loadDetectorParameters(const ProgramOptions& options, const config::PathDetection& config);
#pragma endregion Parameters

		void initPerROIStructures(ThreadTmpData& tmpData, const cv::Rect& roi);
		void createCostMatrix(ThreadTmpData& tmpData);
		void match(ThreadTmpData& tmpData);
		void appendPointsToPaths(ThreadTmpData& tmpData);
		void cleanPerROIStructures(ThreadTmpData& tmpData);
		void savePaths(sequence::PathStream& pathStream, size_t frameId);
		*/
		void visualize();

		DefaultPathDetector m_PathDetector;
	};

	/*template<typename T>
	T DefaultSequenceDetector::getParameter(const ProgramOptions& options, const std::string& paramName, const T& defVal)
	{
		T result;
		if (options.get<T>(paramName, result))
			return result;

		// TODO pobieranie z pliku konfiguracyjnego
		return defVal;
	}*/

}

#endif // Detector_h__