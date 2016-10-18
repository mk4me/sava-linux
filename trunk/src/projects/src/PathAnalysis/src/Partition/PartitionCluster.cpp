#include "PartitionCluster.h"

namespace clustering
{
	namespace partition
	{	
		Id Cluster::s_NextId = 0;
		
		Cluster::Cluster()
			: m_Id(-1)
			, m_TopLeft(2000, 2000)
			, m_BottomRight(-1, -1)
		{
		
		}
		
		void Cluster::generateId(bool createNew)
		{
			std::map<Id, int> pathIdCounts;
			for (auto point : m_Points)
			{
				auto it = pathIdCounts.find(point->getPath()->getClusterId());
				if (it == pathIdCounts.end())
					pathIdCounts.insert(std::make_pair(point->getPath()->getClusterId(), 1));
				else
					++it->second;
			}
		
			auto maxId = pathIdCounts.end();
			int maxCount = 0;
			for (auto it = pathIdCounts.begin(); it != pathIdCounts.end(); ++it)
			{
				if (it->second > maxCount && it->first >= 0)
				{
					maxId = it;
					maxCount = it->second;
				}
			}
		
			if (maxId != pathIdCounts.end())
				m_Id = maxId->first;
			else if (createNew)
				m_Id = s_NextId++;
		}
		
		void Cluster::generateId(const PathClusters& pathClusters)
		{
			std::map<Id, int> pathIdCounts;
			for (auto point : m_Points)
			{
				auto it = pathClusters.find(point->getPath()->getId());
				if (it == pathClusters.end())
					++pathIdCounts[-1];
				else
					++pathIdCounts[it->second];
			}
		
			auto maxId = pathIdCounts.end();
			int maxCount = 0;
			for (auto it = pathIdCounts.begin(); it != pathIdCounts.end(); ++it)
			{
				if (it->second > maxCount && it->first >= 0)
				{
					maxId = it;
					maxCount = it->second;
				}
			}
		
			if (maxId != pathIdCounts.end())
				m_Id = maxId->first;
			else
				m_Id = s_NextId++;
		}
		
		void Cluster::createId()
		{
			m_Id = s_NextId++;
		}
		
		void Cluster::addPoint(Path::Point* point)
		{
			m_Points.push_back(point);
			point->setCluster(this);
		}
		
		void Cluster::calculateBounds()
		{
			m_TopLeft = cv::Point(2000, 2000);
			m_BottomRight = cv::Point(-1, -1);
		
			for (auto point : m_Points)
			{
				const cv::Point& pos = point->getPosition();
		
				m_TopLeft.x = std::min(m_TopLeft.x, pos.x);
				m_TopLeft.y = std::min(m_TopLeft.y, pos.y);
				m_BottomRight.x = std::max(m_BottomRight.x, pos.x);
				m_BottomRight.y = std::max(m_BottomRight.y, pos.y);
			}
		}
		
		cv::Point Cluster::getPosition() const
		{
			return m_TopLeft + (m_BottomRight - m_TopLeft) / 2;
		}
		
		void Cluster::drawPaths(const cv::Mat& frame, Palette& palette) const
		{
			for (auto point : m_Points)
				point->getPath()->draw(frame, palette, 1, true);
		}
		
		void Cluster::takePaths(const Cluster& other)
		{
			for (auto point : other.m_Points)
			{
				m_Points.push_back(point);
				point->setCluster(this);
			}
			calculateBounds();
		}
		
		float Cluster::getCoverLevel(const Cluster& other) const
		{
			cv::Rect currentRect(m_TopLeft, m_BottomRight);
			cv::Rect otherRect(other.m_TopLeft, other.m_BottomRight);
		
			float intersectionArea = (float)(currentRect & otherRect).area();
			return std::max(intersectionArea / currentRect.area(), intersectionArea / otherRect.area()) * 100.0f;
		}
	}
}
