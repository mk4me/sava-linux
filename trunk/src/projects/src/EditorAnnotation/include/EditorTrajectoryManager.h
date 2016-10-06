#pragma once

#include <list>
#include <boost/serialization/list.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <utils/Singleton.h>
#include "EditorTrajectoryGroup.h"

/**
 * \class EditorTrajectoryManager
 *
 *	\brief This class stores all editor trajectories (EditorTrajectory) and manage them.
 *
 */
class EditorTrajectoryManager: public Singleton<EditorTrajectoryManager>
{

public:
	static int TRAJECTORY_GROUP_ID;

	EditorTrajectoryManager();

	/**
	 * \brief Add new empty group.
	 * \return pointer to created group.
	 */
	EditorTrajectoryGroupPtr addGroup();

	/**
	 * \brief Get group by index.
	 * \param i_index index of group.
	 * \return pointer to group (nullptr if not found).
	 */
	EditorTrajectoryGroupPtr getGroupByIndex(int i_index);

	/**
	 * \brief Get group by its id.
	 * \param i_id id of group.
	 * \return pointer to group (nullptr if not found).
	 */
	EditorTrajectoryGroupPtr getGroupById(int i_id);

	/**
	 * \brief Remove group by its index.
	 * \param i_index index of removing group.
	 * \return true if succeeded, otherwise false.
	 */
	bool removeGroup(int i_index);

	/**
	 * \brief Remove group 
	 * \param i_Traj pointer to group to delete.
	 * \return true if succeeded, otherwise false.
	 */
	bool removeGroup(EditorTrajectoryGroup* i_Traj);

	/**
	 * \brief Get count of group.
	 * \return group count.
	 */
	size_t getGroupCount() { return m_Groups.size(); }

	/**
	 * \return string representation of EditorTrajectoryManager
	 */
	std::string toString();

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Groups;
		ar & TRAJECTORY_GROUP_ID;
	};

private:
	std::list<EditorTrajectoryGroupPtr> m_Groups;
};

