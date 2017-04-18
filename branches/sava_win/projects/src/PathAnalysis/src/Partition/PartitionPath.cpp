#include "PartitionPath.h"
#include "PartitionCluster.h"

#include <QtCore/qmath.h>

#include <opencv2/imgproc.hpp>

namespace clustering
{	
	namespace partition
	{
		
		Path::Path(const sequence::PathStream::Path& path, Id id)
			: m_ClusterId(-1)
		{
			/*m_Points.insert(
				path.points.begin(), 
				std::find_if(path.points.begin(), path.points.end(), [](const std::pair<sequence::PathStream::Time, cv::Point>& point){ return point.second.x < 0; })
			);*/
		
			for (auto point : path.points)
			{
				if (point.second.x < 0)
					break;
				m_Points.insert(std::make_pair(point.first, Point(point.second, this)));
			}
			m_Id = id;
		
			computeSpeed();
		}
		
		void Path::computeSpeed()
		{
			if (m_Points.size() < 2)
				return;
		
			m_Velocity = m_Points.rbegin()->second.getPosition() - m_Points.begin()->second.getPosition();
			m_Velocity /= (float)(m_Points.rbegin()->first - m_Points.begin()->first);
		
			m_Power = cv::norm(m_Velocity);
		
			cv::Point2f nv = m_Velocity / m_Power;
			m_Angle = (short)((std::atan2(nv.y, nv.x) / M_PI) * 180.0f + 180);
		}
		
		Path::Point* Path::getPoint(Time frame)
		{
			const auto it = m_Points.find(frame);
			if (it == m_Points.end())
				return nullptr;
			return &(it->second);
		}
		
		void Path::calcId()
		{
			std::map<Id, int> counts;
		
			for (const auto& point : m_Points)
			{
				int clusterId = point.second.getClusterId();
				auto it = counts.find(clusterId);
				if (it == counts.end())
					counts.insert(std::make_pair(clusterId, 1));
				else
					++it->second;
			}
		
			Id maxId = -1;
			int maxCount = -1;
			for (const auto& cluster : counts)
			{
				if (cluster.second > maxCount)
				{
					maxCount = cluster.second;
					maxId = cluster.first;
				}
			}
		
			if (maxId >= 0)
				m_ClusterId = maxId;
		}
		
		void Path::draw(const cv::Mat& frame, Palette& palette, int thickness, bool colored) const
		{
			static const cv::Scalar grey = CV_RGB(128, 128, 128);
			const auto& color = palette[m_ClusterId];
		
			auto end = m_Points.begin();
			auto start = end++;
			for (; end != m_Points.end(); start = end++)
				cv::line(frame, start->second.getPosition(), end->second.getPosition(), colored ? color : grey, thickness);
		}

		clustering::partition::Id Path::Point::getClusterId() const
		{
			return m_Cluster == nullptr ? -1 : m_Cluster->getId();
		}
	}
}
