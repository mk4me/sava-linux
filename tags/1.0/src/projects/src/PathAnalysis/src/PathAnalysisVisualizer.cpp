#include "PathAnalysisVisualizer.h"

#include <motionanalysis/algorithms/PathClustering.h>
#include <motionanalysis/algorithms/PathDistance.h>
#include <motionanalysis/algorithms/PathDistanceFactory.h>

#include <sequence/StreamedVideo.h>

#include <config/PathAnalysis.h>

#define SAVE_OUTPUT_ENABLED 1

namespace clustering
{

	PathAnalysisVisualizer::PathAnalysisVisualizer()
	{
		/*for (int r = 31; r < 256; r += 32)
		{
			for (int g = 31; g < 256; g += 32)
			{
				for (int b = 31; b < 256; b += 32)
				{
					m_ColorPalette.push_back(CV_RGB(r, g, b));
				}
			}
		}*/
		/*std::random_device rd;
		std::mt19937 g(rd());

		std::shuffle(m_ColorPalette.begin(), m_ColorPalette.end(), g);*/
	}

	PathAnalysisVisualizer::~PathAnalysisVisualizer()
	{

	}

	void PathAnalysisVisualizer::loadVideo(const std::string& filename)
	{
		m_Video = sequence::IStreamedVideo::create(filename);
	}

	void PathAnalysisVisualizer::visualize(motion_analysis::PathClustering& pathClustering)
	{
		cv::Mat frame;
		if (!m_Video->getNextFrame(frame))
		{
			std::cerr << "PathAnalysisVisualizer::visualize(): No more frames in video." << std::endl;
			return;
		}

		utils::ZoomObjectCollection zoomObjects;
		int t = pathClustering.getTime();



		for (auto& p : pathClustering.getPaths())
		{
			auto& path = p.second;
			auto assignedCluster = pathClustering.getAssignedCluster(path->id);
			cv::Scalar color = assignedCluster && assignedCluster->elements.size() >= 20 ? cv::Scalar(m_ClusterColorMap[assignedCluster->id]) : cv::Scalar(CV_RGB(0, 0, 0));
			//cv::Scalar color = m_PathColorMap[p.first];

			/*if (assignedCluster)
			{
				auto distance = motion_analysis::PathDistanceFactory::create(*assignedCluster, *path, pathClustering.params);

				std::cout << "Distance clu " << assignedCluster->id << " pth " << path->id
					<< "\ntot: " << distance->total
					<< " disp: " << distance->dispertion
					<< " sqDisp: " << distance->squaredDispertion
					<< " len:" << distance->length
					<< " tan:" << distance->tangentialComponent << std::endl;
			}*/

			auto prev = path->begin();
			auto next = std::next(prev);
			for (; next != path->end(); ++prev, ++next)
			{
				//auto dt = 1.0f - std::min(t - next->t, 100) / 100.0f;
				cv::Scalar finalColor = color;
				//finalColor *= dt;
				utils::zoomLine(zoomObjects, cv::Point(prev->x, prev->y), cv::Point(next->x, next->y), finalColor);
			}
		}

		for (auto& cluster : pathClustering.getClusters())
		{
			if (cluster->elements.size() < config::PathAnalysis::getInstance().getMinPathsInCluster())
				continue;

			cv::Scalar color = m_ClusterColorMap[cluster->id];
			for (auto& point : *cluster)
			{
				utils::zoomCircle(zoomObjects, cv::Point(point.x + cluster->b.x, point.y + cluster->b.y), 3, color, 2);
			}
			cv::Rect clusterContour = cluster->getContour(pathClustering.getTime() - 1);
			utils::zoomRectangle(zoomObjects, clusterContour, color, 2);
		}
		//getchar();

#if SAVE_OUTPUT_ENABLED
		{
			cv::Mat tmpImage = frame.clone();

			class DummyZoomTransform : public utils::IZoomTransform
			{
			public:
				virtual cv::Point globalToZoomed(const cv::Point& global) const override { return global; }
				virtual cv::Point zoomedToGlobal(const cv::Point& zoomed) const override { return zoomed; }
				virtual float getRatio() const override { return 1.0f; }
			};
			utils::IZoomTransformConstPtr zoomTransform = std::make_shared<DummyZoomTransform>();
			for (auto& zo : zoomObjects)
			{
				zo->draw(tmpImage, zoomTransform);
			}
			static int frameNo = 0;
			std::ostringstream oss;
			oss << std::setfill('0') << std::setw(5) << frameNo++;
			cv::imwrite("F:\\2016_05_17\\path_analysis\\frame_" + oss.str() + ".jpg", tmpImage);
		}

		std::lock_guard<std::mutex> lock(m_VisualizeMutex);
		if (!frame.empty())
			frame.copyTo(m_VisualizationFrame);
		m_ZoomObjects = zoomObjects;
	}
#endif // SAVE_OUTPUT_ENABLED

	void PathAnalysisVisualizer::show()
	{
		std::lock_guard<std::mutex> lock(m_VisualizeMutex);
		utils::zoomShow("Path analysis visualization", m_VisualizationFrame, m_ZoomObjects);
		cv::waitKey(1);
	}

}