#pragma once

#include <sstream>
#include <vector>
#include <map>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "EditorTrajectoryPoint.h"


class EditorTrajectory;
typedef boost::shared_ptr<EditorTrajectory> EditorTrajectoryPtr;

/**
 * \class EditorTrajectory
 *
 * \brief Represents trajectory of object/person.
 *		 
 */
class EditorTrajectory
{
public:
	EditorTrajectory();

	int getId() const { return m_Id; }

	std::string getPartName() const { return m_PartName; }
	void setPartName(const std::string& name) { m_PartName = name; }

	EditorTrajectoryPointPtr addPoint(int i_x, int i_y, float i_time = 0);
	bool removePoint(int i_index);
	bool removePoint(const EditorTrajectoryPointPtr& i_Point);

	EditorTrajectoryPointPtr getPoint(size_t i_index);
	EditorTrajectoryPointPtr getPointByTime(float i_Time);

	EditorTrajectoryPointPtr getNextPoint(const EditorTrajectoryPointPtr& point);
	EditorTrajectoryPointPtr getPrevPoint(const EditorTrajectoryPointPtr& point);

	size_t getNumPoints() const;

	float getStartTime() const;
	float getEndTime() const;

	std::string toString();

protected:
	void sortPoints();

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Id;
		ar & m_PartName;
		ar & m_Points;
	};

private:
	int m_Id;

	std::string m_PartName;
	std::vector<EditorTrajectoryPointPtr> m_Points;
};