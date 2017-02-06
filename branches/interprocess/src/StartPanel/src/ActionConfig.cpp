#include "ActionConfig.h"
#include "ActionDefinitionDialog.h"

#include "config/Action.h"

#include <QtWidgets/QFileDialog>

#include <iostream>

ActionConfig::ActionConfig(QWidget *parent)
{
	ui.setupUi(this);
}

ActionConfig::~ActionConfig()
{

}

bool ActionConfig::load()
{
	config::Action& actionConfig = config::Action::getInstance();
	actionConfig.load();

	ui.m_DescType->blockSignals(true);
	ui.m_DescType->setCurrentIndex(actionConfig.getDescriptorType() - 1);
	ui.m_DescType->blockSignals(false);

	ui.m_MinFrames->blockSignals(true);
	ui.m_MinFrames->setValue(actionConfig.getMinVideoFrames());
	ui.m_MinFrames->blockSignals(false);

	ui.m_TrainLength->blockSignals(true);
	ui.m_TrainLength->setValue(actionConfig.getTrainSetLength());
	ui.m_TrainLength->blockSignals(false);

	ui.m_TestLength->blockSignals(true);
	ui.m_TestLength->setValue(actionConfig.getTestSetLength());
	ui.m_TestLength->blockSignals(false);

	ui.m_MaxVideoWidth->blockSignals(true);
	ui.m_MaxVideoWidth->setValue(actionConfig.getMaxVideoWidth());
	ui.m_MaxVideoWidth->blockSignals(false);

	ui.m_MaxVideoHeight->blockSignals(true);
	ui.m_MaxVideoHeight->setValue(actionConfig.getMaxVideoHeight());
	ui.m_MaxVideoHeight->blockSignals(false);

	config::Glossary& glossaryConfig = config::Glossary::getInstance();
	glossaryConfig.load();

	ui.m_Glossary->clear();
	ui.m_Glossary->blockSignals(true);
	for (auto& action : glossaryConfig.getEditorActions())
		ui.m_Glossary->addItem(createItem(action));
	ui.m_Glossary->blockSignals(false);

	return true;
}

bool ActionConfig::save()
{
	config::Glossary& glossaryConfig = config::Glossary::getInstance();
	bool success = true;
	success &= config::Action::getInstance().save();
	success &= glossaryConfig.save();

	return success;
}

void ActionConfig::setDescriptorType(int type)
{
	config::Action::getInstance().setDescriptorType((config::Action::DescriptorType)(type + 1));
}

void ActionConfig::setMinFrames(int minFrames)
{
	config::Action::getInstance().setMinVideoFrames(minFrames);
}

void ActionConfig::setTrainLength(int length)
{
	config::Action::getInstance().setTrainSetLength(length);
}

void ActionConfig::setTestLength(int length)
{
	config::Action::getInstance().setTestSetLength(length);
}

void ActionConfig::setMaxVideoWidth(int width)
{
	config::Action::getInstance().setMaxVideoWidth(width);
}

void ActionConfig::setMaxVideoHeight(int height)
{
	config::Action::getInstance().setMaxVideoHeight(height);
}

void ActionConfig::onGlossarySelChanged(int row)
{
	ui.m_Edit->setEnabled(row >= 0);
	ui.m_Remove->setEnabled(row >= 0);
}

void ActionConfig::editGlossaryItem()
{
	editGlossaryItem(ui.m_Glossary->currentItem());
}

void ActionConfig::editGlossaryItem(QListWidgetItem* item)
{
	if (item == nullptr)
		return;

	ActionDefinitionDialog dialog;
	dialog.setLabel(item->text());
	dialog.setTraining(item->data(Qt::UserRole + 1).toBool());
	if (dialog.exec() == QDialog::Rejected)
		return;

	if (dialog.getLabel().isEmpty())
		return;

	if (!config::Glossary::getInstance().modifyEditorAction(
		item->data(Qt::UserRole).toInt(),
		dialog.getLabel().toStdString(),
		dialog.getTraining()))
		return;

	setItem(item, dialog.getLabel(), dialog.getTraining());
}

void ActionConfig::addGlossaryItem()
{
	ActionDefinitionDialog dialog;
	dialog.setTraining(true);
	if (dialog.exec() == QDialog::Rejected)
		return;

	int id = config::Glossary::getInstance().addEditorAction(dialog.getLabel().toStdString(), dialog.getTraining());
	if (id >= 0)
		ui.m_Glossary->addItem(createItem(config::Glossary::Action(dialog.getLabel().toStdString(), dialog.getTraining(), id)));
}

void ActionConfig::removeGlossaryItem()
{
	QListWidgetItem* item = ui.m_Glossary->currentItem();
	if (item == nullptr)
		return;

	if(config::Glossary::getInstance().removeEditorAction(item->data(Qt::UserRole).toInt()))
		delete ui.m_Glossary->takeItem(ui.m_Glossary->currentRow());
}

QListWidgetItem* ActionConfig::createItem(const config::Glossary::Action& action)
{
	QListWidgetItem* item = new QListWidgetItem();
	item->setData(Qt::UserRole, QVariant::fromValue(action.id));
	setItem(item, action.name.c_str(), action.train);

	return item;
}

void ActionConfig::setItem(QListWidgetItem* item, const QString& name, bool training)
{
	item->setText(name);
	item->setData(Qt::UserRole + 1, QVariant::fromValue(training));

	QFont font = item->font();
	font.setItalic(!training);
	item->setFont(font);

	QBrush brush = item->foreground();
	brush.setColor(training ? Qt::black : Qt::darkGray);
	item->setForeground(brush);
}
