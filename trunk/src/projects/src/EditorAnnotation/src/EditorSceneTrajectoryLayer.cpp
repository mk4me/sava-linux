#include <QtGui/QPen>
#include <QtCore/QDebug>
#include "QtCore/QObject"

#include "EditorSceneTrajectoryLayer.h"
#include "EditorSceneTrajectoryPoint.h"
#include "EditorTrajectoryPoint.h"
#include "EditorSceneTrajectory.h"
#include "EditorTimeline.h"


/*--------------------------------------------------------------------*/
EditorSceneTrajectoryLayer::EditorSceneTrajectoryLayer(const EditorLayerPtr& i_SourceLayer)
	:EditorSceneLayer(i_SourceLayer),
	 m_CurrentGroup(nullptr)
{
	m_TrajectoryManager = EditorTrajectoryManager::getPointer();

	for (int i = 0; i < m_TrajectoryManager->getGroupCount(); i++)
		createElement(m_TrajectoryManager->getGroupByIndex(i)->getId());

	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_TRAJECTORY_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_TRAJECTORY_PART_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_CONFIG_CHANGED, this);
}

/*--------------------------------------------------------------------*/
EditorSceneTrajectoryLayer::~EditorSceneTrajectoryLayer()
{
	for (auto it = m_SceneTrajectoryMap.begin(); it != m_SceneTrajectoryMap.end(); ++it)
		delete it->second;

	EditorMessageManager::unregisterMessages(this);
}

/*-------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::init()
{
	m_PrevTrajectoryPixmapItem = new TrajectoriePointViewItem(this);
	m_PrevTrajectoryPixmapItem->setFlag(QGraphicsItem::ItemIsMovable);
	m_PrevTrajectoryPixmapItem->setFlag(QGraphicsItem::ItemIsFocusable);
}

/*-------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::redraw()
{
	QPoint pos;
	bool bPathChanged;

	m_PrevTrajectoryPixmapItem->setPixmap(getPrevTrajPixmap(bPathChanged, pos));
	m_PrevTrajectoryPixmapItem->setFixedPos(pos);
	if (bPathChanged)
		m_PrevTrajectoryPixmapItem->setOffset(0, 0);
}


/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::selectElement(int id)
{
	EditorSceneTrajectoryGroup* newGroup = getTrajectoryGroupById(id);
	if (newGroup != m_CurrentGroup)
	{
		if (m_CurrentGroup) 
			m_CurrentGroup->onUnselect();

		m_CurrentGroup = newGroup;

		if (m_CurrentGroup)
		{
			m_CurrentGroup->onSelect();
			m_CurrentGroup->selectPart(m_CurrentPartName);
		}

		update();
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::createElement(int id)
{
	EditorTrajectoryGroupPtr traj = m_TrajectoryManager->getGroupById(id);
	if (traj)
	{
		EditorSceneTrajectoryGroup* group = new EditorSceneTrajectoryGroup(traj);
		group->setParentItem(this);
		m_SceneTrajectoryMap.insert(std::make_pair(id, group));
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::deleteElement(int id)
{
	EditorSceneTrajectoryGroup* sceneTraj = getTrajectoryGroupById(id);
	m_SceneTrajectoryMap.erase(m_SceneTrajectoryMap.find(id));

	delete sceneTraj;
	m_CurrentGroup = nullptr;

	update();
}


/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::addNewPoint(int x, int y)
{
	//REMOVE 
	/*if (m_CurrentGroup && m_CurrentGroup->getCurrentTrajectory())
	{
		float currentTime = EditorTimeline::getInstance().getCurrentFrame() + EditorCrumbledStreamData::getInstance().getSequencesRemovedFrames();

		if (m_CurrentGroup->getCurrentTrajectory()->addPoint(x, y, currentTime)){
			EditorMessageManager::sendMessage(MESSAGE_TRAJECTORY_CHANGED);
			EditorTimeline::getInstance().nextFrame();
		} else {
			QTimer::singleShot(1, this, SLOT(selectCurrentTimePoint()));
		}
	}*/
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	addNewPoint(event->scenePos().x(), event->scenePos().y());
	EditorSceneLayer::mouseDoubleClickEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::keyPressEvent(QKeyEvent *event)
{
	if (m_CurrentGroup && m_CurrentGroup->getCurrentTrajectory() && event->key() == Qt::Key_Delete)
	{
		m_CurrentGroup->getCurrentTrajectory()->deleteSelectedPoint();
		EditorMessageManager::sendMessage(MESSAGE_TRAJECTORY_CHANGED);
	}

	EditorSceneLayer::keyPressEvent(event);
}

