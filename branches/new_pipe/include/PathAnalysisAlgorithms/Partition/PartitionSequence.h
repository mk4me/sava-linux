#pragma once
#ifndef PartitionSequence_h__
#define PartitionSequence_h__

#include "PartitionCluster.h"

#include "sequence/Cluster.h"

namespace clustering
{	
	namespace partition
	{
		class Sequence
		{
		public:
			class Position
			{
			public:
				Position() : m_Count(0) { }
				~Position() { }
		
				void addPosition(const cv::Point& position);
				cv::Point getPosition() const { return m_Count > 0 ? m_PositionSum / m_Count : m_PositionSum; }
		
			private:
				cv::Point m_PositionSum;
				int m_Count;
			};
		
			Sequence() { }
			~Sequence() { }
		
			void addCluster(Time time, const ClusterPtr& cluster);
			ClusterPtr getCluster(Time time) const;
		
			Id getId() const { return m_Id; }
			void setId(Id id) { m_Id = id; }
		
			Time getLength() const { return m_Clusters.size() > 0 ? m_Clusters.rbegin()->first - m_Clusters.begin()->first : 0; }
			size_t getNumFrames() const { return m_Clusters.size(); }
		
			void drawPath(const cv::Mat& frame, Palette& palette, bool coords) const;
			void drawPoints(const cv::Mat& frame, Palette& palette, Time time) const;
		
			void computeSpeed();
			void stabilize(int margin);
		
			float getDistance(const Sequence& other) const;
		
			float getAngle() const { return m_Angle; }
			float getPower() const { return m_Power; }
			float getMovement() const { return m_Movement; }
			cv::Point2f getVelocity() const { return m_Velocity; }
			cv::Point getSize() const { return m_Size; }
		
			void mergeWith(const Sequence& other);
			float getCoverLevel(const Sequence& other) const;

			std::shared_ptr<sequence::Cluster> save() const;
		
		private:
			struct PointInfo
			{
				PointInfo(Path::Point* point) : point(point) { }
				PointInfo() { }
		
				Path::Point* point = nullptr;
				cv::Point position;
				bool isNew;
			};
		
			std::map<Time, ClusterPtr> m_Clusters;
			std::map<Id, Position> m_PathPositions;
		
			Id m_Id;
		
			float m_Angle;
			float m_Power;
			float m_Movement;
			cv::Point2f m_Velocity;
			cv::Point m_Size;
			cv::Rect m_PathRect;
		
			void computePathPositions();
			void computeSize();
			void correctClusters(int margin);
		};

		typedef std::shared_ptr<Sequence> SequencePtr;
	}
}

#endif // PartitionSequence_h__
