#pragma once
#ifndef PartitionCluster_h__
#define PartitionCluster_h__

#include "PartitionPath.h"

#include "opencv2/core.hpp"

namespace clustering
{
	namespace partition
	{
		class Sequence;
		
		class Cluster
		{
		public:
			Cluster();
			~Cluster() { }
		
			Id getId() const { return m_Id; }
			void generateId(bool createNew = true);
			void generateId(const PathClusters& pathClusters);
			void createId();
		
			void addPoint(Path::Point* point);
			size_t getNumPoints() const { return m_Points.size(); }
		
			void calculateBounds();
		
			const cv::Point& getTopLeft() const { return m_TopLeft; }
			const cv::Point& getBottomRight() const { return m_BottomRight; }
			cv::Point getPosition() const;
		
			void drawPaths(const cv::Mat& frame, Palette& palette) const;
		
			void takePaths(const Cluster& other);
		
			float getCoverLevel(const Cluster& other) const;
		
		private:
			typedef std::vector<Path::Point*> Points;

			static Id s_NextId;
		
			Id m_Id;
			cv::Point m_TopLeft;
			cv::Point m_BottomRight;
			
			Points m_Points;
		
			friend class Sequence;
		};

		typedef std::shared_ptr<Cluster> ClusterPtr;
		typedef std::vector<ClusterPtr> ClusterVector;
	}
}

#endif // PartitionCluster_h__
