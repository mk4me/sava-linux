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

	m_LockEditAction->setData("lockEdit");
	m_ClearSelectionAction->setData("clearSelection");

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
	bool l_IsObjectSelected = (m_ObjectLayer->getSelectedObjectId() >=0 );

	m_ClearSelectionAction->setEnabled(l_IsObjectSelected);
	m_LockEditAction->setEnabled(l_IsObjectSelected);
	m_LockEditAction->setText((l_IsObjectSelected && m_ObjectLayer->isEditLocked()) ?  "Unlock" : "Lock to edit");

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


