#include "PathAnalysisAlgorithms/Partition/PartitionAnalyzer.h"
#include "PathAnalysisAlgorithms/Partition/PartitionMath.h"

#include "utils/RandomColor.h"

#include <sequence/PathStream.h>
#include <sequence/Cluster.h>
#include <sequence/CompressedVideo.h>

#include "config/PartitionAnalyzer.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <QtCore/qmath.h>

#include <boost/timer/timer.hpp>

#include <algorithm>

namespace clustering
{
	namespace partition
	{
		Analyzer::Analyzer()
		{
			config::PartitionAnalyzer::getInstance().load();
			Frame::loadParameters();
			SequenceGroup::loadParameters();

			Math::initialize();
		}

		void Analyzer::registerParameters(ProgramOptions& programOptions)
		{
			programOptions.add<std::string>("ov", "output video path");
		}

		bool Analyzer::loadParameters(const ProgramOptions& options)
		{
			if (isVisualize())
				createWindow();

			std::string videoPath;
			if (options.get("ov", videoPath))
				m_videoWriter.open(videoPath, CV_FOURCC('M', 'P', '4', '2'), 25.0, cv::Size(1920, 1080), true);

			return true;
		}

		void Analyzer::process()
		{
			boost::timer::cpu_timer timer;

			loadPaths();
			createFrames();
			detectClusters();
			createSequences();
			waitIdle();
			saveSequences();
			cleanup();

			auto msTime = timer.elapsed().wall / 1000000;
			std::cout << "TIME: " << msTime << " ms" << std::endl;

			if (isVisualize())
				visualize();
		}

		void Analyzer::loadPaths()
		{
			m_Frames.clear();
			PathsMap seqPaths;

			std::cout << "Loading path file: " << getInFileName() << std::endl;
			sequence::PathStream pathStream(getInFileName());
			pathStream.getPaths(seqPaths);

			std::cout << "Grouping paths.." << std::endl;
			m_PathsGroup = std::make_shared<PathGroup>(seqPaths, config::PartitionAnalyzer::getInstance().getPathMinMovement());
		}


		void Analyzer::createFrames()
		{
			for (m_FrameNr = m_PathsGroup->getStartTime(); m_FrameNr <= m_PathsGroup->getEndTime(); ++m_FrameNr)
			{
				m_Frames.push_back(std::make_shared<Frame>(m_PathsGroup.get(), m_FrameNr));
				m_Frames.back()->getPaths();
			}
		}

		void Analyzer::detectClusters()
		{
			std::cout << "Clustering paths.." << std::endl;
			
			#pragma omp parallel for schedule(dynamic)
			for (int i = 0; i < m_Frames.size(); ++i)
			{
				m_Frames[i]->detectClusters();
			}

			std::cout << "Assigning ids.." << std::endl;
			for (size_t i = 0; i < m_Frames.size(); ++i)
			{
				i == 0 ? m_Frames[0]->copyClusterId(m_LastPathClusters) : m_Frames[i]->filterClusters();
				m_Frames[i]->reboundClusters();
				m_Frames[i]->assignClusterId();
			}
			correctIds();
		}

		void Analyzer::createSequences()
		{
			m_Sequences = std::make_shared<SequenceGroup>(m_Frames);
			m_Sequences->stabilizeSequences();
			m_Sequences->computeSpeeds();
			m_Sequences->mergeSequences();
			m_Sequences->stabilizeSequences();
			m_Sequences->filter();

			m_LastPathClusters = m_PathsGroup->getPathClusters();
		}

		void Analyzer::saveSequences()
		{
			std::vector<std::shared_ptr<sequence::Cluster>> clusters = m_Sequences->save();
			for (auto& cluster : clusters)
				saveCluster(*cluster);
		}

		void Analyzer::correctIds()
		{
			m_PathsGroup->calculateIds();

			for (auto& frame : m_Frames)
			{
				for (auto& cluster : frame->getClusters())
					cluster->generateId(false);
			}
		}

		void Analyzer::createWindow()
		{
			cv::namedWindow("output");
		}

		void Analyzer::visualize()
		{
			auto video = sequence::IVideo::create(getVideoFileName());
			for (m_FrameNr = 0; m_FrameNr <= video->getNumFrames(); ++m_FrameNr)
			{
				video->getNextFrame(m_Frame);

				drawBounds(true, false, false);
				drawFrame();
			}
		}

		void Analyzer::drawBounds(bool showPaths, bool showCoords, bool showPoints) const
		{
			m_Sequences->drawPaths(m_Frame, m_ColorMap, showCoords);

			for (auto cluster : m_Sequences->getFrame(m_FrameNr))
			{
				if (showPaths)
					cluster->drawPaths(m_Frame, m_ColorMap);
				cv::rectangle(m_Frame, cluster->getTopLeft(), cluster->getBottomRight(), m_ColorMap[cluster->getId()], cluster->getId() < 0 ? 1 : 2);
			}

			if (showPoints)
				m_Sequences->drawPoints(m_Frame, m_ColorMap, m_FrameNr);
		}

		void Analyzer::drawFrame() const
		{
			cv::imshow("output", m_Frame);
			if (m_videoWriter.isOpened())
			{
				m_videoWriter.write(m_Frame);
				cv::waitKey(1);
			}
			else
				cv::waitKey(0);
		}
	}
}