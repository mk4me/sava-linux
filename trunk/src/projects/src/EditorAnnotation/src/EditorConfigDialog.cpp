#include "EditorConfigDialog.h"

/*--------------------------------------------------------------------*/
EditorConfigDialog::EditorConfigDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_Config = EditorConfig::getPointer();

	connect(ui.trajectoryPartsWidget, SIGNAL(dataChanged()), this, SLOT(onTrajectoryPartsDataChanged()));

	//update gui from config
	bool boundingBoxVisible = m_Config->isSceneObjectsBoundingBoxVisibled();
	bool labelVisible = m_Config->isSceneObjectsLabelVisibled();
	ui.showObjectsBoundingBoxesCheckBox->setCheckState(boundingBoxVisible ? Qt::Checked : Qt::Unchecked);
	ui.showObjectsLabelsCheckBox->setCheckState(labelVisible ? Qt::Checked : Qt::Unchecked);
	ui.showObjectsCheckBox->setChecked((boundingBoxVisible || labelVisible) ? Qt::Checked : Qt::Unchecked);

	ui.showTrajectoryCheckBox->setChecked(m_Config->isSceneTrajectoryVisibled() ? Qt::Checked : Qt::Unchecked);

	ui.m_AutoSaveIntervalSpinBox->setValue(m_Config->getAutoSaveTimerInterval() / (1000 * 60));
}

/*--------------------------------------------------------------------*/
EditorConfigDialog::~EditorConfigDialog()
{
	
}

/*--------------------------------------------------------------------*/
void EditorConfigDialog::onShowObjectsChanged(int state)
{
	ui.showObjectsBoundingBoxesCheckBox->setCheckState(ui.showObjectsCheckBox->checkState());
	ui.showObjectsLabelsCheckBox->setCheckState(ui.showObjectsCheckBox->checkState());
}

/*--------------------------------------------------------------------*/
void EditorConfigDialog::onShowObjectsBoundingBoxesChanged(int state){
	m_Config->setSceneObjectsBoundingBoxVisibled(state == Qt::Checked);
}

/*--------------------------------------------------------------------*/
void EditorConfigDialog::onShowObjectsLabelsChanged(int state){
	m_Config->setSceneObjectsLabelVisibled(state == Qt::Checked);
}

/*--------------------------------------------------------------------*/
void EditorConfigDialog::onShowTrajecoriesChanged(int state){
	m_Config->setSceneTrajectoryVisibled(state == Qt::Checked);
}

/*--------------------------------------------------------------------*/
void EditorConfigDialog::onTrajectoryPartsDataChanged(){
	EditorMessageManager::sendMessage(MESSAGE_CONFIG_CHANGED);
}

/*--------------------------------------------------------------------*/
void EditorConfigDialog::onAutoSaveEnabledChanged(){
	bool checked = ui.m_AutoSaveEnabledCheckBox->isChecked();

	ui.m_AutoSaveIntervalLayout->setEnabled(checked);
	m_Config->setAutoSaveEnabled(checked);

	EditorMessageManager::sendMessage(MESSAGE_CONFIG_CHANGED);
}

/*--------------------------------------------------------------------*/
void EditorConfigDialog::onAutoSaveIntervalChanged(int value)
{
	m_Config->setAutoSaveTimerInterval(value * 60 * 1000);
	EditorMessageManager::sendMessage(MESSAGE_CONFIG_CHANGED);
}
