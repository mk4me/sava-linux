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
	/// <summary>
	/// klasy implementuj¹ce wykrywanie cie¿ek poprzez wykrycie punktów charakterystycznych metodami SIFT lub SURF oraz dopasowaniem ich do istniej¹cych cie¿ek algorytmem wêgierskim.
	/// </summary>
	/// <seealso cref="IPathDetector" />
	class DefaultPathDetector : public IPathDetector
	{
	public:
		virtual void initialize() override;
		virtual void processFrame(const cv::Mat& frame) override;
		virtual void visualize(utils::ZoomObjectCollection& zoomObjects) override;

		virtual size_t getPathsCount() const override { return m_Paths.size(); }

	protected:
		config::PathDetection& getConfig();

		void processRegions(const std::vector<cv::Mat>& detectorMasks = std::vector<cv::Mat>(), cv::Mat appendMask = cv::Mat());

		virtual void savePaths(sequence::PathStream& pathStream);

		std::vector<path*> m_Paths;
		std::vector<cv::Rect> m_ROIRegions;
		cv::Mat m_VideoFrame;

	private:
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

		void initPerROIStructures(ThreadTmpData& tmpData, const cv::Rect& roi, cv::InputArray mask = cv::noArray());
		void createCostMatrix(ThreadTmpData& tmpData);
		void match(ThreadTmpData& tmpData);
		void appendPointsToPaths(ThreadTmpData& tmpData, cv::Mat mask = cv::Mat());
		void cleanPerROIStructures(ThreadTmpData& tmpData);
	};
}

#endif // DefaultPathDetector_h__