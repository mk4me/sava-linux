#pragma once
#ifndef PartitionPathGroup_h__
#define PartitionPathGroup_h__

#include "PartitionPath.h"

namespace clustering
{	
	namespace partition
	{
		typedef std::map<sequence::PathStream::Id, sequence::PathStream::Path> PathsMap;
		typedef std::vector<Path::Point*> FramePoints;

		class PathGroup
		{
		public:
			PathGroup(const PathsMap& paths, int minMovement);
			PathGroup() { }
			~PathGroup();
		
			// no copy and assign
			PathGroup(const PathGroup&) = delete;
			PathGroup& operator=(const PathGroup&) = delete;
		
			FramePoints getFramePoints(Time frame) const;
		
			PathClusters getPathClusters() const;
		
			Time getStartTime() const { return m_StartTime; }
			Time getEndTime() const { return m_EndTime; }
		
			void calculateIds();
		
			void drawAll(const cv::Mat& frame, Palette& palette) const;
		
		private:
			std::vector<Path*> m_Paths;
		
			Time m_StartTime;
			Time m_EndTime;
		
			int m_MinMovement;
		
			bool isGood(const sequence::PathStream::Path& path) const;
		};
	}
}

#endif // PartitionPathGroup_h__
