#pragma once
#ifndef PartitionFrame_h__
#define PartitionFrame_h__

#include "PartitionPathGroup.h"
#include "PartitionCluster.h"

namespace clustering
{
	namespace partition
	{	
		class Frame
		{
		public:
			Frame() { }
			Frame(PathGroup* pathsGroup, Time frameNr) : m_PathsGroup(pathsGroup), m_FrameNr(frameNr) { }
		
			// no copy and assign
			Frame(const Frame&) = delete;
			Frame& operator=(const Frame&) = delete;
		
			void getPaths();
		
			bool detectClusters();
			void filterClusters();
			void reboundClusters();
		
			void assignClusterId();
			void copyClusterId(const PathClusters& lastPathClusters);
			
			Time getTime() const { return m_FrameNr; }
			const ClusterVector& getClusters() const { return m_Clusters; }
		
			static void loadParameters();

		private:
			static bool isCongruent(const Path::Point* p1, const Path::Point* p2);

			static int s_MaxDistance;
			static float s_SpeedDiff;
			static int s_AngleDiff;
			static int s_ClusterMinPoints;
		
			FramePoints m_FramePoints;
			Time m_FrameNr;
			ClusterVector m_Clusters;
			PathGroup* m_PathsGroup;
		};

		typedef std::shared_ptr<Frame> FramePtr;
	}
}

#endif // PartitionFrame_h__
