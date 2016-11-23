#include "EditorTrajectoryPoint.h"
#include <sstream>
#include <string>

/*--------------------------------------------------------------------*/
EditorTrajectoryPoint::EditorTrajectoryPoint() : m_Point(0, 0), m_Time(0), m_PathId(-1)
{
}

/*--------------------------------------------------------------------*/
EditorTrajectoryPoint::EditorTrajectoryPoint(int x, int y, float time /*= 0*/)
{
	m_Point.x = x;
	m_Point.y = y;
	m_Time = time;
}

/*--------------------------------------------------------------------*/
std::string EditorTrajectoryPoint::toString()
{
	std::ostringstream s;
	s << "[x:" << m_Point.x << ", y=" << m_Point.y << ", t=" << m_Time << "]";
	return s.str();
}

/*-------------------------------------------------------------------*/
bool EditorTrajectoryPoint::operator==(const EditorTrajectoryPoint& point)
{
	return (point.m_Point == point.m_Point && point.m_Time == point.m_Time);
}

