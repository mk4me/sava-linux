#include "PartitionSequence.h"
#include "PartitionMath.h"

#include <opencv2/imgproc.hpp>

#include <QtCore/qmath.h>

#include <sstream>
#include <iomanip>

namespace clustering
{	
	namespace partition
	{
		void Sequence::Position::addPosition(const cv::Point& position)
		{
			m_PositionSum += position;
			++m_Count;
		}
		
		void Sequence::addCluster(Time time, const ClusterPtr& cluster)
		{
			auto element = m_Clusters.insert(std::make_pair(time, cluster));
			if (!element.second)
				element.first->second->takePaths(*cluster);
		}
		
		ClusterPtr Sequence::getCluster(Time time) const
		{
			auto it = m_Clusters.find(time);
			if (it != m_Clusters.end())
				return it->second;
		
			return ClusterPtr();
		}
		
		void Sequence::drawPath(const cv::Mat& frame, Palette& palette, bool coords) const
		{
			const auto& color = palette[m_Clusters.begin()->second->getId()];
		
			auto end = m_Clusters.begin();
			auto start = end++;
			for (; end != m_Clusters.end(); start = end++)
				cv::line(frame, start->second->getPosition(), end->second->getPosition(), color, 2);
		
			if (coords)
			{
				std::ostringstream ofs;
				ofs << std::fixed << std::setprecision(2) << "a" << m_Angle << " m" << m_Movement;
				cv::putText(frame, ofs.str(), m_Clusters.begin()->second->getPosition() + cv::Point(0, 10), cv::FONT_HERSHEY_COMPLEX, 0.5, color, 1);
			}
		}
		
		void Sequence::drawPoints(const cv::Mat& frame, Palette& palette, Time time) const
		{
			auto it = m_Clusters.find(time);
			if (it == m_Clusters.end())
				return;
		
			for (auto pathPos : m_PathPositions)
			{
				bool bold = std::find_if(it->second->m_Points.begin(), it->second->m_Points.end(), [&](Path::Point* point){ return point->getPath()->getId() == pathPos.first; }) != it->second->m_Points.end();
				cv::Point pos = pathPos.second.getPosition() - m_PathRect.tl() + it->second->getTopLeft();
		
				cv::circle(frame, pos, 1, palette[it->second->getId()], bold ? 2 : 1);
			}
		}
		
		void Sequence::computeSpeed()
		{
			if (m_Clusters.size() < 2)
				return;
		
			m_Velocity = m_Clusters.rbegin()->second->getPosition() - m_Clusters.begin()->second->getPosition();
			m_Movement = cv::norm(m_Velocity);
			m_Velocity /= (float)(m_Clusters.rbegin()->first - m_Clusters.begin()->first);
		
			m_Power = cv::norm(m_Velocity);
		
			cv::Point2f nv = m_Velocity / m_Power;
			m_Angle = (std::atan2(nv.y, nv.x) / M_PI) * 180.0f;
		}
		
		void Sequence::stabilize(int margin)
		{
			computePathPositions();
			computeSize();
			correctClusters(margin);
		}
		
		float Sequence::getDistance(const Sequence& other) const
		{
			cv::Point l1 = m_Clusters.begin()->second->getPosition();
			Math::Vector line1(l1.x, l1.y, m_Clusters.begin()->first);
		
			cv::Point l2 = m_Clusters.rbegin()->second->getPosition();
			Math::Vector line2(l2.x, l2.y, m_Clusters.rbegin()->first);
		
			cv::Point p1 = other.m_Clusters.begin()->second->getPosition();
			Math::Vector point1(p1.x, p1.y, other.m_Clusters.begin()->first);
		
			cv::Point p2 = other.m_Clusters.rbegin()->second->getPosition();
			Math::Vector point2(p2.x, p2.y, other.m_Clusters.rbegin()->first);
		
			float dst = Math::distance(line1, line2, point1);
			dst += Math::distance(line1, line2, point2);
			return dst / 2.0f;
		}
		
		void Sequence::computeSize()
		{
			m_Size = cv::Point(0, 0);
		
			for (const auto& cluster : m_Clusters)
			{
				m_Size.x = std::max(m_Size.x, cluster.second->getBottomRight().x - cluster.second->getTopLeft().x);
				m_Size.y = std::max(m_Size.y, cluster.second->getBottomRight().y - cluster.second->getTopLeft().y);
			}
		
			cv::Point topLeft(2000, 2000);
			cv::Point bottomRight(0, 0);
			for (const auto path : m_PathPositions)
			{
				cv::Point pathPos = path.second.getPosition();
				topLeft.x = std::min(topLeft.x, pathPos.x);
				topLeft.y = std::min(topLeft.y, pathPos.y);
				bottomRight.x = std::max(bottomRight.x, pathPos.x);
				bottomRight.y = std::max(bottomRight.y, pathPos.y);
			}
			m_PathRect = cv::Rect(topLeft, bottomRight);
		
			m_Size.x = std::max(m_Size.x, bottomRight.x - topLeft.x);
			m_Size.y = std::max(m_Size.y, bottomRight.y - topLeft.y);
		}
		
