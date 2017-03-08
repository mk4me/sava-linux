#pragma once
#ifndef PartitionPath_h__
#define PartitionPath_h__

#include "PartitionDefinitions.h"

#include "sequence/PathStream.h"

#include <map>

namespace clustering
{
	namespace partition
	{
		class Cluster;
		
		class Path
		{
		public:
			class Point
			{
			public:
				Point() : m_Cluster(nullptr) { }
				Point(const cv::Point& position, Path* path) : m_Position(position), m_Cluster(nullptr), m_Path(path) { }
		
				const cv::Point& getPosition() const { return m_Position; }
				Path* getPath() const { return m_Path; }
		
				Cluster* getCluster() const { return m_Cluster; }
				void setCluster(Cluster* cluster) { m_Cluster = cluster; }

				Id getClusterId() const;
		
			private:
				cv::Point m_Position;
				Path* m_Path;
				Cluster* m_Cluster;
			};
			
			Path(const sequence::PathStream::Path& path, Id id);
			~Path() { }
		
			// no copy and assign
			Path(const Path&) = delete;
			Path& operator=(const Path&) = delete;
		
			size_t getLength() const { return m_Points.size(); }
			Id getId() const { return m_Id; }
		
			short getAngle() const { return m_Angle; }
			float getPower() const { return m_Power; }
			const cv::Point2f& getVelocity() const { return m_Velocity; }
		
			Point* getPoint(Time frame);
		
			Id getClusterId() const { return m_ClusterId; }
			void setClusterId(Id id) { m_ClusterId = id; }
			void calcId();
		
			void draw(const cv::Mat& frame, Palette& palette, int thickness, bool colored) const;
		
		private:
			short m_Angle;
			float m_Power;
			cv::Point2f m_Velocity;
		
			std::map<Time, Point> m_Points;
		
			Id m_Id;
			Id m_ClusterId;
		
			void computeSpeed();
		};
	}
}

#endif // PartitionPath_h__
