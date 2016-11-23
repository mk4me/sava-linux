#include "EditorSceneTrajectoryGroup.h"
#include <QtCore/QDebug>
#include "EditorTimeline.h"

/*-------------------------------------------------------------------*/
EditorSceneTrajectoryGroup::EditorSceneTrajectoryGroup(const EditorTrajectoryGroupPtr& source, QGraphicsItem* parent /*= 0*/)
	: QGraphicsItem(parent),
	m_Source(source),
	m_CurrentSceneTrajectory(nullptr)
{
	setFlag(QGraphicsItem::ItemHasNoContents);
	setFlag(QGraphicsItem::ItemIsFocusable);

	//create editor scene trajectory
	for (int i = 0; i < m_Source->count(); i++)
	{
		EditorTrajectoryPtr child = m_Source->get(i);
		EditorSceneTrajectory* traj = new EditorSceneTrajectory(child, this);
		m_SceneTrajectories.insert(make_pair(child->getPartName(), traj));
	}
}

/*-------------------------------------------------------------------*/
EditorSceneTrajectoryGroup::~EditorSceneTrajectoryGroup()
{
	for (auto it = m_SceneTrajectories.begin(); it != m_SceneTrajectories.end(); ++it)
		delete it->second;
}


/*------------------------------- if selected item not exist, create it ------------------------------------*/
void EditorSceneTrajectoryGroup::selectPart(const std::string& name)
{
	if (!EditorConfig::getInstance().getTrajectoryParts().exist(name))
		return;

	if (!existPart(name))
	{
		addPart(name);
		goToStartTime(m_CurrentSceneTrajectory);
	}

	bool m_ShowAllParts = EditorConfig::getInstance().isTrajectoryPartsVisibled();
	for (auto it = m_SceneTrajectories.begin(); it != m_SceneTrajectories.end(); ++it)
		it->second->setVisible(m_ShowAllParts);

	m_CurrentSceneTrajectory = m_SceneTrajectories[name];

	if (m_CurrentSceneTrajectory)
		m_CurrentSceneTrajectory->setVisible(true);
}

/*-------------------------------------------------------------------*/
void EditorSceneTrajectoryGroup::addPart(const std::string& name)
{
	if (m_Source->add(name))
	{
		EditorSceneTrajectory* traj = new EditorSceneTrajectory(m_Source->get(name), this);
		m_SceneTrajectories.insert(make_pair(name, traj));
	}
}

/*-------------------------------------------------------------------*/
bool EditorSceneTrajectoryGroup::existPart(const std::string& name){
	return m_Source->exist(name);
}

/*-------------------------------------------------------------------*/
QRectF EditorSceneTrajectoryGroup::boundingRect() const{
	return parentItem() ? parentItem()->boundingRect() : QRectF();
}

/*-------------------------------------------------------------------*/
void EditorSceneTrajectoryGroup::onSelect(){
	setVisible(true);
}

/*-------------------------------------------------------------------*/
void EditorSceneTrajectoryGroup::onUnselect()
{
	setVisible(false);
}

/*-------------------------------------------------------------------*/
void EditorSceneTrajectoryGroup::goToStartTime(const EditorSceneTrajectory* traj)
{
	if (traj)
	{
		//REMOVE 
		/*int currentTime = traj->getSource()->getStartTime() - EditorCrumbledStreamData::getInstance().getSequencesRemovedFrames();
		if (currentTime >= 0)
			EditorTimeline::getInstance().setCurrentFrame(currentTime);*/
	}
}





