#include "EditorSceneContextMenu.h"
#include "QtCore/QDebug"
#include "EditorMessageManager.h"
#include "QtWidgets/QGraphicsView"
#include "EditorSceneLayerManager.h"

/*--------------------------------------------------------------------*/
EditorSceneContextMenu::EditorSceneContextMenu(QWidget* parent /*= 0*/)
	:QMenu(parent)
{

	m_LockEditAction = addAction("Lock to edit");
	m_ClearSelectionAction = addAction("Unselect objects");
	//m_MergeAction = addAction("Merge");

	m_LockEditAction->setData("lockEdit");
	m_ClearSelectionAction->setData("clearSelection");
	//m_MergeAction->setData("merge");

	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onTriggered(QAction*)));
	connect(this, SIGNAL(aboutToShow()), this, SLOT(onAboutToShow()));

	m_ObjectLayer = EditorSceneLayerHelper::getObjectLayer();
}

/*--------------------------------------------------------------------*/
EditorSceneContextMenu::~EditorSceneContextMenu(){
}

/*--------------------------------------------------------------------*/
void EditorSceneContextMenu::onTriggered(QAction* action)
{
	QString functionName = action->data().toString();
	QMetaObject::invokeMethod(this, functionName.toStdString().c_str());
	emit actionDone();
}

/*--------------------------------------------------------------------*/
void EditorSceneContextMenu::onAboutToShow()
{
	auto selectedIds = m_ObjectLayer->getSelectedObjectsIds();

	m_ClearSelectionAction->setEnabled(!selectedIds.empty());
	m_LockEditAction->setEnabled(selectedIds.size() == 1);
	m_LockEditAction->setText(( (selectedIds.size() == 1) && m_ObjectLayer->isEditLocked()) ? "Unlock" : "Lock to edit");
	//m_MergeAction->setEnabled(selectedIds.size() > 1);
}

/*--------------------------------------------------------------------*/
void EditorSceneContextMenu::lockEdit()
{
	if (m_ObjectLayer->isEditLocked())
		EditorMessageManager::sendMessage(MESSAGE_UNCLOCK_SCENE_OBJECT);
	else
		EditorMessageManager::sendMessage(MESSAGE_LOCK_SCENE_OBJECT);
}

/*--------------------------------------------------------------------*/
void EditorSceneContextMenu::clearSelection(){
	EditorMessageManager::sendMessage(MESSAGE_SCENE_UNSELECT_ALL);
}

/*--------------------------------------------------------------------*/
//void EditorSceneContextMenu::merge() {
//	EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_MERGE);
//}



