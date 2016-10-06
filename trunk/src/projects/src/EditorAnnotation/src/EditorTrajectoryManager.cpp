#include "EditorTrajectoryManager.h"
#include <boost/make_shared.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>


/*--------------------------------------------------------------------*/
EditorTrajectoryManager::EditorTrajectoryManager()
{

}

/*--------------------------------------------------------------------*/
EditorTrajectoryGroupPtr EditorTrajectoryManager::addGroup()
{
	EditorTrajectoryGroupPtr traj = boost::make_shared<EditorTrajectoryGroup>();
	m_Groups.push_back(traj);
	return traj;
}

/*--------------------------------------------------------------------*/
EditorTrajectoryGroupPtr EditorTrajectoryManager::getGroupByIndex(int i_index)
{
	bool validIndex = i_index >= 0 && i_index < m_Groups.size();
	if (validIndex)
	{
		auto it = m_Groups.begin();
		std::advance(it, i_index);
		return *it;
	}

	return nullptr;
}

/*--------------------------------------------------------------------*/
EditorTrajectoryGroupPtr EditorTrajectoryManager::getGroupById(int id)
{
	auto it = std::find_if(m_Groups.begin(), m_Groups.end(), [id](EditorTrajectoryGroupPtr& traj){ return traj->getId() == id; });
	if (it != m_Groups.end())
		return *it;

	return nullptr;
}


/*--------------------------------------------------------------------*/
bool EditorTrajectoryManager::removeGroup(int i_index)
{
	bool validIndex = i_index >= 0 && i_index < m_Groups.size();
	if (validIndex)
	{
		auto it = m_Groups.begin();
		std::advance(it, i_index);
		m_Groups.erase(it);
	}

	return validIndex;
}

/*--------------------------------------------------------------------*/
bool EditorTrajectoryManager::removeGroup(EditorTrajectoryGroup* i_Traj)
{
	auto it = std::find_if(m_Groups.begin(), m_Groups.end(), [i_Traj](EditorTrajectoryGroupPtr& traj){ return traj.get() == i_Traj;  });
	bool bRemove = it != m_Groups.end();
	if (bRemove)
		m_Groups.erase(it);

	return bRemove;
}

/*--------------------------------------------------------------------*/
std::string EditorTrajectoryManager::toString()
{
	std::ostringstream s;
	int nr = 1;
	for (auto it = m_Groups.begin(); it != m_Groups.end(); ++it)
	{
		s << "-----Trajectory nr " << nr << std::endl << (*it)->toString() << std::endl;
		nr++;
	}

	return s.str();
}



/*--------------------------------------------------------------------*/
int EditorTrajectoryManager::TRAJECTORY_GROUP_ID = 0;