/*--------------------------------------------------------------------*/
EditorSceneTrajectoryGroup* EditorSceneTrajectoryLayer::getTrajectoryGroupById(int id)
{
	auto it = m_SceneTrajectoryMap.find(id);
	if (it != m_SceneTrajectoryMap.end())
		return it->second;

	return nullptr;
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
		case MESSAGE_FRAME_CHANGED:
			selectCurrentTimePoint();
			break;

		case MESSAGE_TRAJECTORY_CHANGED:
			if (m_CurrentGroup)
				m_CurrentGroup->update();
			break;

		case MESSAGE_TRAJECTORY_PART_CHANGED:
			//if (m_CurrentGroup)
			//	m_CurrentGroup->selectPart(i_ExtraParam);
			break;

		case MESSAGE_CONFIG_CHANGED:
			setVisible(EditorConfig::getInstance().isSceneTrajectoryVisibled());
			m_PrevTrajectoryPixmapItem->setVisible(EditorConfig::getInstance().isTrajectoryHelpViewVisibled());

			break;
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::selectCurrentTimePoint()
{
	if (m_CurrentGroup && m_CurrentGroup->getCurrentTrajectory())
		m_CurrentGroup->getCurrentTrajectory()->selectPointByTime(EditorTimeline::getInstance().getCurrentFrame());
}

/*-------------------------------------------------------------------*/
void EditorSceneTrajectoryLayer::unselectAllPoints()
{
	if (m_CurrentGroup && m_CurrentGroup->getCurrentTrajectory())
		m_CurrentGroup->getCurrentTrajectory()->unselectAllPoint();
}


/*-------------------------------------------------------------------*/
QPixmap EditorSceneTrajectoryLayer::getPrevTrajPixmap(bool& o_PathIdChanged, QPoint& o_Position /*= QPoint()*/) const
{

	static int prevPathId = -1;

	o_PathIdChanged = false;
	o_Position = QPoint(0, 0);

	//get current trajectory
	EditorTrajectoryPtr traj;
	if (m_CurrentGroup && m_CurrentGroup->getCurrentTrajectory())
		traj = m_CurrentGroup->getCurrentTrajectory()->getSource();

	if (!traj)
		return QPixmap();

	//find prev trajectory point
	int currentFrame = EditorTimeline::getInstance().getCurrentFrame();
	EditorTrajectoryPointPtr currPoint = traj->getPointByTime(currentFrame);
	EditorTrajectoryPointPtr prevPoint;
	if (currPoint)	
	{
		prevPoint = traj->getPrevPoint(currPoint);
	}
	else if (traj->getNumPoints() > 0)
	{
		prevPoint = traj->getPoint(traj->getNumPoints() - 1);
	}

	if (!prevPoint)
		return QPixmap();

	//check if path id changed
	if (prevPathId != prevPoint->getPathId())
	{
		prevPathId = prevPoint->getPathId();
		o_PathIdChanged = true;
	}

	//draw image
	//REMOVE cv::Mat img = EditorCrumbledStreamData::getInstance().getCrumbledSequence()->getMaskedFrameByPathId(prevPoint->getTime(), prevPoint->getPathId());
	QPixmap pix; /*= ImageConverter::cvMatToQPixmap(img);
	if (!pix.isNull())
	{
		QPainter painter(&pix);

		//draw point
		cv::Rect pathRect = EditorCrumbledStreamData::getInstance().getCrumbledSequence()->getRectPathById(prevPoint->getTime(), prevPoint->getPathId());
		QPoint pointOffset(prevPoint->getX() - pathRect.x, prevPoint->getY() - pathRect.y);
		QColor color = m_CurrentGroup->getCurrentTrajectory()->getColor();
		int radius = EditorConfig::getInstance().getTrajectoryPointRadius();

		painter.save();
		painter.setPen(Qt::black);
		painter.setBrush(color);
		painter.drawEllipse(pointOffset.x() - radius, pointOffset.y() - radius, 2 * radius, 2 * radius);
		painter.restore();

		//draw border
		QPen pen;
		pen.setColor(Qt::red);
		pen.setStyle(Qt::DashDotLine);
		pen.setWidth(3);
		painter.setPen(pen);
		painter.drawRect(pix.rect().adjusted(0,0,-2,-2));

		cv::Rect currentPathRect = EditorCrumbledStreamData::getInstance().getCrumbledSequence()->getRectPathById(currentFrame, prevPoint->getPathId());
		o_Position = QPoint(currentPathRect.x + currentPathRect.width + 7, currentPathRect.y);
	}
	*/return pix;
}