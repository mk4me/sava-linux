#include "ActionConfig.h"

#include <QFileDialog>

ActionConfig::ActionConfig(QWidget *parent)
{
	ui.setupUi(this);
}

ActionConfig::~ActionConfig()
{

}

bool ActionConfig::load()
{
	m_Config.load();

	ui.m_DatabasePath->blockSignals(true);
	ui.m_DatabasePath->setText(m_Config.getDatabasePath().c_str());
	ui.m_DatabasePath->blockSignals(false);

	ui.m_DescType->blockSignals(true);
	ui.m_DescType->setCurrentIndex(m_Config.getDescriptorType() - 1);
	ui.m_DescType->blockSignals(false);

	return true;
}

bool ActionConfig::save()
{
	return m_Config.save();
}

void ActionConfig::selectDatabasePath()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select database path");
	if (!path.isEmpty())
	{
		ui.m_DatabasePath->setText(path);
		databasePathChanged();
	}
}

void ActionConfig::databasePathChanged()
{
	m_Config.setDatabasePath(ui.m_DatabasePath->text().toStdString());
}

void ActionConfig::setDescriptorType(int type)
{
	m_Config.setDescriptorType((config::Action::DescriptorType)(type + 1));
}
