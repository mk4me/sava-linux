#include "PathAnalysisAlgorithms/DefaultPathAnalyzer.h"

#include "../../PathAnalysis/src/PathAnalysisVisualizer.h"
#include "../../PathAnalysis/src/NullVisualizer.h"
#include "PathAnalysisAlgorithms/StatisticPathDistanceFactory.h"
#include "../../PathAnalysis/src/ConfigurationWindow.h"

#include <PathAnalysisAlgorithms/algorithms/FastPathClustering.h>
#include <PathAnalysisAlgorithms/algorithms/OptimizedPathDistanceFactory.h>
#include <PathAnalysisAlgorithms/algorithms/PathEstimator.h>

#include <sequence/PathStream.h>
#include <sequence/Cluster.h>
#include <sequence/CompressedVideo.h>

#include <config/PathAnalysis.h>

#include <utils/Filesystem.h>

#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/filesystem.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace clustering
{

	DefaultPathAnalyzer::DefaultPathAnalyzer()
		: m_StartTime(0)
		, m_LastMaxPathId(0)
		, m_MaxClusterWidth(300)
		, m_MaxClusterHeight(500)
		, m_MinPathsInCluster(0)
		, m_RoiEstimation(true)
	{

	}

	DefaultPathAnalyzer::~DefaultPathAnalyzer()
	{

	}

	/*void DefaultPathAnalyzer::registerParameters(ProgramOptions& programOptions)
	{

	}*/

	bool DefaultPathAnalyzer::loadParameters(const ProgramOptions& options)
	{
		if (isVisualize())
			m_Visualizer = std::make_shared<PathAnalysisVisualizer>();
		else
			m_Visualizer = std::make_shared<NullVisualizer>();


		config::PathAnalysis& config = config::PathAnalysis::getInstance();

		m_PathClustering = config.isUseOpenMP() ? std::make_shared<motion_analysis::FastPathClustering>() : std::make_shared<motion_analysis::PathClustering>();
		m_PathEstimator = std::make_shared<motion_analysis::PathEstimator>();

		motion_analysis::PathDistanceFactory::setFactory<motion_analysis::OptimizedPathDistanceFactory>();
		//motion_analysis::PathDistanceFactory::setFactory(std::make_shared<StatisticPathDistanceFactory>(std::make_shared<motion_analysis::PathDistanceFactory>()));

		motion_analysis::PathClusteringParams& params = m_PathClustering->params;
		params.alpha = config.getAlpha();
		params.minimumPathLength = config.getMinimumPathLength();

		params.pathThresholds.total = config.getPathThresholds().getTotal();
		params.pathThresholds.positional = config.getPathThresholds().getPositional();
		params.pathThresholds.tangent = config.getPathThresholds().getTangent();

		params.clusterThresholds.total = config.getClusterThresholds().getTotal();
		params.clusterThresholds.positional = config.getClusterThresholds().getPositional();
		params.clusterThresholds.tangent = config.getClusterThresholds().getTangent();

		params.weightX = config.getWeightX();
		params.weightY = config.getWeightY();
		params.weightTau = config.getWeightTau();

		params.enableMerging = config.getEnableMerging();
		params.enableClusterRemoving = config.getEnableClusterRemoving();

		params.enableMovingPaths = config.getEnableMovingPaths();
		params.pathsPercentageMovesPerFrame = config.getPathsPercentageMovesPerFrame();
		params.minPathsMovesPerFrame = config.getMinPathsMovesPerFrame();
		params.createNewCluster = config.isCreateNewCluster();
		params.tryMovingOnlyWhenCurrentIsToFar = config.isTryMovingOnlyWhenCurrentIsTooFar();

		params.enableMergingGaps = config.getEnableMergingGaps();
		params.mergingThresholds.time = config.getMergingThresholds().getTime();
		params.mergingThresholds.x = config.getMergingThresholds().getX();
		params.mergingThresholds.y = config.getMergingThresholds().getY();
		params.mergingOldestClusterTime = config.getMergingOldestClusterTime();

		m_MinProcessTime = config.getMinProcessTime();
		m_MaxClusterWidth = config.getMaxClusterWidth();
		m_MaxClusterHeight = config.getMaxClusterHeight();

		m_MinPathsInCluster = config.getMinPathsInCluster();

		m_RoiEstimation = config.isRoiEstimation();
		motion_analysis::PathEstimator::CENTER_PARAM_THRESHOLD = config.getRoiCenterThreshold();
		motion_analysis::PathEstimator::CENTER_PARAM_W = config.getRoiCenterW();
		motion_analysis::PathEstimator::BOUNDING_BOX_PARAM_R = config.getBoundingBoxR();
		motion_analysis::PathEstimator::BOUNDING_BOX_PARAM_T = config.getBoundingBoxT();

		std::cout << "\nclustering parameters: \n";
		for (auto it = params.wrappers.begin(); it != params.wrappers.end(); ++it)
		{
			std::cout << std::setw(35) << it->second->getName() << ": " << it->second->getStringValue() << '\n';
		}
		std::cout << std::setw(35) << "Max cluster widht: " << m_MaxClusterWidth << '\n';
		std::cout << std::setw(35) << "Max cluster height: " << m_MaxClusterHeight << '\n';
		std::cout << std::setw(35) << "Min paths in cluster: " << m_MinPathsInCluster << '\n';

		if (isVisualize())
		{
			m_ConfigurationWindow = std::make_shared<ConfigurationWindow>("Clustering", params);
		}

		return true;
	}

	void DefaultPathAnalyzer::process()
	{
		std::cout << "Processing file " << getInFileName() << std::endl;
		boost::timer::cpu_timer timer;

		sequence::PathStream pathStream(getInFileName());

		m_Visualizer->loadVideo(getVideoFileName());

		m_StartTime = m_PathClustering->getTime();
		//unsigned long long maxPathId = m_LastMaxPathId;
		std::set<uint64_t> paths;
		int pathPreRemoved = 0;
		while (pathStream.grabFrame())
		{
			sequence::PathStream::Id id;
			sequence::PathStream::Point point;
			while (pathStream.grabPath(id, point))
			{
				switch (point.x)
				{
				case -2:
					m_PathClustering->removePath(id);
					++pathPreRemoved;
					break;
				case -1:
					break;
				default:
					m_PathClustering->insertPath(id, 0, point.x, point.y);
					paths.insert(id);
				}
			}
			m_PathClustering->update();
			m_PathEstimator->update(m_PathClustering->getClusters(), m_PathClustering->getTime());

			m_PathClustering->setTime(m_PathClustering->getTime() + 1);

			m_Visualizer->visualize(*m_PathClustering);

			//StatisticPathDistanceFactory::printAndClearStats();
		}
		//m_LastMaxPathId = maxPathId;

		int pathPostRemoved = 0;
		for (auto& p : m_PathClustering->getPaths())
		{
			if (paths.find(p.first) == paths.end())
			{
				m_PathClustering->removePath(p.first);
				++pathPostRemoved;
			}
		}

		std::cout << "paths pre-removed = " << pathPreRemoved << "\npaths post-removed = " << pathPostRemoved << "\npaths left = " << m_PathClustering->getPaths().size() << "\nclusters left = " << m_PathClustering->getClusters().size() << std::endl;
		
		waitIdle();

		//boost::timer::cpu_timer timer;
		if (m_RoiEstimation)
			saveEstimated();
		else
			save();

		cleanup();

		timer.stop();
		auto elapsed = timer.elapsed().wall / 1000000;
		std::cout << "\tdone in " << elapsed << " ms" << std::endl;
	}

	static void SMAonVector(const std::vector<sequence::PathStream::Point>& inVec, std::vector<sequence::PathStream::Point>& outVec, int windowSize)
	{
		// Resize out
		outVec.resize(inVec.size());

		// Copy 'window' elements
		for (int i = 0; i < windowSize; ++i)
			outVec[i] = inVec[i];

		// Iterate over rest of the sequence
		for (int i = windowSize; i < inVec.size(); ++i)
		{
			// Forward sum
			sequence::PathStream::Point fSum(0, 0);
			for (int f = 0; f < windowSize; ++f)
			{
				// Sum 
				fSum += inVec[i - f - 1];
			}

			// SMA
			outVec[i] = fSum / windowSize;
		}
	}

	// KL: Test smoothing function
	static void superSmooth(const sequence::Cluster::FramesPositionsMap& inPositions, sequence::Cluster::FramesPositionsMap& outPositions)
	{
		const int windowSize = 5;

		size_t buffLen = inPositions.size();

		// Need more frames
		if (buffLen <= windowSize)
		{
			outPositions = inPositions;
			return;
		}

		// Copy map to vector
		std::vector<sequence::PathStream::Point> pointList, revPointList;
		for (const auto& elem : inPositions)
			pointList.push_back(elem.second);
		revPointList = pointList;
		std::reverse(revPointList.begin(), revPointList.end());

		// Calculate in both directions
		outPositions.clear();
		std::vector<sequence::PathStream::Point> forwardAvg, backwardAvg;
		SMAonVector(pointList, forwardAvg, windowSize);
		SMAonVector(revPointList, backwardAvg, windowSize);

		// Make final list
		int cnt = 0;
		for (const auto& elem : inPositions)
		{
			outPositions[elem.first] = (forwardAvg[cnt] + backwardAvg[buffLen - cnt - 1]) / 2;
			++cnt;
		}

	}

	// Draw sequence of frames
	static void drawTrajectory(const sequence::Cluster::FramesPositionsMap& inPositions, const std::string& wndCaption)
	{
		cv::Mat mat(1080, 1920, CV_8UC3);

		size_t colLen = inPositions.size();
		if (!colLen)
			return;

		int cnt = 0;
		auto it = inPositions.begin();
		double intensity;
		for (auto it_prev = it++; it != inPositions.end(); it_prev = it++, ++cnt)
		{
			intensity = cnt / (double)colLen;
			cv::line(mat, it_prev->second, it->second, cv::Scalar(0, 255 * (1.0 - intensity), 255 * intensity));
		}

		cv::namedWindow(wndCaption);
		cv::imshow(wndCaption, mat);
		std::cout << wndCaption << " trajectory drawn" << std::endl;
	}

	void DefaultPathAnalyzer::save()
	{
		auto& finishedClusters = m_PathClustering->getFinishedClusters();
		auto& almostFinishedClusters = m_PathClustering->getAlmostFinishedClusters();
		auto& currentClusters = m_PathClustering->getClusters();

		struct Cmp
		{
			typedef std::shared_ptr<motion_analysis::PathCluster> C;
			bool operator()(const C& c1, const C& c2) const { return c1->elementsCount() > c2->elementsCount(); }
		};

		std::set<std::shared_ptr<motion_analysis::PathCluster>, Cmp> clusters;
		clusters.insert(finishedClusters.begin(), finishedClusters.end());
		clusters.insert(almostFinishedClusters.begin(), almostFinishedClusters.end());
		clusters.insert(currentClusters.begin(), currentClusters.end());
#define PATH_ANALYSIS_USE_CRUMBLES 0
#if PATH_ANALYSIS_USE_CRUMBLES == 1
		sequence::CompressedVideo compressedVideo(getVideoFileName());

		std::vector<std::vector<cv::Rect>> allCrumbles;
		for (size_t i = 0; i < compressedVideo.getNumFrames(); ++i)
		{
			auto crumbles = compressedVideo.getFrameCrumbles(i);
			allCrumbles.push_back(std::move(crumbles));
		}
#endif
		unsigned clusterFileNr = 0;
		bool noClusters = true;
		for (auto& c : clusters)
		{
			if (c->elements.size() < m_MinPathsInCluster)
				continue;

			std::vector<cv::Rect> contours;

#if PATH_ANALYSIS_USE_CRUMBLES == 0
			for (int t = m_StartTime; t < m_PathClustering->getTime(); ++t)
			{
				contours.push_back(c->getContour(t, cv::Size(30, 30)));
			}
#endif
#if PATH_ANALYSIS_USE_CRUMBLES == 1
			std::vector<cv::Rect> origContours;
			for (int t = m_StartTime; t < m_PathClustering->getTime(); ++t)
			{
				origContours.push_back(c->getContour(t, cv::Size(30, 30)));
			}


			for (size_t frame = 0; frame < origContours.size(); ++frame)
			{
				std::vector<cv::Rect>& crumbles = allCrumbles[frame];
				cv::Rect& origContour = origContours[frame];

				for (auto c = crumbles.begin(); c != crumbles.end();)
				{
					cv::Rect& crumble = *c;
					if ((origContour & crumble).area() > 0)
					{
						if (contours.size() == frame)
							contours.push_back(crumble);
						else
							contours[frame] = contours[frame] | crumble;

						c = crumbles.erase(c);
					}
					else
					{
						++c;
					}
				}

				if (contours.size() == frame)
				{
					//contours.push_back(origContour);

					break;
				}
			}
#endif


			cv::Size maxSize(-1, -1);
			for (cv::Rect& contour : contours)
			{
				if (contour.height > maxSize.height)
				{
					maxSize.height = contour.height;
				}

				if (contour.width > maxSize.width)
				{
					maxSize.width = contour.width;
				}
			}
			if ((maxSize.width == -1) || (maxSize.height == -1))
				continue;

			if (maxSize.width > m_MaxClusterWidth || maxSize.height > m_MaxClusterHeight)
			{
				std::cout << "Cluster is too big (" << maxSize.width << "x" << maxSize.height << ")" << std::endl;
				continue;
			}

			sequence::Cluster clusterFile(c->id);
			clusterFile.setFrameSize(maxSize);

			static const int SMOOTHING_RADIOUS = 10;

			{
				cv::Point avgSizePoint(maxSize.width, maxSize.height);
				sequence::Cluster::FramesPositionsMap positions;
				for (size_t t = 0; t < contours.size(); ++t)
				{
					cv::Rect& contour = contours[t];
					if (contour.x != -1)
					{
						cv::Point position = contour.tl() + (cv::Point(contour.size().width, contour.size().height) - avgSizePoint) / 2;
						positions.insert(std::make_pair(t, position));
					}
				}

				if (positions.empty())
					continue;

				long long firstFrame = positions.begin()->first;
				long long lastFrame = positions.rbegin()->first;

				sequence::Cluster::FramesPositionsMap smoothedPositions, itOne;
				superSmooth(positions, itOne);
				superSmooth(itOne, smoothedPositions);

				clusterFile.setFramesPositions(std::move(smoothedPositions));
			}

			if (clusterFile.getEndFrame() - clusterFile.getStartFrame() < 25)
				continue;

			// save file
			noClusters = false;
			saveCluster(clusterFile);
		}
		if (noClusters)
		{
			// save empty file
			sequence::Cluster clusterFile;
			saveCluster(clusterFile);
		}
		std::cout << clusterFileNr << " files saved." << std::endl;
		m_PathClustering->resetFinishedClusters();
	}

	void DefaultPathAnalyzer::saveEstimated()
	{
		auto& finishedClusters = m_PathClustering->getFinishedClusters();
		auto& almostFinishedClusters = m_PathClustering->getAlmostFinishedClusters();
		auto& currentClusters = m_PathClustering->getClusters();

		struct Cmp
		{
			typedef std::shared_ptr<motion_analysis::PathCluster> C;
			bool operator()(const C& c1, const C& c2) const { return c1->elementsCount() > c2->elementsCount(); }
		};

		std::set<std::shared_ptr<motion_analysis::PathCluster>, Cmp> clusters;
		clusters.insert(finishedClusters.begin(), finishedClusters.end());
		clusters.insert(almostFinishedClusters.begin(), almostFinishedClusters.end());
		clusters.insert(currentClusters.begin(), currentClusters.end());

		int frameCount = m_PathClustering->getTime() - std::max(0LL, m_StartTime - 15);
		int endFrame = m_PathClustering->getTime() - 1;

		std::cout << "frameCount = " << frameCount << "\nendFrame = " << endFrame << std::endl;

		unsigned clusterFileNr = 0;
		bool noClusters = true;
		for (auto& c : clusters)
		{
			if (c->elements.size() < m_MinPathsInCluster)
				continue;

			auto clusterBoxes = m_PathEstimator->getBoundingBoxes(c, endFrame, frameCount);
			
			sequence::Cluster clusterFile(c->id);

			cv::Size maxSize;
			sequence::Cluster::FramesPositionsMap positions;

			for (auto b : clusterBoxes)
			{
				auto t = b.first - m_StartTime;
				if (t < 0)
					continue;

				auto box = b.second;

				maxSize.width = std::max(maxSize.width, box.width);
				maxSize.height = std::max(maxSize.height, box.height);

				positions[t] = box.tl();
			}

			clusterFile.setFrameSize(maxSize);
			clusterFile.setFramesPositions(positions);

			if (clusterFile.getEndFrame() - clusterFile.getStartFrame() < 25)
				continue;

			// save file
			noClusters = false;
			saveCluster(clusterFile);
		}

		if (noClusters)
		{
			// save empty file
			sequence::Cluster clusterFile;
			saveCluster(clusterFile);
		}
		std::cout << clusterFileNr << " files saved." << std::endl;
		m_PathClustering->resetFinishedClusters();
	}

	void DefaultPathAnalyzer::show()
	{
		m_Visualizer->show();
	}

}