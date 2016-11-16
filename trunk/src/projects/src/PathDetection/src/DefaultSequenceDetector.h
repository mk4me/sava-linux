#pragma once
#ifndef Detector_h__
#define Detector_h__

#include "ISequenceDetector.h"
#include "DefaultPathDetector.h"
#include "PredictedRoiPathDetector.h"

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
				
		cv::Mat m_VideoFrame;		
		void visualize();

		std::shared_ptr<DefaultPathDetector> m_PathDetector;
		//PredictedRoiPathDetector m_PathDetector;
	};
}

#endif // Detector_h__