		void Sequence::correctClusters(int margin)
		{
			for (auto& cluster : m_Clusters)
			{
				// compute paths position
				cv::Point pathCenter(0, 0);
				cv::Point screenCenter(0, 0);
				for (auto point : cluster.second->m_Points)
				{
					cv::Point pathPos = m_PathPositions[point->getPath()->getId()].getPosition();
					pathCenter += pathPos;
					screenCenter += point->getPosition();
				}
				pathCenter /= (int)cluster.second->m_Points.size();
				screenCenter /= (int)cluster.second->m_Points.size();
		
				// compute cluster position
				cv::Point2f movementFactor = cv::Point2f(pathCenter - m_PathRect.tl());
				movementFactor.x /= m_PathRect.size().width;
				movementFactor.y /= m_PathRect.size().height;
				cv::Point correction(m_Size.x * movementFactor.x, m_Size.y * movementFactor.y);
				cv::Point clusterPos = screenCenter - correction;
				
				// move to original
				if (clusterPos.x > cluster.second->m_TopLeft.x)
					clusterPos.x = cluster.second->m_TopLeft.x;
				if (clusterPos.y > cluster.second->m_TopLeft.y)
					clusterPos.y = cluster.second->m_TopLeft.y;
				if (clusterPos.x + m_Size.x < cluster.second->m_BottomRight.x)
					clusterPos.x += cluster.second->m_BottomRight.x - (clusterPos.x + m_Size.x);
				if (clusterPos.y + m_Size.y < cluster.second->m_BottomRight.y)
					clusterPos.y += cluster.second->m_BottomRight.y - (clusterPos.y + m_Size.y);
		
				// apply new position
				cv::Point marginSize(margin, margin);
				cluster.second->m_TopLeft = clusterPos - marginSize;
				cluster.second->m_BottomRight = clusterPos + m_Size + marginSize;
			}
		}
		
		void Sequence::mergeWith(const Sequence& other)
		{
			for (auto cluster : other.m_Clusters)
			{
				auto it = m_Clusters.find(cluster.first);
				if (it != m_Clusters.end())
					it->second->takePaths(*cluster.second);
				else
				{
					m_Clusters.insert(cluster);
					cluster.second->m_Id = m_Id;
				}
			}
		
			computeSpeed();
		}
		
		float Sequence::getCoverLevel(const Sequence& other) const
		{
			if (other.m_Clusters.begin()->first > m_Clusters.rbegin()->first)
				return 0.0f;
			if (other.m_Clusters.rbegin()->first < m_Clusters.begin()->first)
				return 0.0f;
		
			int compared = 0;
			float coverageSum = 0.0f;
		
			for (auto& cluster : other.m_Clusters)
			{
				auto it = m_Clusters.find(cluster.first);
				if (it == m_Clusters.end())
					continue;
		
				coverageSum += cluster.second->getCoverLevel(*it->second);
				++compared;
			}
		
			return coverageSum / compared;
		}
		
		std::shared_ptr<sequence::Cluster> Sequence::save() const
		{
			sequence::Cluster::FramesPositionsMap positions;
			for (const auto& cluster : m_Clusters)
				positions.insert(std::make_pair((size_t)cluster.first, cluster.second->getTopLeft()));
			
			std::shared_ptr<sequence::Cluster> clu = std::make_shared<sequence::Cluster>(m_Id);
			clu->setFramesPositions(positions);

			//TODO: correct m_Size;
			ClusterPtr cluster = m_Clusters.begin()->second;
			clu->setFrameSize(cluster->getBottomRight() - cluster->getTopLeft());

			return clu;
		}

		void Sequence::computePathPositions()
		{
			m_PathPositions.clear();
		
			for (auto& cluster : m_Clusters)
			{
				std::vector<PointInfo> points;
				for (auto point : cluster.second->m_Points)
				{
					points.push_back(point);
					auto pathPos = m_PathPositions.find(point->getPath()->getId());
					points.back().isNew = pathPos == m_PathPositions.end();
				}
		
				points[0].position = cv::Point(0, 0);
				cv::Point basePos = points[0].point->getPosition();
				cv::Point avgDistance;
				int numOldPoints = 0;
				for (int i = 1; i < points.size(); ++i)
				{
					points[i].position = points[i].point->getPosition() - basePos;
					if (points[i].isNew)
						continue;
		
					avgDistance += points[i].position - m_PathPositions[points[i].point->getPath()->getId()].getPosition();
					numOldPoints++;
				}
		
				if (numOldPoints > 0)
				{
					avgDistance /= numOldPoints;
					for (auto& point : points)
						point.position -= avgDistance;
				}
		
				for (auto& point : points)
				{
					Path::Point* p = point.point;
					Path* path = p->getPath();
					int pathId = path->getId();
		
					m_PathPositions[pathId].addPosition(point.position);
				}
			}
		}
	}
}
