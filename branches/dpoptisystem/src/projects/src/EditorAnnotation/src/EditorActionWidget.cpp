#include "EditorActionWidget.h"
#include "config/Glossary.h"

/*--------------------------------------------------------------------*/
EditorActionWidget::EditorActionWidget(QWidget *parent)
	: QWidget(parent)
	, m_CurrentCluster(nullptr)
	, m_CurrentAction(nullptr)
{
	ui.setupUi(this);

	initAction();
	initActionSplit();
	setEnabled(false);

	m_Scene = new EditorActionScene(ui.graphicsView, this);

	connect(m_Scene, SIGNAL(actionChanged()), this, SLOT(onSceneActionChanged()));

	EditorMessageManager::registerMessage(MESSAGE_SCENE_SELECTION_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_OBJECT_UPDATE, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_OBJECT_DELETED, this);
	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);
}

/*--------------------------------------------------------------------*/
EditorActionWidget::~EditorActionWidget(){
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	if (i_Message == MESSAGE_SCENE_SELECTION_CHANGED || 
		i_Message == MESSAGE_SCENE_OBJECT_UPDATE || 
		i_Message == MESSAGE_SEQUENCE_LOADED ||
		i_Message == MESSAGE_SCENE_OBJECT_DELETED)
	{
		if (i_Message == MESSAGE_SEQUENCE_LOADED) {
			m_Sequence = EditorSequencesManager::getInstance().getSequence();
			assert(m_Sequence);
		}

		auto selectedIds = EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectsIds();
		int selectedId = (selectedIds.size() == 1) ? *selectedIds.begin() : -1;

		m_CurrentCluster = m_Sequence->getCluster(selectedId);

		setEnabled(m_CurrentCluster != nullptr);
		m_Scene->setCluster(m_CurrentCluster);
		m_Scene->refresh();

		if (m_CurrentCluster)
		{
			block(true);
			updateGui();
			block(false);
		}
	}
	else if (i_Message == MESSAGE_FRAME_CHANGED)
	{
		updateClusterGui();
	}
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::initAction()
{
	// add no action
	ui.actionComboBox->addItem("NO ACTION", -1);

	for (auto& action : config::Glossary::getInstance().getEditorActions()){
		ui.actionComboBox->addItem(action.name.c_str(), action.id);
	}
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::initActionSplit()
{
	ui.actionSplitComboBox->addItem("UNASSIGNED", Action::Split::UNASSIGNED);
	ui.actionSplitComboBox->addItem("TRAIN", Action::Split::TRAIN);
	ui.actionSplitComboBox->addItem("TEST", Action::Split::TEST);
	ui.actionSplitComboBox->addItem("GENERATED", Action::Split::GENERATED);

	ui.actionSplitComboBox->setEnabled(false);
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::onActionSplitChanged(int index)
{
	if (m_CurrentAction)
	{
		//get selected cluster id
		//TODO current we take selected id from scene. Change it. Store it in some model class
		Action::Split split = static_cast<Action::Split>(ui.actionSplitComboBox->itemData(index).toInt());

		m_CurrentAction->getSource()->setSplit(split);
		m_Sequence->modified();
	}
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::onActionChanged(int index)
{
	int actionId = ui.actionComboBox->itemData(index).toInt();

	if (m_CurrentAction)
	{
		m_CurrentAction->getSource()->setActionId(actionId);
		m_Sequence->modified();	
		m_Scene->update();

		EditorMessageManager::sendMessage(MESSAGE_ACTION_CHANGED);
	}

	ui.actionSplitComboBox->setEnabled(actionId >= 0);
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::onSplitClicked()
{
	if (m_CurrentCluster)
	{	
		size_t frame_nr = EditorTimeline::getInstance().getCurrentFrame();
		m_CurrentCluster->splitActions(frame_nr);
		m_Sequence->modified();
		m_Scene->refresh(m_CurrentAction);

		EditorMessageManager::sendMessage(MESSAGE_ACTION_CHANGED);
	}
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::onDeleteClicked()
{
	if (m_CurrentCluster && m_CurrentAction)
	{
		m_CurrentCluster->removeAction(m_CurrentAction);
		m_CurrentAction = nullptr;

		m_Scene->refresh();
		m_Sequence->modified();
		updateGui();

		EditorMessageManager::sendMessage(MESSAGE_ACTION_CHANGED);
	}
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::onBeginClicked()
{
	if (m_CurrentCluster && m_CurrentAction)
	{
		size_t frame_nr = EditorTimeline::getInstance().getCurrentFrame();
		m_CurrentCluster->setActionBeginTime(m_CurrentAction, frame_nr);
		m_Sequence->modified();
		m_Scene->refresh(m_CurrentAction);

		EditorMessageManager::sendMessage(MESSAGE_ACTION_CHANGED);
	}
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::onEndClicked()
{
	if (m_CurrentCluster && m_CurrentAction)
	{
		size_t frame_nr = EditorTimeline::getInstance().getCurrentFrame();
		m_CurrentCluster->setActionEndTime(m_CurrentAction, frame_nr);
		m_Sequence->modified();
		m_Scene->refresh(m_CurrentAction);

		EditorMessageManager::sendMessage(MESSAGE_ACTION_CHANGED);
	}
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::onSceneActionChanged()
{
	m_CurrentAction = m_Scene->selectedAction();
	updateGui();
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::updateClusterGui()
{
	if (m_CurrentCluster)
	{
		//float time = EditorTimeline::getInstance().getCurrentFrame();
		//cv::Rect rect = m_CurrentCluster->getSource()->getFrameAt(time);

		//QString clusterId = QString::number(m_CurrentCluster->getSource()->getClusterId());
		//QString clusterCoords = QString(" (%1,%2) %3x%4").arg(rect.x).arg(rect.y).arg(rect.width).arg(rect.height);

		//ui.clusterIdLineEdit->setText(clusterId);
		//ui.clusterCoordsLineEdit->setText(clusterCoords);
		
	}
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::clearClusterGui()
{
	//ui.clusterIdLineEdit->clear();
	//ui.clusterCoordsLineEdit->clear();
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::updateGui()
{
	if (m_CurrentCluster && m_CurrentAction)
	{
		ClusterPtr cluster = m_CurrentCluster->getSource();

		//set action widgets
		bool foundItem = false;
		ActionPtr action = m_CurrentAction->getSource();
		for (int i = 0; i < ui.actionComboBox->count(); i++)
			if (ui.actionComboBox->itemData(i) == action->getActionId()){
				ui.actionComboBox->setCurrentIndex(i);
				foundItem = true;
				break;
			}

		if (!foundItem)
			ui.actionComboBox->setCurrentIndex(0);

		bool noActionSelected = (ui.actionComboBox->currentData().toInt() < 0);
		if (!noActionSelected)
			for (int i = 0; i < ui.actionSplitComboBox->count(); i++)
				if (ui.actionSplitComboBox->itemData(i) == action->getSplit()){
					ui.actionSplitComboBox->setCurrentIndex(i);
					break;
				}

		ui.actionSplitComboBox->setEnabled(!noActionSelected);
		if (noActionSelected)
			ui.actionSplitComboBox->setCurrentIndex(0);

		updateClusterGui();

		setEnabled(true);
	}
	else
	{
		clearGui();
		setEnabled(false);
	}
}


/*--------------------------------------------------------------------*/
void EditorActionWidget::clearGui()
{
	ui.actionComboBox->setCurrentIndex(0);
	ui.actionSplitComboBox->setCurrentIndex(0);
	clearClusterGui();
}

/*--------------------------------------------------------------------*/
void EditorActionWidget::block(bool _blockEnabled)
{
	if (_blockEnabled)
	{
		disconnect(ui.actionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onActionChanged(int)));
		disconnect(ui.actionSplitComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onActionSplitChanged(int)));
	}
	else
	{
		connect(ui.actionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onActionChanged(int)));
		connect(ui.actionSplitComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onActionSplitChanged(int)));
	}
}

void EditorActionWidget::resizeEvent(QResizeEvent *e)
{
	m_Scene->refresh();
	QWidget::resizeEvent(e);
}
