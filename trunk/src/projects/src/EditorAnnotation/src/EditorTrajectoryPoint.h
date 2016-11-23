#pragma once 

#include <opencv2/core/types.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/shared_ptr.hpp>


class EditorTrajectoryPoint;
typedef boost::shared_ptr<EditorTrajectoryPoint> EditorTrajectoryPointPtr;

/**
 * \class EditorTrajectoryPoint
 *
 * \brief Represents single point in EditorTrajectory
 *
 */
class EditorTrajectoryPoint
{

public:
	EditorTrajectoryPoint();
	EditorTrajectoryPoint(int x, int y, float time = 0);

	bool operator==(const EditorTrajectoryPoint& point);

	void setTime(float time) { m_Time = time; }
	float getTime() const { return m_Time; }

	void setX(int x) { m_Point.x = x; }
	int getX() const { return m_Point.x; }

	void setY(int y) { m_Point.y = y; }
	int getY() const { return m_Point.y; }

	void setPos(const cv::Point& point) { m_Point = point; }
	void setPos(int x, int y) { m_Point.x = x; m_Point.y = y; }
	cv::Point getPos() const { return m_Point; }

	int getPathId() const { return m_PathId; }
	void setPathId(int id) { m_PathId = id; }

	/**
	 * String representation of EditorTrajectoryPoint
	 */
	std::string toString();

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Point.x;
		ar & m_Point.y;
		ar & m_Time;
		ar & m_PathId;
	};

private:
	cv::Point m_Point;
	float m_Time;
	//its a path id, which this point belongs to (-1 belongs to none path)
	int m_PathId;
};






