#pragma once
#ifndef DefaultPathDetector_h__
#define DefaultPathDetector_h__

#include "IPathDetector.h"

#include <utils/Hungarian.h>

class path;

namespace config
{
	class PathDetection;
}

namespace clustering
{
	class DefaultPathDetector : public IPathDetector
	{
	public:
		virtual void initialize() override;
		virtual void processFrame(const cv::Mat& frame) override;
		virtual void visualize(utils::ZoomObjectCollection& zoomObjects) override;

		virtual size_t getPathsCount() const override { return m_Paths.size(); }

	protected:
		config::PathDetection& getConfig();

		void processRegions();

		virtual void savePaths(sequence::PathStream& pathStream);

	private:
		std::vector<path*> m_Paths;
		std::vector<cv::Rect> m_ROIRegions;
		cv::Mat m_VideoFrame;

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

		void initPerROIStructures(ThreadTmpData& tmpData, const cv::Rect& roi);
		void createCostMatrix(ThreadTmpData& tmpData);
		void match(ThreadTmpData& tmpData);
		void appendPointsToPaths(ThreadTmpData& tmpData);
		void cleanPerROIStructures(ThreadTmpData& tmpData);
	};
}

#endif // DefaultPathDetector_h__