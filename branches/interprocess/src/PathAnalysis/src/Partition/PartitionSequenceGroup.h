#pragma once
#ifndef PartitionClusterGroup_h__
#define PartitionClusterGroup_h__

#include "PartitionSequence.h"
#include "PartitionFrame.h"

namespace clustering
{	
	namespace partition
	{
		class SequenceGroup
		{
		public:
			SequenceGroup() { }
			SequenceGroup(const std::vector<FramePtr>& frames);
			~SequenceGroup() { }
		
			void filter();
			void computeSpeeds();
			void mergeSequences();
			void stabilizeSequences();
		
			void drawPaths(const cv::Mat& frame, Palette& palette, bool coords) const;
			void drawPoints(const cv::Mat& frame, Palette& palette, Time time) const;
		
			ClusterVector getFrame(Time time) const;

			std::vector<std::shared_ptr<sequence::Cluster>> save() const;

			static void loadParameters();
		
		private:
			static int s_MaxDistance;
			static float s_SpeedDiff;
			static int s_AngleDiff;
			static int s_MinMovement;
			static int s_CoverLevel;
			static int s_MinTime;
			static int s_MinSize;
			static int s_MaxSize;
			static int s_ClusterMargin;
			static int s_ContainLevel;

			std::vector<SequencePtr> m_Seqences;
		
			static bool isCongruent(const SequencePtr& s1, const SequencePtr& s2);
			static bool isLinear(const SequencePtr& s1, const SequencePtr& s2);
			static bool isCovered(const SequencePtr& s1, const SequencePtr& s2);
		};
	}
	
	#endif // PartitionClusterGroup_h__
}
