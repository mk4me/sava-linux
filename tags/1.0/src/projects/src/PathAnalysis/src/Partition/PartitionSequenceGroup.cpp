#include "PartitionSequenceGroup.h"
#include "PartitionMath.h"

#include "config/PartitionAnalyzer.h"

#include <opencv2/core/operations.hpp>

namespace clustering
{	
	namespace partition
	{	
		int SequenceGroup::s_MaxDistance = 40;
		float SequenceGroup::s_SpeedDiff = 0.6f;
		int SequenceGroup::s_AngleDiff = 20;
		int SequenceGroup::s_MinMovement = 50;
		int SequenceGroup::s_CoverLevel = 50;
		int SequenceGroup::s_MinTime = 10;
		int SequenceGroup::s_MinSize = 30;
		int SequenceGroup::s_MaxSize = 350;
		int SequenceGroup::s_ClusterMargin = 5;
		int SequenceGroup::s_ContainLevel = 90;
		
		SequenceGroup::SequenceGroup(const std::vector<FramePtr>& frames)
		{
			std::cout << "Creating sequences.." << std::endl;
			
			std::map<int, Sequence> seqences;
			for (auto& frame : frames)
			{
				for (auto& cluster : frame->getClusters())
					seqences[cluster->getId()].addCluster(frame->getTime(), cluster);
			}
		
			for (auto it = seqences.begin(); it != seqences.end(); ++it)
			{
				if (it->second.getLength() < s_MinTime)
					continue;
		
				it->second.setId(it->first);
				m_Seqences.push_back(std::make_shared<Sequence>(it->second));
			}
		}
		
		void SequenceGroup::filter()
		{
			std::cout << "Filtering sequences.. ";
		
			int removed = 0;
			for (auto it = m_Seqences.begin(); it != m_Seqences.end();)
			{
				cv::Point size = (*it)->getSize();
				if ((*it)->getNumFrames() < s_MinTime)
				{
					it = m_Seqences.erase(it);
					++removed;
				}
				else if (size.x < s_MinSize || size.y < s_MinSize)
				{
					it = m_Seqences.erase(it);
					++removed;
				}
				else if (size.x > s_MaxSize || size.y > s_MaxSize) 
				{
					it = m_Seqences.erase(it);
					++removed;
				}
				else
					++it;
			}
		
			std::cout << "Removed: " << removed << std::endl;
		}
		
		void SequenceGroup::computeSpeeds()
		{
			for (auto& sequence : m_Seqences)
				sequence->computeSpeed();
		}
		
		void SequenceGroup::mergeSequences()
		{
			std::cout << "Merging sequences.. ";
		
			if (m_Seqences.size() < 2)
				return;
			
			int merged = 0;
			
			std::vector<int> labels(m_Seqences.size());
			int numClusters = cv::partition(m_Seqences, labels, &isCongruent);
			
			if (m_Seqences.size() != numClusters)
			{
				std::vector<SequencePtr> sequences(numClusters);
				for (int i = 0; i < m_Seqences.size(); ++i)
				{
					int label = labels[i];
					if (!sequences[label])
						sequences[label] = m_Seqences[i];
					else
					{
						sequences[label]->mergeWith(*m_Seqences[i]);
						sequences[label]->computeSpeed();
					}
				}
		
				std::cout << "Merged: " << m_Seqences.size() - sequences.size() << std::endl;
				m_Seqences = sequences;
			}
			else
				std::cout << "Merged: 0" << std::endl;
		}
		
		void SequenceGroup::stabilizeSequences()
		{
			std::cout << "Stabilizing sequences.. " << std::endl;
		
			for (auto& sequence : m_Seqences)
				sequence->stabilize(s_ClusterMargin);
		}
		
		void SequenceGroup::drawPaths(const cv::Mat& frame, Palette& palette, bool coords) const
		{
			for (const auto& sequence : m_Seqences)
				sequence->drawPath(frame, palette, coords);
		}
		
		void SequenceGroup::drawPoints(const cv::Mat& frame, Palette& palette, Time time) const
		{
			for (const auto& sequence : m_Seqences)
				sequence->drawPoints(frame, palette, time);
		}
		
		ClusterVector SequenceGroup::getFrame(Time time) const
		{
			ClusterVector clusters;
			for (const auto& sequence : m_Seqences)
			{
				ClusterPtr cluster = sequence->getCluster(time);
				if (cluster)
					clusters.push_back(cluster);
			}
		
			return clusters;
		}
		
		std::vector<std::shared_ptr<sequence::Cluster>> SequenceGroup::save() const
		{
			std::vector<std::shared_ptr<sequence::Cluster>> clusters;
			if (m_Seqences.empty())
			{
				clusters.push_back(std::make_shared<sequence::Cluster>());
				return clusters;
			}

			for (auto& sequence : m_Seqences)
				clusters.push_back(sequence->save());
			
			return clusters;
		}

		void SequenceGroup::loadParameters()
		{
			config::PartitionAnalyzer& config = config::PartitionAnalyzer::getInstance();

			s_MaxDistance = config.getSequenceMaxDistance();
			s_SpeedDiff = config.getSequenceSpeedDiff();
			s_AngleDiff = config.getSequenceAngleDiff();
			s_MinMovement = config.getSequenceMinMovement();
			s_CoverLevel = config.getSequenceCoverLevel();
			s_MinTime = config.getSequenceMinTime();
			s_MinSize = config.getSequenceMinSize();
			s_MaxSize = config.getSequenceMaxSize();
			s_ClusterMargin = config.getClusterMarign();
			s_ContainLevel = config.getSequenceContainLevel();
		}

		bool SequenceGroup::isCongruent(const SequencePtr& s1, const SequencePtr& s2)
		{
			float coverLevel = s1->getCoverLevel(*s2);
			if (coverLevel > s_ContainLevel)
				return true;
			
			bool moving1 = s1->getMovement() > s_MinMovement;
			bool moving2 = s2->getMovement() > s_MinMovement;
		
			if (moving1 && moving2)
			{
				if (Math::getAngle(s1->getAngle(), s2->getAngle()) > s_AngleDiff)
					return false;
		
				float sDiff = abs(s1->getPower() - s2->getPower());
				if (sDiff > s_SpeedDiff)
					return false;
		
				if (isLinear(s1, s2))
					return true;
			}
			
			if (moving1 == moving2)
				return coverLevel > s_CoverLevel;
		
			return false;
		}
		
		bool SequenceGroup::isLinear(const SequencePtr& s1, const SequencePtr& s2)
		{
			const cv::Point2f& p1 = s1->getVelocity();
			Math::Vector v1((int)p1.x, (int)p1.y, (int)s1->getLength());
			const cv::Point2f& p2 = s2->getVelocity();
			Math::Vector v2((int)p2.x, (int)p2.y, (int)s2->getLength());
		
			if (Math::length(v1) > Math::length(v2))
				return s1->getDistance(*s2) < s_MaxDistance;
			else
				return s2->getDistance(*s1) < s_MaxDistance;
		}
		
		bool SequenceGroup::isCovered(const SequencePtr& s1, const SequencePtr& s2)
		{
			return s1->getCoverLevel(*s2) > s_CoverLevel;
		}
	}
}
