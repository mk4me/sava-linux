#include "EditorTrajectory.h"
#include <boost/make_shared.hpp>
#include <QtCore/QtGlobal>

/*--------------------------------------------------------------------*/
EditorTrajectory::EditorTrajectory()
{
	
}

/*-------------------------------------------------------------------*/
size_t EditorTrajectory::getNumPoints() const
{
	return m_Points.size();
}

/*--------------------------------------------------------------------*/
EditorTrajectoryPointPtr EditorTrajectory::addPoint(int i_x, int i_y, float i_time /*= 0*/)
{
	if (getPointByTime(i_time) != nullptr) //if point exists in this time, don't create next one
		return nullptr;

	EditorTrajectoryPointPtr point = boost::make_shared<EditorTrajectoryPoint>(i_x, i_y, i_time);

	auto it = std::lower_bound(m_Points.begin(), m_Points.end(), point,
		[](const EditorTrajectoryPointPtr& point1, const EditorTrajectoryPointPtr& point2) { return point1->getTime() < point2->getTime(); });

	m_Points.insert(it, point);
	return point;
}

/*--------------------------------------------------------------------*/
bool EditorTrajectory::removePoint(int i_index)
{
	bool validIndex = i_index >= 0 && i_index < m_Points.size();
	if (validIndex)
		m_Points.erase(m_Points.begin() + i_index);

	return validIndex;
}

/*--------------------------------------------------------------------*/
bool EditorTrajectory::removePoint(const EditorTrajectoryPointPtr& i_Point)
{
	auto it = std::find(m_Points.begin(), m_Points.end(), i_Point);
	bool shouldRemove = it != m_Points.end();
	if (shouldRemove)
		m_Points.erase(it);

	return shouldRemove;
}

/*--------------------------------------------------------------------*/
EditorTrajectoryPointPtr EditorTrajectory::getPoint(size_t i_index)
{
	bool validIndex = i_index >= 0 && i_index < m_Points.size();
	if (validIndex)
		return m_Points[i_index];

	return nullptr;
}

/*--------------------------------------------------------------------*/
EditorTrajectoryPointPtr EditorTrajectory::getPointByTime(float i_Time)
{
	auto it = std::find_if(m_Points.begin(), m_Points.end(), [i_Time](const EditorTrajectoryPointPtr& point) { return point->getTime() == i_Time; });
	if (it != m_Points.end())
		return *it;

	return nullptr;
}

/*--------------------------------------------------------------------*/
std::string EditorTrajectory::toString()
{
	std::ostringstream s;
	int index = 1;
	for (auto it = m_Points.begin(); it != m_Points.end(); ++it)
	{
		s << index << ". " << (*it)->toString() << std::endl;
		index++;
	}

	return s.str();
}

/*--------------------------------------------------------------------*/
void EditorTrajectory::sortPoints()
{
	std::sort(m_Points.begin(), m_Points.end(), [](const EditorTrajectoryPointPtr& point1, const EditorTrajectoryPointPtr& point2) { return point1->getTime() < point2->getTime(); });
}

/*--------------------------------------------------------------------*/
float EditorTrajectory::getStartTime() const
{
	if (!m_Points.empty())
		return (*m_Points.begin())->getTime();

	return -1;
}

/*--------------------------------------------------------------------*/
float EditorTrajectory::getEndTime() const
{
	if (!m_Points.empty())
		return (*(m_Points.end() - 1))->getTime();

	return -1;
}

/*-------------------------------------------------------------------*/
EditorTrajectoryPointPtr EditorTrajectory::getNextPoint(const EditorTrajectoryPointPtr& point)
{
	if (!point)
		return nullptr;

	auto it = std::find(m_Points.begin(), m_Points.end(), point);
	if (it != m_Points.end() && (it + 1) != m_Points.end())
		return *(it + 1);

	return nullptr;
}

/*-------------------------------------------------------------------*/
EditorTrajectoryPointPtr EditorTrajectory::getPrevPoint(const EditorTrajectoryPointPtr& point)
{
	if (!point)
		return nullptr;

	auto it = std::find(m_Points.begin(), m_Points.end(), point);
	if (it != m_Points.end() && it != m_Points.begin())
		return *(it - 1);

	return nullptr;
}




