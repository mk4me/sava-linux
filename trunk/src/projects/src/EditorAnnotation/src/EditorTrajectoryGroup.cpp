#include "EditorTrajectoryGroup.h"
#include <boost/make_shared.hpp>
#include <QtCore/qglobal.h>
#include "EditorSingletons.h"
#include <limits>


/*-------------------------------------------------------------------*/
EditorTrajectoryGroup::EditorTrajectoryGroup()
{
	m_Id = EditorTrajectoryManager::TRAJECTORY_GROUP_ID++;
}

/*--------------------------------------------------------------------*/
bool EditorTrajectoryGroup::add(const std::string& name)
{
	EditorTrajectoryPtr traj = boost::make_shared<EditorTrajectory>();
	traj->setPartName(name);
	auto insertIt = m_Trajectories.insert(traj);

	return insertIt.second;
}

/*-------------------------------------------------------------------*/
bool EditorTrajectoryGroup::remove(const std::string& name)
{
	auto it = std::find_if(m_Trajectories.begin(), m_Trajectories.end(), [name](const EditorTrajectoryPtr& traj) { return traj->getPartName() == name; });
	bool bFounded = it != m_Trajectories.end();
	if (bFounded)
		m_Trajectories.erase(it);

	return bFounded;
}

/*--------------------------------------------------------------------*/
EditorTrajectoryPtr EditorTrajectoryGroup::get(int index)
{
	if (index < 0 || index >= m_Trajectories.size())
		return nullptr;

	auto it = m_Trajectories.begin();
	std::advance(it, index);
	return *it;
}

/*--------------------------------------------------------------------*/
EditorTrajectoryPtr EditorTrajectoryGroup::get(const std::string& name)
{
	auto it = std::find_if(m_Trajectories.begin(), m_Trajectories.end(), [name](const EditorTrajectoryPtr& traj) { return traj->getPartName() == name; });
	return (it != m_Trajectories.end()) ? *it : nullptr;
}

/*-------------------------------------------------------------------*/
bool EditorTrajectoryGroup::exist(const std::string& name)
{
	return get(name) != nullptr;
}

/*-------------------------------------------------------------------*/
int EditorTrajectoryGroup::getNumPoints() const
{
	size_t num = 0;
	std::for_each(m_Trajectories.begin(), m_Trajectories.end(), [&num](const EditorTrajectoryPtr& traj) { num += traj->getNumPoints(); });
	return static_cast<int>(num);
}

/*-------------------------------------------------------------------*/
float EditorTrajectoryGroup::getStartTime() const
{
	float minVal = 100000000000000000;
	for (EditorTrajectoryPtr traj : m_Trajectories)
	{
		float val = traj->getStartTime();
		if (val >= 0)
			minVal = qMin(minVal, val);
	}

	return minVal;
}

/*-------------------------------------------------------------------*/
float EditorTrajectoryGroup::getEndTime() const
{
	float maxVal = 0;
	for (EditorTrajectoryPtr traj : m_Trajectories)
	{
		float val = traj->getEndTime();
		if (val >= 0)
			maxVal = qMax(maxVal, val);
	}

	return maxVal;
}


/*-------------------------------------------------------------------*/
std::string EditorTrajectoryGroup::toString() const{
	return std::string();
}



