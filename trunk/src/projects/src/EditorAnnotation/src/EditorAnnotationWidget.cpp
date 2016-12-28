#include "EditorAnnotationWidget.h"
#include "EditorWindow.h"
#include "EditorSequencesManager.h"
#include "EditorSequencesManager.h"

#include <QtWidgets/QSpacerItem>


/*--------------------------------------------------------------------*/
EditorAnnotationWidget::EditorAnnotationWidget(QWidget *parent)
: QWidget(parent)
{
	ui.setupUi(this);
	ui.m_RangeBeginButton->setEnabled(false);
	ui.m_RangeEndButton->setEnabled(false);
	ui.m_RemoveButton->setEnabled(false);

	m_ObjectLayer = EditorSceneLayerHelper::getObjectLayer();
	m_Sequence = EditorSequencesManager::getInstance().getSequence();

	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this);
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
		case MESSAGE_SEQUENCE_LOADED:
			ui.m_RangeBeginButton->setEnabled(false);
			ui.m_RangeEndButton->setEnabled(false);
			ui.m_RemoveButton->setEnabled(false);

		case MESSAGE_FRAME_CHANGED:
			repaint();
			break;

		case MESSAGE_SCENE_SELECTION_CHANGED:
			updateGui();
			break;
	}
}

void EditorAnnotationWidget::updateGui()
{
	std::set<size_t> selectedIds = m_ObjectLayer->getSelectedObjectsIds();
	bool isNotEditable = std::any_of(selectedIds.begin(), selectedIds.end(), [&](size_t id) {
		auto cluster = m_Sequence->getCluster(id);
		if (cluster)
			return !cluster->isEditable();
		return false;
	});

	bool enabled = !isNotEditable && !selectedIds.empty();
	ui.m_RangeBeginButton->setEnabled(enabled);
	ui.m_RangeEndButton->setEnabled(enabled);
	ui.m_RemoveButton->setEnabled(enabled);
}

/*--------------------------------------------------------------------*/
void EditorAnnotationWidget::onListClicked(const QModelIndex& index){
	m_ObjectLayer->setSelectedObjectById(index.internalId());
}

/*--------------------------------------------------------------------*/
void EditorAnnotationWidget::add(){
	m_ObjectLayer->addSceneObject();
}

void EditorAnnotationWidget::remove(){
	m_ObjectLayer->deleteSelectedObject();
}

/*--------------------------------------------------------------------*/
void EditorAnnotationWidget::rangeBegin()
{
	auto ids = m_ObjectLayer->getSelectedObjectsIds();
	if (!ids.empty())
	{
		int frameNr = EditorTimeline::getInstance().getCurrentFrame();
		for (size_t id: ids)
			EditorSequencesManager::getInstance().getSequence()->setClusterStartFrame(id, frameNr);

		EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_UPDATE);
	}
}

/*--------------------------------------------------------------------*/
void EditorAnnotationWidget::rangeEnd()
{
	auto ids = m_ObjectLayer->getSelectedObjectsIds();
	if (!ids.empty())
	{
		int frameNr = EditorTimeline::getInstance().getCurrentFrame();
		for (size_t id : ids)
			EditorSequencesManager::getInstance().getSequence()->setClusterEndFrame(id, frameNr);

		EditorMessageManager::sendMessage(MESSAGE_SCENE_OBJECT_UPDATE);
	}
}
