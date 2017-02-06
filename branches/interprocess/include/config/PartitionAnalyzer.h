#pragma once
#ifndef PartitionAnalyzer_h__
#define PartitionAnalyzer_h__

namespace boost
{
	namespace serialization
	{
		class access;
	}
}

namespace config
{
	class PartitionAnalyzer
	{
		friend class boost::serialization::access;

	public:
		static PartitionAnalyzer& getInstance();

		bool load();
		bool save() const;

		int getPointMaxDistance() const { return m_PointMaxDistance; }
		void setPointMaxDistance(int val) { m_PointMaxDistance = val; }

		int getPathMinMovement() const { return m_PathMinMovement; }
		void setPathMinMovement(int val) { m_PathMinMovement = val; }

		float getPathSpeedDiff() const { return m_PathSpeedDiff; }
		void setPathSpeedDiff(float val) { m_PathSpeedDiff = val; }

		int getPathAngleDiff() const { return m_PathAngleDiff; }
		void setPathAngleDiff(int val) { m_PathAngleDiff = val; }

		int getClusterMinPoints() const { return m_ClusterMinPoints; }
		void setClusterMinPoints(int val) { m_ClusterMinPoints = val; }

		int getClusterMarign() const { return m_ClusterMarign; }
		void setClusterMarign(int val) { m_ClusterMarign = val; }

		int getSequenceMaxDistance() const { return m_SequenceMaxDistance; }
		void setSequenceMaxDistance(int val) { m_SequenceMaxDistance = val; }

		float getSequenceSpeedDiff() const { return m_SequenceSpeedDiff; }
		void setSequenceSpeedDiff(float val) { m_SequenceSpeedDiff = val; }

		int getSequenceAngleDiff() const { return m_SequenceAngleDiff; }
		void setSequenceAngleDiff(int val) { m_SequenceAngleDiff = val; }

		int getSequenceMinMovement() const { return m_SequenceMinMovement; }
		void setSequenceMinMovement(int val) { m_SequenceMinMovement = val; }

		int getSequenceCoverLevel() const { return m_SequenceCoverLevel; }
		void setSequenceCoverLevel(int val) { m_SequenceCoverLevel = val; }

		int getSequenceContainLevel() const { return m_SequenceContainLevel; }
		void setSequenceContainLevel(int val) { m_SequenceContainLevel = val; }

		int getSequenceMinTime() const { return m_SequenceMinTime; }
		void setSequenceMinTime(int val) { m_SequenceMinTime = val; }

		int getSequenceMinSize() const { return m_SequenceMinSize; }
		void setSequenceMinSize(int val) { m_SequenceMinSize = val; }

		int getSequenceMaxSize() const { return m_SequenceMaxSize; }
		void setSequenceMaxSize(int val) { m_SequenceMaxSize = val; }

	private:
		int m_PointMaxDistance;

		int m_PathMinMovement;
		float m_PathSpeedDiff;
		int m_PathAngleDiff;

		int m_ClusterMinPoints;
		int m_ClusterMarign;
		
		int m_SequenceMaxDistance;
		float m_SequenceSpeedDiff;
		int m_SequenceAngleDiff;
		int m_SequenceMinMovement;
		int m_SequenceCoverLevel;
		int m_SequenceContainLevel;
		int m_SequenceMinTime;
		int m_SequenceMinSize;
		int m_SequenceMaxSize;

		PartitionAnalyzer();
		~PartitionAnalyzer() { }

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // PartitionAnalyzer_h__
