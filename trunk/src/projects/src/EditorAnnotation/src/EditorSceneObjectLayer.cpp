#include "EditorSceneObjectLayer.h"
#include "EditorGraphicsView.h"
#include "EditorSequencesManager.h"

#include <QtCore\QRect>
#include <QtGui\QCursor>
#include <QtWidgets\QGraphicsView>

/*--------------------------------------------------------------------*/
EditorSceneObjectLayer::EditorSceneObjectLayer(const EditorLayerPtr& i_SourceLayer)
:EditorSceneLayer(i_SourceLayer),
m_SelectedObject(nullptr),
m_EditMode(EDIT_DISABLE),
m_Action(ACTION_NONE),
m_IsEditLocked(false)
{
	EditorMessageManager::registerMessage(MESSAGE_SCENE_UNSELECT_ALL, this);
	EditorMessageManager::registerMessage(MESSAGE_LOCK_SCENE_OBJECT, this);
	EditorMessageManager::registerMessage(MESSAGE_UNCLOCK_SCENE_OBJECT, this); 
	EditorMessageManager::registerMessage(MESSAGE_PREPARE_FRAME_CHANGE, this);
	setAcceptHoverEvents(true);
}

/*--------------------------------------------------------------------*/
EditorSceneObjectLayer::~EditorSceneObjectLayer()
{
	EditorMessageManager::unregisterMessages(this);
	deleteObjects();
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::init()
{
	m_Scene = static_cast<EditorScene*>(scene());
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::reload()
{
	setAction(ACTION_NONE);
	m_EditMode = EDIT_DISABLE;
	m_IsEditLocked = false;
	setSelectedObject(nullptr);

	deleteObjects();

	//create 
	std::map<size_t, ClusterPtr> i_Clusters = EditorSequencesManager::getInstance().getSequence()->getClusters();
	for each(auto pair in i_Clusters) {
		EditorSceneObject* newObj = createObject(pair.first);
		newObj->setClusterId(pair.second->getClusterId());
	}

}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::redraw()
{
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (!sequence)
		return;

	auto regions = sequence->getClusterFrames(EditorTimeline::getInstance().getCurrentFrame());

	//wyczyœæ flagi
	for each (auto pair in m_SceneObjects) {
		EditorSceneObject* obj = pair.second;
		obj->setVisible(false);
	}

	//To jest obiekt na scenie, który ma taki sam clsuter id, jak zaznaczony obiekt.
	//Jest to funkcjonalnoœæ, która s³u¿y do zachowania ci¹g³oœci zaznaczenia, obiektu o tym samym cluster id 
	EditorSceneObject* l_NextSelectedClusterIdObject = nullptr;

	//nadaj rozmiar i id-s obiektom na scenie
	for (auto it = regions.begin(); it != regions.end(); ++it)
	{
		EditorSceneObject* object = m_SceneObjects[it->first];
		if (!object || (object != m_SelectedObject && m_IsEditLocked))
			continue;
	
		object->setRect(it->second.x, it->second.y, it->second.width, it->second.height);
		object->setVisible(true);

		if (m_SelectedObject && m_SelectedObject->getClusterId() == object->getClusterId())
			l_NextSelectedClusterIdObject = object;
	}

	if (l_NextSelectedClusterIdObject)
		setSelectedObject(l_NextSelectedClusterIdObject);

}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::unselectAll(){
	setSelectedObject(nullptr);
}

/*--------------------------------------------------------------------*/
int EditorSceneObjectLayer::getSelectedObjectId() const {
return m_SelectedObject ? m_SelectedObject->getId() : -1;
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::setSelectedObjectById(int i_Id){
	setSelectedObject(getObjectById(i_Id));
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::deleteSelectedObject()
{
	if (m_Action != ACTION_NONE)
		return;

	if (m_SelectedObject)
	{
		size_t cluster_Id = m_SelectedObject->getId();
		EditorSequencesManager::getInstance().getSequence()->removeCluster(cluster_Id);
		deleteObject(cluster_Id);
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::deleteObjects()
{
	for (auto it = m_SceneObjects.begin(); it != m_SceneObjects.end(); ++it)
		if (it->second != nullptr)
		{
			it->second->setParentItem(nullptr);
			delete it->second;
		}
	
	m_SceneObjects.clear();
}


/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::lockEditSceneObject(bool i_LockEnabled)
{
	m_IsEditLocked = i_LockEnabled;
	if (m_IsEditLocked)
		setAction(ACTION_EDIT);
	else
		setAction(ACTION_NONE);

	EditorMessageManager::sendMessage(MESSAGE_SCENE_UPDATE);
}

/*--------------------------------------------------------------------*/
EditorSceneObject* EditorSceneObjectLayer::getObjectById(size_t i_Id)
{
	auto foundIt = m_SceneObjects.find(i_Id);
	return (foundIt != m_SceneObjects.end()) ? foundIt->second : nullptr;	
}


/*--------------------------------------------------------------------*/
QPoint EditorSceneObjectLayer::getMouseScenePos() {
	return getView()->mapToScene(getView()->mapFromGlobal(QCursor::pos())).toPoint();
}


/*--------------------------------------------------------------------*/
EditorSceneObject* EditorSceneObjectLayer::createObject(size_t i_Id)
{
	EditorSceneObject * object = new EditorSceneObject(i_Id);
	object->setRect(0, 0, 1, 1);
	object->setVisible(false);
	object->setParentItem(this);
	m_SceneObjects.insert(std::make_pair(i_Id, object));

	return object;
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::deleteObject(size_t i_Id)
{
	auto foundIt = m_SceneObjects.find(i_Id);
	if (foundIt != m_SceneObjects.end())
	{
		if (foundIt->second == m_SelectedObject)
			setSelectedObject(nullptr);

		foundIt->second->setParentItem(nullptr);
		delete foundIt->second;
		m_SceneObjects.erase(foundIt);

		EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_DELETED);
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::setSelectedObject(EditorSceneObject* i_Obj)
{
	if (m_IsEditLocked)
		return;

	if (m_SelectedObject != i_Obj)
	{
		//unselect prev scene object
		if (m_SelectedObject)
		{
			m_SelectedObject->setSelected(false);
			m_SelectedObject->updateRegion();
		}

		m_SelectedObject = i_Obj;

		if (m_SelectedObject)
		{
			m_SelectedObject->setSelected(true);
			m_SelectedObject->updateRegion();
		}

		//notify selection changed
		EditorMessageManager::sendMessage(MESSAGE_SCENE_SELECTION_CHANGED);
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	if (!EditorSequencesManager::getPointer()->isLoaded())
		return;

	switch (i_Message)
	{
		case MESSAGE_SCENE_UNSELECT_ALL:
			unselectAll();
			lockEditSceneObject(false);
			break;

		case MESSAGE_LOCK_SCENE_OBJECT:
			lockEditSceneObject(true);
			break;

		case MESSAGE_UNCLOCK_SCENE_OBJECT:
			lockEditSceneObject(false);
			break;

		case MESSAGE_PREPARE_FRAME_CHANGE:
			updateCluster(i_ExtraParam); //i_ExtraParam is next frame
			break;
	}
}

/*-------------------------------------------------------------------*/
std::vector<size_t> EditorSceneObjectLayer::getObjectIdsByCursorPos()
{
	QGraphicsView* view = getView();

	std::vector<size_t> ids;
	QPointF cursorScenePos = getMouseScenePos();
	for each(auto pair in m_SceneObjects)
	{
		EditorSceneObject* obj = pair.second;
		if (obj && obj->rect().contains(cursorScenePos))
			ids.push_back(obj->getId());
	}	

	return ids;
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::addSceneObject(QRectF& i_ObjRect /*= QRectF()*/)
{
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (sequence)
	{
		//set random rectangle object
		if (i_ObjRect.isNull())
			i_ObjRect = QRectF(10, 10, 100, 100);
			
		if (!i_ObjRect.isNull())
		{
			int currentFrame = EditorTimeline::getInstance().getCurrentFrame();

			size_t id = sequence->createCluster();
			addClusterRect(id, i_ObjRect);
			addClusterRect(id, i_ObjRect, currentFrame + NEW_CLUSTER_TIME_RANGE - 1);

			//add scene object
			EditorSceneObject* createdObj = createObject(id);
			createdObj->setClusterId(sequence->getCluster(id)->getClusterId());

			setSelectedObject(createdObj); 
			EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_CREATED);
			EditorMessageManager::sendMessage(MESSAGE_SCENE_UPDATE);
		}
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::updateCluster(int i_Time)
{

	if (!EditorTimeline::getInstance().isFrameValid(i_Time))
		return;
	
	if (m_SelectedObject &&
		m_SelectedObject->isVisible() &&
		m_Action == ACTION_EDIT && 
		m_EditMode != EDIT_DISABLE)
		addClusterRect(m_SelectedObject->getId(), m_SelectedObject->rect(), i_Time);
	
}


/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::addClusterRect(size_t i_ClusterId, const QRectF& i_Rect, int i_Time /*= -1*/)
{
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (sequence) {
		if (i_Time < 0)
			i_Time = EditorTimeline::getInstance().getCurrentFrame();

		i_Time = std::min(i_Time, EditorTimeline::getInstance().getFrameCount() - 1);
		sequence->addClusterFrame(i_ClusterId, i_Rect.toRect(), i_Time);
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	getView()->setFocus();
	EditorSceneLayer::hoverEnterEvent(event);
}


/*--------------------------------------------------------------------*/
bool EditorSceneObjectLayer::canEdit(Qt::KeyboardModifiers modifiers) const{
	return m_SelectedObject && (modifiers.testFlag(OBJECT_EDIT_MODIFY_KEY) || m_IsEditLocked);
}

/*--------------------------------------------------------------------*/
bool EditorSceneObjectLayer::canCreate(Qt::KeyboardModifiers modifiers) const{
	return modifiers.testFlag(OBJECT_CREATE_KEY) && !m_IsEditLocked;
}

////////////// TODO dave - poni¿szy kod do osobnego obiektu (i zrobic porz¹dek) ///////////////////////////////////////////


/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	if (m_Action == ACTION_EDIT){
		updateEditMode();
	}
	if (m_Action == ACTION_NONE){
		getView()->setCursor(getView()->getScale() > 1 ? Qt::OpenHandCursor : Qt::ArrowCursor);
	}

	EditorSceneLayer::wheelEvent(event);
}


/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::keyPressEvent(QKeyEvent *event)
{
	if (canCreate(event->modifiers()))
		setAction(ACTION_CREATE);
	else if (canEdit(event->modifiers()))
		setAction(ACTION_EDIT);

	EditorSceneLayer::keyPressEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::keyReleaseEvent(QKeyEvent *event)
{

	bool isframeChanging = (event->key() == Qt::Key_D || event->key() == Qt::Key_A);

	if (!isframeChanging)
	{
		if (m_Action == ACTION_CREATE){
			if (!(QApplication::mouseButtons() & Qt::LeftButton))
				setAction(ACTION_NONE);
		}
		else if (m_Action == ACTION_EDIT){
			if (!m_IsEditLocked)
				setAction(ACTION_NONE);
		}
	}


	if (event->key() == Qt::Key_Delete)
		deleteSelectedObject();
	

	EditorSceneLayer::keyReleaseEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::mouseMove(QGraphicsSceneMouseEvent* e)
{
	if (m_Action == ACTION_EDIT)
	{
		updateEditMode();
		updateObjectSize();
	}

	EditorSceneLayer::mouseMove(e);
}


/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_Action == ACTION_EDIT)
	{
		m_CachedData.savedMousePos = getMouseScenePos();
		m_CachedData.savedObjRect = m_SelectedObject ? m_SelectedObject->rect().toRect() : QRect();
	}

	EditorSceneLayer::mousePressEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::mouseRelease(QGraphicsSceneMouseEvent *event)
{
	//tworzymy nowy prostok¹t/cluster jeœli bylismy w akcji tworzenia
	if (m_Action == ACTION_CREATE)
	{
		addSceneObject(m_Scene->selectionArea().boundingRect().intersected(m_Scene->sceneRect()));
		setAction(ACTION_NONE);
	}

	if (m_Action == ACTION_EDIT)
	{
		updateCluster(EditorTimeline::getInstance().getCurrentFrame());
	}

	//jesli trzymamy OBJECT_EDIT_MODIFY_KEY zostajemy w akcji edytowania
	if (canEdit(QApplication::keyboardModifiers()))
		setAction(ACTION_EDIT);


	EditorSceneLayer::mouseRelease(event);
}

/*--------------------------------------------------------------------*/
bool EditorSceneObjectLayer::isCursonInCorner(QRectF originalRec, QPointF cursorPos, QPointF corner)
{
	//float box_multiplier = 0.1;
	//float box_width = box_multiplier * originalRec.width();
	//float box_height = box_multiplier * originalRec.height();

	float box_width = 10;
	float box_height = 10;
	QRectF cornerBox(corner.x() - box_width / 2, corner.y() - box_height / 2, box_width, box_height);

	return cornerBox.contains(cursorPos);
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::updateEditMode()
{
	//if we grab corner dont change m_EditMode
	if (QApplication::mouseButtons() & Qt::LeftButton)
		return;

	if (m_Action == ACTION_EDIT && m_SelectedObject) //we are insert the scene object. Check the cursor position  
	{
		QRectF objectRect = m_SelectedObject->rect();
		QPoint mousePos = getMouseScenePos();
		
		if (isCursonInCorner(objectRect, mousePos, objectRect.topLeft()))			m_EditMode = EDIT_TOP_LEFT_CORNER;
		else if (isCursonInCorner(objectRect, mousePos, objectRect.topRight()))		m_EditMode = EDIT_TOP_RIGHT_CONRNER;
		else if (isCursonInCorner(objectRect, mousePos, objectRect.bottomLeft()))	m_EditMode = EDIT_BOTTOM_LEFT_CORNER;
		else if (isCursonInCorner(objectRect, mousePos, objectRect.bottomRight()))	m_EditMode = EDIT_BOTTOM_RIGHT_CORNER;
		else if (m_SelectedObject->rect().contains(mousePos))						m_EditMode = EDIT_MOVE;
		else
			m_EditMode = EDIT_DISABLE;
	}

	//set cursor and view drag mode
	switch (m_EditMode)
	{
	case EDIT_DISABLE:
		getView()->setDragMode(getView()->getScale() > 1 ? QGraphicsView::ScrollHandDrag : QGraphicsView::NoDrag);
		getView()->setCursor(getView()->getScale() > 1 ? Qt::OpenHandCursor : Qt::ArrowCursor);
		break;

	case EDIT_TOP_LEFT_CORNER:
	case EDIT_BOTTOM_RIGHT_CORNER:
		getView()->setDragMode(QGraphicsView::NoDrag);
		getView()->setCursor(Qt::SizeFDiagCursor);
		break;

	case EDIT_TOP_RIGHT_CONRNER:
	case EDIT_BOTTOM_LEFT_CORNER:
		getView()->setDragMode(QGraphicsView::NoDrag);
		getView()->setCursor(Qt::SizeBDiagCursor);
		break;

	case EDIT_MOVE:
		getView()->setDragMode(QGraphicsView::NoDrag);
		getView()->setCursor(Qt::SizeAllCursor);
		break;
	}
	
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::updateObjectSize()
{
	if (QApplication::mouseButtons() & Qt::LeftButton)
	{
		QPoint l_MousePos = getMouseScenePos();
		QPoint l_Offset = l_MousePos - m_CachedData.savedMousePos;
		QRect l_NewRect = m_SelectedObject->rect().toRect();
		
		switch (m_EditMode)
		{
			case EDIT_MOVE:
				l_NewRect = m_CachedData.savedObjRect;
				l_NewRect.translate(l_Offset);
				break;

			case EDIT_BOTTOM_RIGHT_CORNER:
				l_NewRect.setBottomRight(l_MousePos);
				break;

			case EDIT_BOTTOM_LEFT_CORNER:
				l_NewRect.setBottomLeft(l_MousePos);
				break;

			case EDIT_TOP_LEFT_CORNER:
				l_NewRect.setTopLeft(l_MousePos);
				break;

			case EDIT_TOP_RIGHT_CONRNER:
				l_NewRect.setTopRight(l_MousePos);
				break;	
		}


		if (l_NewRect.isValid())
			m_SelectedObject->setRect(l_NewRect);
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::setAction(EAction i_Action)
{
	if (i_Action != m_Action){
		m_Action = i_Action;
		
		//enter state
		switch (i_Action){
			case ACTION_EDIT:
				updateEditMode();
				break;

			case ACTION_CREATE:
				getView()->setDragMode(QGraphicsView::RubberBandDrag);
				break;

			case ACTION_NONE:
				getView()->setCursor(getView()->getScale() > 1 ? Qt::OpenHandCursor : Qt::ArrowCursor);
				getView()->setDragMode(getView()->getScale() > 1 ? QGraphicsView::ScrollHandDrag : QGraphicsView::NoDrag);
				break;
		}
	}
}

