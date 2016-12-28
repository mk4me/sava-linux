#include "MonitorConfig.h"
#include "AliasDefinitionDialog.h"

#include "config/Monitor.h"

MonitorConfig::MonitorConfig(QWidget *parent)
{
	ui.setupUi(this);
}

MonitorConfig::~MonitorConfig()
{

}

bool MonitorConfig::load()
{
	config::Monitor& monitorConfig = config::Monitor::getInstance();
	monitorConfig.load();

	ui.m_AlertsTime->setValue(monitorConfig.getAlertsLifetime());
	ui.m_MaxAlertsCount->setValue(monitorConfig.getMaxAlertsCount());
	ui.m_MinQueueSize->setValue(monitorConfig.getMinQueueSize());
	ui.m_MaxQueueSize->setValue(monitorConfig.getMaxQueueSize());
	ui.m_TimeScale->setValue((int)((monitorConfig.getTimeScale() - 1) * 100));

	ui.m_Aliases->setRowCount(0);
	const auto& aliases = monitorConfig.getAliases();
	for (auto alias : aliases)
		setAlias(ui.m_Aliases->rowCount(), alias.first.c_str(), alias.second.c_str());

	return true;
}

bool MonitorConfig::save()
{
	config::Monitor& monitorConfig = config::Monitor::getInstance();
	
	monitorConfig.setAlertsLifetime(ui.m_AlertsTime->value());
	monitorConfig.setMaxAlertsCount(ui.m_MaxAlertsCount->value());
	monitorConfig.setMinQueueSize(ui.m_MinQueueSize->value());
	monitorConfig.setMaxQueueSize(ui.m_MaxQueueSize->value());
	monitorConfig.setTimeScale(ui.m_TimeScale->value() / 100.0f + 1.0f);

	config::Monitor::AliasMap aliases;
	for (int i = 0; i < ui.m_Aliases->rowCount(); ++i)
		aliases.insert(std::make_pair(ui.m_Aliases->item(i, 0)->text().toStdString(), ui.m_Aliases->item(i, 1)->text().toStdString()));
	monitorConfig.setAliases(aliases);

	return config::Monitor::getInstance().save();
}

void MonitorConfig::onMinQueueChanged(int value)
{
	if (ui.m_MaxQueueSize->value() < value + 3)
		ui.m_MaxQueueSize->setValue(value + 3);
}

void MonitorConfig::onMaxQueueChanged(int value)
{
	if (ui.m_MinQueueSize->value() > value - 3)
		ui.m_MinQueueSize->setValue(value - 3);
}

void MonitorConfig::addAlias()
{
	AliasDefinitionDialog dialog;
	if (dialog.exec() == QDialog::Rejected)
		return;

	if (dialog.getAlias().isEmpty() || dialog.getBaseName().isEmpty())
		return;

	auto items = ui.m_Aliases->findItems(dialog.getBaseName(), Qt::MatchExactly);
	for (auto item : items)
	{
		if (item->column() == 0)
			return;
	}

	setAlias(ui.m_Aliases->rowCount(), dialog.getBaseName(), dialog.getAlias());
}

void MonitorConfig::editAlias()
{
	int index = ui.m_Aliases->currentRow();
	if (index < 0)
		return;

	AliasDefinitionDialog dialog;
	dialog.setFields(ui.m_Aliases->item(index, 0)->text(), ui.m_Aliases->item(index, 1)->text());
	if (dialog.exec() == QDialog::Rejected)
		return;

	if (dialog.getAlias().isEmpty() || dialog.getBaseName().isEmpty())
		return;

	auto items = ui.m_Aliases->findItems(dialog.getBaseName(), Qt::MatchExactly);
	for (auto item : items)
	{
		if (item->column() == 0 && item->row() != index)
			return;
	}

	setAlias(index, dialog.getBaseName(), dialog.getAlias());
}

void MonitorConfig::editAlias(int row, int column)
{
	editAlias();
}

void MonitorConfig::removeAlias()
{
	int index = ui.m_Aliases->currentRow();
	if (index >= 0)
		ui.m_Aliases->removeRow(index);
}

void MonitorConfig::onSelectionChanged(int currentRow, int currentCol, int prevRow, int prewCol)
{
	ui.m_EditAlias->setEnabled(currentRow >= 0);
	ui.m_RemoveAlias->setEnabled(currentRow >= 0);
}

void MonitorConfig::setAlias(int row, const QString& baseName, const QString& alias)
{
	ui.m_Aliases->blockSignals(true);

	if (ui.m_Aliases->rowCount() <= row)
		ui.m_Aliases->setRowCount(row + 1);

	setItem(row, 0, baseName);
	setItem(row, 1, alias);

	ui.m_Aliases->blockSignals(false);
}

void MonitorConfig::setItem(int row, int column, const QString& text)
{
	QTableWidgetItem* item = ui.m_Aliases->item(row, column);
	if (item == nullptr)
	{
		item = new QTableWidgetItem();
		ui.m_Aliases->setItem(row, column, item);
	}
	item->setText(text);
}