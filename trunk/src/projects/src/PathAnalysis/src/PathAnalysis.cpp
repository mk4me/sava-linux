#include "PathAnalysis.h"

#include <motionanalysis\algorithms\FastPathClustering.h>

#include <sequence/PathStream.h>
#include <sequence/Cluster.h>

#include <config/PathAnalysis.h>

#include <utils/PipeProcessUtils.h>
#include <utils/FileLock.h>

#include <boost\timer\timer.hpp>

namespace clustering
{
	const std::string PathAnalysis::INPUT_FILE_EXTENSION("path");
	const std::string PathAnalysis::OUTPUT_FILE_EXTENSION("clu");

	PathAnalysis::PathAnalysis()
		: utils::PipeProcess(INPUT_FILE_EXTENSION)
		, m_StartTime(0)
		, m_LastMaxPathId(0)
	{

	}

	PathAnalysis::~PathAnalysis()
	{

	}

	utils::IAppModule* PathAnalysis::create()
	{
		return new PathAnalysis();
	}

	void PathAnalysis::registerParameters(ProgramOptions& programOptions)
	{
		PipeProcess::registerParameters(programOptions);
	}

	bool PathAnalysis::loadParameters(const ProgramOptions& options)
	{
		std::cout << "================================================================================\n";
		if (!PipeProcess::loadParameters(options))
			return false;

		config::PathAnalysis config;
		config.load();
				
		m_PathClustering = config.isUseOpenMP() ? std::make_shared<motion_analysis::FastPathClustering>() : std::make_shared<motion_analysis::PathClustering>();

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

		std::cout << "\nclustering parameters: \n";
		for (auto it = params.wrappers.begin(); it != params.wrappers.end(); ++it)
		{
			std::cout << std::setw(35) << it->second->getName() << ": " << it->second->getStringValue() << '\n';
		}

		std::cout << "================================================================================\n";

		return true;
	}

	void PathAnalysis::reserve()
	{
		utils::PipeProcessUtils::IndicesVectorSet outFileNumbers;
		utils::PipeProcessUtils::IndicesVector pattern = { -1 };
		utils::PipeProcessUtils::getFilesIndices(outFileNumbers, pattern, getOutputFolder(), getFilePattern(), OUTPUT_FILE_EXTENSION, 2);

		int lastFileId = -1;
		if (!outFileNumbers.empty())
		{
			assert(outFileNumbers.rbegin()->size() == 1);
			lastFileId = outFileNumbers.rbegin()->at(0);
		}

		std::set<int> inFileNumbers;
		utils::PipeProcessUtils::getFilesIndices(inFileNumbers, getInputFolder(), getFilePattern(), INPUT_FILE_EXTENSION);

		auto nextFileNumberIt = inFileNumbers.upper_bound(lastFileId);
		if (nextFileNumberIt != inFileNumbers.end())
		{
			m_FileNumber = *nextFileNumberIt;
			m_FileLock = std::make_shared<utils::FileLock>(getInFileName() + ".loc");
			if (m_FileLock->lock())
			{
				setState(PipeProcess::PROCESS);
				return;
			}
		}

		waitForFile();
	}

	void PathAnalysis::process()
	{
		std::cout << "Processing file " << getInFileName() << std::endl;
		boost::timer::cpu_timer timer;

		sequence::PathStream pathStream(getInFileName());

		m_StartTime = m_PathClustering->getTime();
		unsigned long long maxPathId = m_LastMaxPathId;
		while (pathStream.grabFrame())
		{
			sequence::PathStream::Id id;
			sequence::PathStream::Point point;
			while (pathStream.grabPath(id, point))
			{
				id += m_LastMaxPathId;
				if (maxPathId < id)
					maxPathId = id;

				switch (point.x)
				{
				case -2:
					m_PathClustering->removePath(id);
					break;
				case -1:
					break;
				default:
					m_PathClustering->insertPath(id, 0, point.x, point.y);
				}
			}
			m_PathClustering->update();
			m_PathClustering->setTime(m_PathClustering->getTime() + 1);
		}
		m_LastMaxPathId = maxPathId;
		timer.stop();
		auto elapsed = timer.elapsed().wall / 1000000;

		std::cout << "\tdone in " << elapsed << " ms" << std::endl;
		setState(PipeProcess::FINALIZE);
	}

