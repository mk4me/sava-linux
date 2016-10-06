
#include "EditorSceneTrajectory.h"
#include <QtCore/QDebug>
#include "EditorSceneLayerManager.h"

/*--------------------------------------------------------------------*/
EditorSceneTrajectory::EditorSceneTrajectory(const EditorTrajectoryPtr& sourceTrajectory, QGraphicsItem* parent /*= 0*/)
:QGraphicsItem(parent), 
m_SourceTrajectory(sourceTrajectory)
{
	m_Color = EditorConfig::getInstance().getTrajectoryParts().getColorByName(m_SourceTrajectory->getPartName());
	
	for (int i = 0; i < m_SourceTrajectory->getNumPoints(); i++)
		addPoint(new EditorSceneTrajectoryPoint(m_SourceTrajectory->getPoint(i), this));

	setVisible(false);

	EditorMessageManager::registerMessage(MESSAGE_TRAJECTORY_PART_CHANGED, this);
}

/*--------------------------------------------------------------------*/
EditorSceneTrajectory::~EditorSceneTrajectory()
{
	for (auto it = m_ScenePoints.begin(); it != m_ScenePoints.end(); ++it)
		delete it->second;

	m_ScenePoints.clear();

	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
EditorSceneTrajectoryPoint* EditorSceneTrajectory::addPoint(int x, int y, float time /*= 0*/)
{
	EditorTrajectoryPointPtr point = m_SourceTrajectory->addPoint(x, y, time);
	EditorSceneTrajectoryPoint* scenePoint = nullptr;
	if (point)
	{
		scenePoint = new EditorSceneTrajectoryPoint(point, this);
		addPoint(scenePoint);

		std::vector<size_t> vec = EditorSceneLayerHelper::getObjectLayer()->getObjectIdsByCursorPos();
		if (!vec.empty())
			point->setPathId(*(vec.begin()));
	}

	return scenePoint;
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectory::addPoint(EditorSceneTrajectoryPoint* point){
	m_ScenePoints.insert(std::make_pair(point->getSourcePoint()->getTime(), point));
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectory::deletePoint(EditorSceneTrajectoryPoint* point)
{
	EditorTrajectoryPointPtr sourcePoint = point->getSourcePoint();
	m_SourceTrajectory->removePoint(sourcePoint);

	delete point;
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectory::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setOpacity(1);

	//draw trajectory line 
	size_t pointsNum = m_SourceTrajectory->getNumPoints();
	if (pointsNum > 1)
	{
		int lineWidth = EditorConfig::getInstance().getTrajectoryLineWidth();

		QPen pen;
		pen.setWidth(lineWidth);
		pen.setColor(m_Color);
		painter->setPen(pen);

		EditorTrajectoryPointPtr trajPoint = m_SourceTrajectory->getPoint(0);
		QPointF linePoint1(trajPoint->getX(), trajPoint->getY());
		QPointF linePoint2;

		for (size_t i = 1; i < pointsNum; i++)
		{
			trajPoint = m_SourceTrajectory->getPoint(i);
			linePoint2 = QPointF(trajPoint->getX(), trajPoint->getY());
			painter->drawLine(linePoint1, linePoint2);
			linePoint1 = linePoint2;
		}
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectory::deleteSelectedPoint()
{
	std::vector<float> m_TimesToDelete;

	for (auto it = m_ScenePoints.begin(); it != m_ScenePoints.end(); ++it)
	{
		EditorSceneTrajectoryPoint* trajPoint = it->second;
		if (trajPoint && trajPoint->isSelected())
		{
			deletePoint(trajPoint);
			m_TimesToDelete.push_back(it->first);
			update();
		}
	}

	for each(float time in m_TimesToDelete)
		m_ScenePoints.erase(time);
}

/*--------------------------------------------------------------------*/
QRectF EditorSceneTrajectory::boundingRect() const {
	return parentItem() ? parentItem()->boundingRect() : QRectF();
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectory::selectPointByTime(float i_time)
{
	for each(auto elem in m_ScenePoints)
	{
		EditorSceneTrajectoryPoint* point = elem.second;
		point->setSelected(point->getSourcePoint()->getTime() == i_time);
	}
}

/*-------------------------------------------------------------------*/
void EditorSceneTrajectory::unselectAllPoint()
{
	for each(auto elem in m_ScenePoints)
	{
		EditorSceneTrajectoryPoint* point = elem.second;
		point->setSelected(false);
	}
}


/*-------------------------------------------------------------------*/
void EditorSceneTrajectory::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	m_Color = EditorConfig::getInstance().getTrajectoryParts().getColorByName(m_SourceTrajectory->getPartName());
	update();
}





