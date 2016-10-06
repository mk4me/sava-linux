#include "EditorAnnotationWidget.h"
#include "EditorWindow.h"
#include "include\EditorSequencesManager.h"

/*--------------------------------------------------------------------*/
EditorAnnotationWidget::EditorAnnotationWidget(QWidget *parent)
: QWidget(parent)
{
	ui.setupUi(this);
	ui.m_RangeBeginButton->setEnabled(false);
	ui.m_RangeEndButton->setEnabled(false);

	m_ObjectLayer = EditorSceneLayerHelper::getObjectLayer();

	EditorMessageManager::registerMessage(MESSAGE_SCENE_SELECTION_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);
}

/*--------------------------------------------------------------------*/
EditorAnnotationWidget::~EditorAnnotationWidget()
{
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorAnnotationWidget::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
		case MESSAGE_FRAME_CHANGED:
			repaint();
			break;

		case MESSAGE_SCENE_SELECTION_CHANGED:
			bool isSelected = (m_ObjectLayer->getSelectedObjectId() >= 0);
			ui.m_RangeBeginButton->setEnabled(isSelected);
			ui.m_RangeEndButton->setEnabled(isSelected);
			break;
	}
}

/*--------------------------------------------------------------------*/
void EditorAnnotationWidget::onListClicked(const QModelIndex& index){
	m_ObjectLayer->setSelectedObjectById(index.internalId());
}

/*--------------------------------------------------------------------*/
void EditorAnnotationWidget::onAdd(){
	m_ObjectLayer->addSceneObject();
}

/*--------------------------------------------------------------------*/
void EditorAnnotationWidget::onRangeBegin()
{
	size_t id = m_ObjectLayer->getSelectedObjectId();
	if (id >= 0)
	{
		int frameNr = EditorTimeline::getInstance().getCurrentFrame();
		EditorSequencesManager::getInstance().getSequence()->setClusterStartFrame(id, frameNr);
		EditorMessageManager::sendMessage(MESSAGE_SCENE_UPDATE);
	}
}

/*--------------------------------------------------------------------*/
void EditorAnnotationWidget::onRangeEnd()
{
	size_t id = m_ObjectLayer->getSelectedObjectId();
	if (id >= 0)
	{
		int frameNr = EditorTimeline::getInstance().getCurrentFrame();
		EditorSequencesManager::getInstance().getSequence()->setClusterEndFrame(id, frameNr);
		EditorMessageManager::sendMessage(MESSAGE_SCENE_UPDATE);
	}
}



