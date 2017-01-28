#include "EditorSceneObjectLayer.h"
#include "EditorGraphicsView.h"
#include "EditorSequencesManager.h"

#include <QtCore/QRect>
#include <QtGui/QCursor>
#include <QtWidgets/QGraphicsView>

/*--------------------------------------------------------------------*/
EditorSceneObjectLayer::EditorSceneObjectLayer(const EditorLayerPtr& i_SourceLayer)
:EditorSceneLayer(i_SourceLayer),
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
	m_ObjectMeasure = new EditorSceneObjectMeasure(this);
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::reload()
{
	setAction(ACTION_NONE);
	m_EditMode = EDIT_DISABLE;
	m_IsEditLocked = false;
	m_SelectedObjects.clear();

	deleteObjects();

	//create 
	auto clusters = EditorSequencesManager::getInstance().getSequence()->getClusters();
	for (auto pair : clusters)
		EditorSceneObject* newObj = createObject(pair->getUniqueId());
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::redraw()
{
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (!sequence)
		return;

	auto regions = sequence->getClusterFrames(EditorTimeline::getInstance().getCurrentFrame());

	//wyczy�� flagi
	for  (auto pair : m_SceneObjects) {
		EditorSceneObject* obj = pair.second;
		if (obj)
 			obj->setVisible(false);
	}

	//nadaj rozmiar i id-s obiektom na scenie
	for (auto it = regions.begin(); it != regions.end(); ++it)
	{
		EditorSceneObject* object = m_SceneObjects[it->first];
		if (!object)
			continue;
		//if (!object || (object != m_SelectedObject && m_IsEditLocked))
		//	continue;
	
		object->setRect(it->second.x, it->second.y, it->second.width, it->second.height);
		object->setVisible(true);
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::unselectAll(){
	for (auto& obj : m_SelectedObjects) {
		if (obj) {
			obj->setSelected(false);
			obj->updateRegion();
		}
	}

	m_SelectedObjects.clear();

	EditorMessageManager::sendMessage(MESSAGE_SCENE_SELECTION_CHANGED);
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::setSelectedObjectById(int i_Id, bool forceMultiselection /*= false*/){
	setSelectedObject(getObjectById(i_Id), forceMultiselection);
}

/*--------------------------------------------------------------------*/
size_t EditorSceneObjectLayer::getObjectsCount() const {
	return m_SceneObjects.size();
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::deleteSelectedObject()
{
	if (m_Action != ACTION_NONE)
		return;

	for (auto it = m_SelectedObjects.begin(); it != m_SelectedObjects.end();)
	{
		size_t id = (*it)->getId();
		if (EditorSequencesManager::getInstance().getSequence()->removeCluster(id))
		{
			it = m_SelectedObjects.erase(it);
			deleteObject(id);
		}
		else
		{
			it++;
		}
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

	EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_UPDATE);
}

/*--------------------------------------------------------------------*/
EditorSceneObject* EditorSceneObjectLayer::getOnlyOneSelectedObject() const
{
	if (m_SelectedObjects.size() == 1)
		return *m_SelectedObjects.begin();

	return nullptr;
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
	m_SceneObjects[i_Id] = object;

	return object;
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::deleteObject(size_t i_Id)
{
	auto foundIt = m_SceneObjects.find(i_Id);
	if (foundIt != m_SceneObjects.end())
	{
	
		foundIt->second->setParentItem(nullptr);
		delete foundIt->second;
		m_SceneObjects.erase(foundIt);

		EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_DELETED, i_Id);
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::setSelectedObject(EditorSceneObject* i_Obj, bool forceMultiselection)
{
	if (!i_Obj)
		return;

	if (m_IsEditLocked)
		return;

	//check if cluster can be selected
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	//EditorSequence::ClusterInfo clusterInfo = sequence->getClusterInfo(i_Obj->getId());
	//if (clusterInfo.hasNextPart || clusterInfo.hasPrevPart)
	//	return;
	

	//check if given object is already selected
	auto it = m_SelectedObjects.find(i_Obj);
	bool isSelected = (it != m_SelectedObjects.end());

	if (qApp->keyboardModifiers().testFlag(OBJECT_MULTI_SELECTION) || forceMultiselection) //multiselection mode
	{
		i_Obj->setSelected(!isSelected);
		i_Obj->updateRegion();

		//if it was selected, then remove it from selection
		if (isSelected)
			m_SelectedObjects.erase(it);
		else
			m_SelectedObjects.insert(i_Obj);

		EditorMessageManager::sendMessage(MESSAGE_SCENE_SELECTION_CHANGED);
	}
	else //single selection mode
	{
		if (!(isSelected && m_SelectedObjects.size() == 1))
		{
			for (auto& obj : m_SelectedObjects)
			{
				obj->setSelected(false);
				obj->updateRegion();
			}

			m_SelectedObjects.clear();
			m_SelectedObjects.insert(i_Obj);
			i_Obj->setSelected(true);
			i_Obj->updateRegion();

			EditorMessageManager::sendMessage(MESSAGE_SCENE_SELECTION_CHANGED);
		}
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
			//updateCluster(i_ExtraParam); //i_ExtraParam is next frame
			break;
	}
}

/*-------------------------------------------------------------------*/
std::vector<size_t> EditorSceneObjectLayer::getObjectIdsByCursorPos()
{
	QGraphicsView* view = getView();

	std::vector<size_t> ids;
	QPointF cursorScenePos = getMouseScenePos();
	for (auto pair : m_SceneObjects)
	{
		EditorSceneObject* obj = pair.second;
		if (obj && obj->rect().contains(cursorScenePos))
			ids.push_back(obj->getId());
	}	

	return ids;
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::addSceneObject(const QRectF& i_ObjRect /*= QRectF()*/)
{
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (sequence)
	{
		//set random rectangle object
		if (i_ObjRect.isNull())
			i_ObjRect = QRectF(10, 10, 100, 100);
			
		if (!i_ObjRect.isNull() && EditorTimeline::getInstance().getCurrentFrame() != sequence->getNumFrames() - 1)
		{
			size_t id = sequence->createCluster();
			addClusterRect(id, i_ObjRect.toRect(), -1, false); //begin rectangle
			addClusterRect(id, i_ObjRect.toRect(), sequence->getNumFrames() - 1, false); //end rectangle

			//add scene object
			EditorSceneObject* createdObj = createObject(id);

			setSelectedObject(createdObj); 
			EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_CREATED);
			EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_UPDATE);
		}
	}
}

std::set<size_t> EditorSceneObjectLayer::getSelectedObjectsIds() const
{
	std::set<size_t> ids;
	for (auto& obj : m_SelectedObjects)
		ids.insert(obj->getId());
	return ids;
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::updateCluster(int i_Time)
{
	

	if (!EditorTimeline::getInstance().isFrameValid(i_Time))
		return;

	EditorSceneObject* selectedObj = getOnlyOneSelectedObject();
	if (!selectedObj)
		return;
	
	if (selectedObj->isVisible() &&
		m_Action == ACTION_EDIT &&
		m_EditMode != EDIT_DISABLE)
	{
		addClusterRect(selectedObj->getId(), selectedObj->rect().toRect(), i_Time);
	}


	//ClusterPtr clu = EditorSequencesManager::getInstance().getSequence()->getCluster(selectedObj->getId());
	//for (auto& pos : clu->getFramesPositions())
		//qInfo() << QString("%1 (%2, %3)").arg(pos.first).arg(pos.second.x).arg(pos.second.y);
	
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::addClusterRect(size_t i_ClusterId, const QRect& i_Rect, int i_Time /*= -1*/, bool checkIfNotDuplicated /*= true*/)
{
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (sequence) {
		if (i_Time < 0)
			i_Time = EditorTimeline::getInstance().getCurrentFrame();
		i_Time = std::min(i_Time, EditorTimeline::getInstance().getFrameCount() - 1);

		if (checkIfNotDuplicated && i_Rect == sequence->getClusterFrame(i_ClusterId, i_Time))
			return;
		
		sequence->addClusterFrame(i_ClusterId, i_Rect, i_Time);
		EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_UPDATE);
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
	return (getOnlyOneSelectedObject() != nullptr) && (modifiers.testFlag(OBJECT_EDIT_MODIFY_KEY) || m_IsEditLocked);
}

/*--------------------------------------------------------------------*/
bool EditorSceneObjectLayer::canCreate(Qt::KeyboardModifiers modifiers) const{
	return modifiers.testFlag(OBJECT_CREATE_KEY) && !m_IsEditLocked;
}

////////////// TODO dave - poni�szy kod do osobnego obiektu (i zrobic porz�dek) ///////////////////////////////////////////


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
		EditorSceneObject* selectedObj = getOnlyOneSelectedObject();

		m_CachedData.savedMousePos = getMouseScenePos();
		m_CachedData.savedObjRect = selectedObj ? selectedObj->rect().toRect() : QRect();
		m_CachedData.savedObj = selectedObj;
	}

	EditorSceneLayer::mousePressEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::mouseRelease(QGraphicsSceneMouseEvent *event)
{
	//tworzymy nowy prostok�t/cluster je�li bylismy w akcji tworzenia
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

	EditorSceneObject* selectedObj = getOnlyOneSelectedObject();

	if (m_Action == ACTION_EDIT && selectedObj) //we are insert the scene object. Check the cursor position  
	{
		QRectF objectRect = selectedObj->rect();
		QPoint mousePos = getMouseScenePos();
		
		if (isCursonInCorner(objectRect, mousePos, objectRect.topLeft()))			m_EditMode = EDIT_TOP_LEFT_CORNER;
		else if (isCursonInCorner(objectRect, mousePos, objectRect.topRight()))		m_EditMode = EDIT_TOP_RIGHT_CONRNER;
		else if (isCursonInCorner(objectRect, mousePos, objectRect.bottomLeft()))	m_EditMode = EDIT_BOTTOM_LEFT_CORNER;
		else if (isCursonInCorner(objectRect, mousePos, objectRect.bottomRight()))	m_EditMode = EDIT_BOTTOM_RIGHT_CORNER;
		else if (selectedObj->rect().contains(mousePos))						m_EditMode = EDIT_MOVE;
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

	m_ObjectMeasure->setObject(m_EditMode == EDIT_DISABLE ? nullptr : getOnlyOneSelectedObject());
	m_ObjectMeasure->update();
	
}

/*--------------------------------------------------------------------*/
void EditorSceneObjectLayer::updateObjectSize()
{
	if (QApplication::mouseButtons() & Qt::LeftButton)
	{
		EditorSceneObject* selectedObj = getOnlyOneSelectedObject();

		QPoint l_MousePos = getMouseScenePos();
		QPoint l_Offset = l_MousePos - m_CachedData.savedMousePos;
		QRect l_NewRect = selectedObj->rect().toRect();
		
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
		{
			selectedObj->setRect(l_NewRect);
			m_ObjectMeasure->update();
		}
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

				m_ObjectMeasure->setObject(nullptr);
				break;
		}
	}
}

