#include "PartitionFrame.h"
#include "PartitionMath.h"

#include "config/PartitionAnalyzer.h"

namespace clustering
{
	namespace partition
	{	
		int Frame::s_MaxDistance = 40;
		float Frame::s_SpeedDiff = 0.4f;
		int Frame::s_AngleDiff = 20;
		int Frame::s_ClusterMinPoints = 10;
		
		void Frame::getPaths()
		{
			m_FramePoints = m_PathsGroup->getFramePoints(m_FrameNr);
		}
		
		bool Frame::detectClusters()
		{
			if (m_FramePoints.size() < 2)
				return false;
		
			std::vector<int> pointLabels(m_FramePoints.size());
			int numClusters = cv::partition(m_FramePoints, pointLabels, &isCongruent);
		
			for (int i = 0; i < numClusters; ++i)
				m_Clusters.push_back(std::make_shared<Cluster>());
			for (int i = 0; i < m_FramePoints.size(); ++i)
			{
				int label = pointLabels[i];
				m_Clusters[label]->addPoint(m_FramePoints[i]);
			}
		
			return true;
		}
		
		void Frame::filterClusters()
		{
			//std::cout << "Filtering clusters.." << std::endl;
		
			for (auto it = m_Clusters.begin(); it != m_Clusters.end();)
			{
				if ((*it)->getNumPoints() < s_ClusterMinPoints)
					it = m_Clusters.erase(it);
				else
					++it;
			}
		}
		
		void Frame::reboundClusters()
		{
		//	std::cout << "Bounding clusters.." << std::endl;
		
			for (auto it = m_Clusters.begin(); it != m_Clusters.end(); ++it)
				(*it)->calculateBounds();
		}
		
		void Frame::assignClusterId()
		{
			for (auto& cluster : m_Clusters)
				cluster->generateId();
		
			for (auto point : m_FramePoints)
			{
				if (point->getPath()->getClusterId() < 0)
					point->getPath()->setClusterId(point->getClusterId());
			}
		}
		
		void Frame::copyClusterId(const PathClusters& lastPathClusters)
		{
		//	std::cout << "Assigning ids.." << std::endl;
		
			for (auto& cluster : m_Clusters)
				cluster->generateId(lastPathClusters);
		
			for (auto it = m_Clusters.begin(); it != m_Clusters.end(); ++it)
			{
				ClusterVector copies;
				std::copy_if(it, m_Clusters.end(), std::back_inserter(copies), [&](ClusterPtr& c) { return c->getId() == (*it)->getId(); });
				if (copies.size() < 2)
					continue;
		
				auto max = copies.begin();
				for (auto c = copies.begin() + 1; c != copies.end(); ++c)
				{
					if ((*c)->getNumPoints() > (*max)->getNumPoints())
						max = c;
				}
				for (auto c = copies.begin(); c != copies.end(); ++c)
				{
					if (c != max)
						(*c)->createId();
				}
			}
		
			for (auto point : m_FramePoints)
			{
				if (point->getPath()->getClusterId() < 0)
					point->getPath()->setClusterId(point->getClusterId());
			}
		}
		
		void Frame::loadParameters()
		{
			config::PartitionAnalyzer& config = config::PartitionAnalyzer::getInstance();

			s_MaxDistance = config.getPointMaxDistance();
			s_SpeedDiff = config.getPathSpeedDiff();
			s_AngleDiff = config.getPathAngleDiff();
			s_ClusterMinPoints = config.getClusterMinPoints();
		}

		bool Frame::isCongruent(const Path::Point* p1, const Path::Point* p2)
		{
			if (Math::getAngle(p1->getPath()->getAngle(), p2->getPath()->getAngle()) > s_AngleDiff)
				return false;
		
			float speedDiff = abs(p1->getPath()->getPower() - p2->getPath()->getPower());
			if (speedDiff > s_SpeedDiff)
				return false;
		
			return cv::norm(p2->getPosition() - p1->getPosition()) < s_MaxDistance;
		}
	}
}