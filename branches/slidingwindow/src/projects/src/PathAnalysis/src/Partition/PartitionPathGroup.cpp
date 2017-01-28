#include "PartitionPathGroup.h"

namespace clustering
{
	namespace partition
	{
		PathGroup::PathGroup(const PathsMap& paths, int minMovement)
			: m_MinMovement(minMovement)
			, m_StartTime(999999)
			, m_EndTime(0)
		{
			for (auto it : paths)
			{
				if (!isGood(it.second))
					continue;
		
				m_Paths.push_back(new Path(it.second, (int)it.first));
				m_StartTime = std::min(it.second.points.begin()->first, m_StartTime);
				m_EndTime = std::max(it.second.points.rbegin()->first, m_EndTime);
			}
		}
		
		PathGroup::~PathGroup()
		{
			for (auto path : m_Paths)
				delete path;
		}
		
		FramePoints PathGroup::getFramePoints(Time frame) const
		{
			FramePoints points;
			for (auto path : m_Paths)
			{
				Path::Point* point = path->getPoint(frame);
				if (point != nullptr)
					points.push_back(point);
			}
		
			return points;
		}
		
		PathClusters PathGroup::getPathClusters() const
		{
			PathClusters pc;
			for (const auto& path : m_Paths)
			{
				if (path->getClusterId() >= 0)
					pc.insert(std::make_pair(path->getId(), path->getClusterId()));
			}
		
			return pc;
		}
		
		void PathGroup::calculateIds()
		{
			for (auto path : m_Paths)
				path->calcId();
		}
		
		bool PathGroup::isGood(const sequence::PathStream::Path& path) const
		{
			cv::Point minPoint = cv::Point(2000, 2000);
			cv::Point maxPoint = cv::Point(-1, -1);
		
			auto& points = path.points;
			for (auto point : points)
			{
				if (point.second.x < 0)
					break;
		
				minPoint.x = std::min(minPoint.x, point.second.x);
				minPoint.y = std::min(minPoint.y, point.second.y);
				maxPoint.x = std::max(maxPoint.x, point.second.x);
				maxPoint.y = std::max(maxPoint.y, point.second.y);
			}
		
			if (maxPoint.x < 0)
				return false;
		
			if ((maxPoint.x - minPoint.x) + (maxPoint.y - minPoint.y) < m_MinMovement)
				return false;
		
			return true;
		}
		
		void PathGroup::drawAll(const cv::Mat& frame, Palette& palette) const
		{
			for (auto path : m_Paths)
				path->draw(frame, palette, 1, false);
		}
	}
}