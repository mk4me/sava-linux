#include "EditorTrajectoryParts.h"
#include "boost/smart_ptr/make_shared_object.hpp"
#include <fstream>
#include <boost/archive//text_iarchive.hpp>
#include <boost/archive//text_oarchive.hpp>
#include <QtCore/QDebug>



//TRAJECTORY PARTS
/*--------------------------------------------------------------------*/
EditorTrajectoryParts::EditorTrajectoryParts()
{
	
}

/*--------------------------------------------------------------------*/
bool EditorTrajectoryParts::add(const std::string& name, QColor color /*= QColor()*/)
{
	auto foundIt = std::find_if(m_Parts.begin(), m_Parts.end(), [name](TrajectoryPartPtr& part){ return part->getName() == name; });
	if (foundIt == m_Parts.end())
	{
		m_Parts.push_back(boost::make_shared<TrajectoryPart>(name, color));
		return true;
	}

	return false;
}

/*--------------------------------------------------------------------*/
bool EditorTrajectoryParts::remove(const std::string& name)
{
	auto foundedIt = std::find_if(m_Parts.begin(), m_Parts.end(), [name](const TrajectoryPartPtr& part) { return part->getName() == name; });
	bool removed = foundedIt != m_Parts.end();
	if (removed)
		m_Parts.erase(foundedIt);

	return removed;
}

/*--------------------------------------------------------------------*/
bool EditorTrajectoryParts::remove(int index)
{
	if (index < 0 || index >= m_Parts.size())
		return false;

	auto it = m_Parts.begin() + index;
	m_Parts.erase(it);

	return true;
}

/*--------------------------------------------------------------------*/
TrajectoryPartPtr EditorTrajectoryParts::get(int index)
{
	if (index < 0 || index >= m_Parts.size())
		return nullptr;

	return m_Parts[index];
}

/*-------------------------------------------------------------------*/
TrajectoryPartPtr EditorTrajectoryParts::get(const std::string& name)
{
	auto foundedIt = std::find_if(m_Parts.begin(), m_Parts.end(), [name](const TrajectoryPartPtr& part) { return part->getName() == name; });
	return (foundedIt != m_Parts.end()) ? *foundedIt : nullptr;
}

/*--------------------------------------------------------------------*/
int EditorTrajectoryParts::moveUp(int index)
{
	if (index < 0 || index >= m_Parts.size())
		return -1;

	if (index == 0)
		return 0;

	std::swap(m_Parts[index - 1], m_Parts[index]);
	return index - 1;
}

/*--------------------------------------------------------------------*/
int EditorTrajectoryParts::moveDown(int index)
{
	if (index < 0 || index >= m_Parts.size())
		return -1;

	if (index == m_Parts.size() - 1)
		return index;

	std::swap(m_Parts[index], m_Parts[index + 1]);
	return index + 1;
}

/*-------------------------------------------------------------------*/
bool EditorTrajectoryParts::exist(const std::string& name)
{
	auto it = std::find_if(m_Parts.begin(), m_Parts.end(), [name](const TrajectoryPartPtr& part) { return part->getName() == name; });
	return it != m_Parts.end();
}

/*-------------------------------------------------------------------*/
QColor EditorTrajectoryParts::getColorByName(const std::string& name)
{
	TrajectoryPartPtr part = get(name);
	return part ? part->getColor() : QColor();
}