	void PathAnalysis::finalize()
	{
		std::cout << "Finalizing file " << getInFileName() << std::endl;
		boost::timer::cpu_timer timer;
		save();
		if (false)
			cleanup();
		m_FileLock.reset();

		timer.stop();
		auto elapsed = timer.elapsed().wall / 1000000;
		std::cout << "\tdone in " << elapsed << " ms" << std::endl;

		setState(PipeProcess::RESERVE_FILE);
	}

	std::string PathAnalysis::getInFileName() const
	{
		FilesystemPath inputFile = getInputFolder();
		inputFile += getFilePattern();
		return inputFile.string() + "." + std::to_string(m_FileNumber) + "." + INPUT_FILE_EXTENSION;
	}

	std::string PathAnalysis::getOutFileName(unsigned subId) const
	{
		FilesystemPath inputFile = getOutputFolder();
		inputFile += getFilePattern();
		return inputFile.string() + "." + std::to_string(m_FileNumber) + "." + std::to_string(subId) + "." + OUTPUT_FILE_EXTENSION;
	}

	void PathAnalysis::cleanup()
	{
		try
		{
			if (boost::filesystem::exists(getInFileName()))
				boost::filesystem::remove(getInFileName());
		}
		catch (...)
		{
			std::cerr << "Compression::cleanup(): Can't remove file \"" << getInFileName() << "\"" << std::endl;
		}
	}

	void PathAnalysis::save()
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
		
		unsigned clusterFileNr = 0;
		bool noClusters = true;
		for (auto& c : clusters)
		{
			std::vector<cv::Rect> contours;
			for (int t = m_StartTime; t < m_PathClustering->getTime(); ++t)
			{
				contours.push_back(c->getContour(t, cv::Size(30, 30)));
			}

			cv::Size avgSize;
			int count = 0;
			for (cv::Rect& contour : contours)
			{
				if (contour.x != -1)
				{
					avgSize += contour.size();
					++count;
				}
			}
			if (count == 0)
				continue;

			sequence::Cluster clusterFile(c->id);
			avgSize /= count;
			clusterFile.setFrameSize(avgSize);

			static const int SMOOTHING_RADIOUS = 10;

			{
				cv::Point avgSizePoint(avgSize.width, avgSize.height);
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

				sequence::Cluster::FramesPositionsMap smoothedPositions;
				for (auto& entry : positions)
				{
					cv::Point sum;
					int count = 0;

					long long t = entry.first;

					long long from = t - SMOOTHING_RADIOUS;
					long long to = t + SMOOTHING_RADIOUS;					
					for (long long i = from; i <= to; ++i)
					{
						cv::Point p;
						if (i < firstFrame)
						{
							p = sequence::Cluster::lerp(positions, 2 * firstFrame - i);
							p = 2 * entry.second - p;
						}
						else if (i > lastFrame)
						{
							p = sequence::Cluster::lerp(positions, 2 * lastFrame - i);
							p = 2 * entry.second - p;
						}
						else
							p = sequence::Cluster::lerp(positions, i);

						sum += p;
						++count;
					}
					if (count > 0)
					{
						smoothedPositions[t] = sum / count;
					}
				}
				
				clusterFile.setFramesPositions(std::move(smoothedPositions));
			}

			// save file
			std::string outFile = getOutFileName(clusterFileNr++);
			utils::FileLock fileLock(outFile + ".loc");
			if (!fileLock.lock())
			{
				std::cerr << "PathAnalysis::save() can't lock file " << outFile << ".loc" << std::endl;
				continue;
			}
			noClusters = false;
			clusterFile.save(outFile);
		}
		if (noClusters)
		{
			// save empty file
			std::string outFile = getOutFileName(0);
			utils::FileLock fileLock(outFile + ".loc");
			sequence::Cluster clusterFile;
			if (!fileLock.lock())
			{
				std::cerr << "PathAnalysis::save() can't lock file " << outFile << ".loc" << std::endl;				
			}
			else
				clusterFile.save(outFile);
		}
		m_PathClustering->resetFinishedClusters();
	}

}