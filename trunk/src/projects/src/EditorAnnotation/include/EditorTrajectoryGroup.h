#pragma once

#include "EditorTrajectory.h"
#include <set>

/**
* \class EditorTrajectoryGroup
*
* \brief Class to group trajectory.
*
*/


class EditorTrajectoryGroup
{
	struct TrajectoryComparator
	{
		bool operator()(const EditorTrajectoryPtr& t1, const EditorTrajectoryPtr& t2) {
			return t1->getPartName() < t2->getPartName();
		}
	};

public:
	EditorTrajectoryGroup();

	int getId() const { return m_Id; }
	void setId(int id) { m_Id = id; }

	//trajectories functionality
	bool add(const std::string& name);
	bool remove(const std::string& name);
	bool exist(const std::string& name);
	EditorTrajectoryPtr get(int index);
	EditorTrajectoryPtr get(const std::string& name);
	int count() { return static_cast<int>(m_Trajectories.size()); }

	int getNumPoints() const;
	float getStartTime() const;
	float getEndTime() const;

	std::string toString() const;

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Id;
		ar & m_Trajectories;
	};

private:
	int m_Id;
	std::set<EditorTrajectoryPtr, TrajectoryComparator> m_Trajectories;
};

/*-------------------------------------------------------------------*/
typedef boost::shared_ptr<EditorTrajectoryGroup> EditorTrajectoryGroupPtr;