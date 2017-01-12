#pragma once
#ifndef PathAnalysisVisualizer_h__
#define PathAnalysisVisualizer_h__

#include "IVisualizer.h"

#include <utils/CvZoom.h>
#include <utils/RandomColor.h>

#include <opencv2/core.hpp>

#include <mutex>
#include <random>

namespace motion_analysis
{
	class PathClustering;
	class PathCluster;
}

namespace sequence
{
	class IStreamedVideo;
}

namespace clustering
{
	/// <summary>
	/// klasa implementuj¹ca mechanizmy wizualizacji. Realizuje interfejs IVisualizer.
	/// </summary>
	/// <seealso cref="IVisualizer" />
	class PathAnalysisVisualizer : public IVisualizer
	{
	public:
		PathAnalysisVisualizer();
		~PathAnalysisVisualizer();

		virtual void loadVideo(const std::string& filename) override;
		virtual void visualize(motion_analysis::PathClustering& pathClustering) override;
		virtual void show() override;

	private:
		std::mutex m_VisualizeMutex;
		cv::Mat m_VisualizationFrame;
		utils::ZoomObjectCollection m_ZoomObjects;

		std::shared_ptr<sequence::IStreamedVideo> m_Video;

		//std::vector<cv::Scalar> m_ColorPalette;
		//std::mt19937 m_ColorPickingEngine;
		utils::RandomColor m_RandomColor;

		std::map<int, utils::RandomColor> m_ClusterColorMap;
		std::map<unsigned long long, utils::RandomColor> m_PathColorMap;
	};
}

#endif // PathAnalysisVisualizer_h__