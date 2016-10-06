#include "EditorPropertyWidget.h"

/*--------------------------------------------------------------------*/
EditorPropertyWidget::EditorPropertyWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	initAction();
	initActionSplit();
	setEnabled(false);

	EditorMessageManager::registerMessage(MESSAGE_SCENE_SELECTION_CHANGED, this);
}

/*--------------------------------------------------------------------*/
EditorPropertyWidget::~EditorPropertyWidget(){
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorPropertyWidget::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	if (i_Message == MESSAGE_SCENE_SELECTION_CHANGED)
	{
		block(true);
		updateGui();
		block(false);
	}
}

/*--------------------------------------------------------------------*/
void EditorPropertyWidget::initAction()
{
	// add no action
	ui.actionComboBox->addItem("NO ACTION", -1);

	utils::Glossary::getInstance().load();
	std::vector<std::string> actions = utils::Glossary::getInstance().getAnnotations();
	for (int i = 0; i < actions.size(); i++){
		ui.actionComboBox->addItem(actions[i].c_str(), i);
	}
}

/*--------------------------------------------------------------------*/
void EditorPropertyWidget::initActionSplit()
{
	ui.actionSplitComboBox->addItem("GENERATED", Action::Split::GENERATED);
	ui.actionSplitComboBox->addItem("TEST", Action::Split::TRAIN);
	ui.actionSplitComboBox->addItem("TRAIN", Action::Split::TEST);

	ui.actionSplitComboBox->setEnabled(false);
}

/*--------------------------------------------------------------------*/
void EditorPropertyWidget::onActionSplitChanged(int index)
{
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (sequence)
	{
		//get selected cluster id
		//TODO current we take selected id from scene. Change it. Store it in some model class
		int selectedClusterId = EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectId();
		if (selectedClusterId >= 0)
		{
			Action::Split split = static_cast<Action::Split>(ui.actionSplitComboBox->itemData(index).toInt());
			sequence->getAction(selectedClusterId)->setSplit(split);
		}
	}
}

/*--------------------------------------------------------------------*/
void EditorPropertyWidget::onActionChanged(int index)
{
	int actionId = ui.actionComboBox->itemData(index).toInt();

	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (sequence)
	{
		//get selected cluster id
		//TODO current we take selected id from scene. Change it. Store it in some model class
		int selectedClusterId = EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectId();
		if (selectedClusterId >= 0)
		{
			sequence->getAction(selectedClusterId)->setActionId(actionId);
			EditorMessageManager::sendMessage(MESSAGE_ADD_ACTION);
		}
	}

	ui.actionSplitComboBox->setEnabled(actionId >= 0);
}

/*--------------------------------------------------------------------*/
void EditorPropertyWidget::updateGui()
{
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (sequence)
	{
		//get selected cluster id
		//TODO current we take selected id from scene. Change it. Store it in some model class
		int selectedClusterId = EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectId();
		if (selectedClusterId >= 0)
		{
			//set cluster widgets
			ClusterPtr cluster = sequence->getCluster(selectedClusterId);
			assert(cluster);

			ui.idLineEdit->setText(QString::number(selectedClusterId));
			ui.clusterIdLineEdit->setText(QString::number(cluster->getClusterId()));

			//set action widgets
			ActionPtr action = sequence->getAction(selectedClusterId);
			for (int i = 0; i < ui.actionComboBox->count(); i++)
				if (ui.actionComboBox->itemData(i) == action->getActionId()){
					ui.actionComboBox->setCurrentIndex(i);
					break;
				}

			bool noActionSelected = (ui.actionComboBox->currentData().toInt() < 0);
			if (!noActionSelected)
				for (int i = 0; i < ui.actionSplitComboBox->count(); i++)
					if (ui.actionSplitComboBox->itemData(i) == action->getSplit()){
						ui.actionSplitComboBox->setCurrentIndex(i);
						break;
					}

			ui.actionSplitComboBox->setEnabled(!noActionSelected);


			setEnabled(true);

		}
		else
		{
			clearGui();
			setEnabled(false);
		}
	}	
}


/*--------------------------------------------------------------------*/
void EditorPropertyWidget::clearGui()
{
	ui.idLineEdit->clear();
	ui.clusterIdLineEdit->clear();
	ui.actionComboBox->setCurrentIndex(0);
	ui.actionSplitComboBox->setCurrentIndex(0);
}

/*--------------------------------------------------------------------*/
void EditorPropertyWidget::block(bool _blockEnabled)
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
