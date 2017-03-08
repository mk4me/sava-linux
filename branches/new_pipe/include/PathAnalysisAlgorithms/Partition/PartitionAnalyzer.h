#pragma once
#ifndef PartitionPathAnalyzer_h__
#define PartitionPathAnalyzer_h__

#include "PathAnalysisAlgorithms/PathAnalyzer.h"

#include "sequence/PathStream.h"
#include "utils/RandomColor.h"

#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"

#include "PartitionCluster.h"
#include "PartitionPathGroup.h"
#include "PartitionSequenceGroup.h"
#include "PartitionFrame.h"

namespace clustering
{
	namespace partition
	{
		class Analyzer : public PathAnalyzer
		{
		public:
			Analyzer();

			static void registerParameters(ProgramOptions& programOptions);
			virtual bool loadParameters(const ProgramOptions& options) override;
			virtual void process() override;

		private:
			// per sequence fields
			std::shared_ptr<PathGroup> m_PathsGroup;
			std::vector<FramePtr> m_Frames;
			std::shared_ptr<SequenceGroup> m_Sequences;
			PathClusters m_LastPathClusters;

			// per frame fields
			cv::Mat m_Frame;
			Time m_FrameNr;

			mutable cv::VideoWriter m_videoWriter;

			mutable std::map<unsigned long long, utils::RandomColor> m_ColorMap;

			void loadPaths();
			void createFrames();
			void detectClusters();
			void createSequences();
			void saveSequences();

			void correctIds();

			void createWindow();
			void visualize();
			void drawBounds(bool showPaths, bool showCoords, bool showPoints) const;
			void drawFrame() const;
		};
	}
}

#endif // PartitionPathAnalyzer_h__