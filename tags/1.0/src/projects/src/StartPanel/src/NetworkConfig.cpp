#include "NetworkConfig.h"
#include "NetworkDefinitionDialog.h"

#include "config/Network.h"

NetworkConfig::NetworkConfig(QWidget *parent)
{
	ui.setupUi(this);
}

NetworkConfig::~NetworkConfig()
{

}

bool NetworkConfig::load()
{
	config::Network& config = config::Network::getInstance();
	config.load();

	for (int i = 0; i < config.getNumNodes(); ++i)
		setNode(i, config.getName(i).c_str(), config.getIp(i).c_str());

	ui.m_Port->blockSignals(true);
	ui.m_Port->setValue(config.getTcpPort());
	ui.m_Port->blockSignals(false);

	return true;
}

bool NetworkConfig::save()
{
	return config::Network::getInstance().save();
}

void NetworkConfig::addNode()
{
	NetworkDefinitionDialog dialog;
	if (dialog.exec() == QDialog::Rejected)
		return;

	int index = config::Network::getInstance().addNode(dialog.getName().toStdString(), dialog.getIp().toStdString());
	if (index >= 0)
		setNode(index, dialog.getName(), dialog.getIp());
}

void NetworkConfig::editNode()
{
	config::Network& config = config::Network::getInstance();
	int index = ui.m_NodeList->currentRow();
	if (index < 0 || index >= config.getNumNodes())
		return;

	NetworkDefinitionDialog dialog;
	dialog.setFields(config.getName(index).c_str(), config.getIp(index).c_str());

	if (dialog.exec() == QDialog::Rejected)
		return;

	if (!config.setName(index, dialog.getName().toStdString())) return;
	config.setIp(index, dialog.getIp().toStdString());

	setNode(index, config.getName(index).c_str(),	config.getIp(index).c_str());
}

void NetworkConfig::editNode(int row, int column)
{
	editNode();
}

void NetworkConfig::removeNode()
{
	int index = ui.m_NodeList->currentRow();
	if (!config::Network::getInstance().removeNode(index))
		return;

	ui.m_NodeList->removeRow(index);
}

void NetworkConfig::onSelectionChanged(int currentRow, int currentCol, int prevRow, int prewCol)
{
	ui.m_EditButton->setEnabled(currentRow >= 0);
	ui.m_RemoveButton->setEnabled(currentRow >= 0);
}

void NetworkConfig::setPort(int port)
{
	config::Network::getInstance().setTcpPort(port);
}

void NetworkConfig::setNode(int row, const QString& name, const QString& ip)
{
	ui.m_NodeList->blockSignals(true);

	if (ui.m_NodeList->rowCount() <= row)
		ui.m_NodeList->setRowCount(row + 1);

	setItem(row, 0, name);
	setItem(row, 1, ip);

	ui.m_NodeList->blockSignals(false);
}

void NetworkConfig::setItem(int row, int column, const QString& text)
{
	QTableWidgetItem* item = ui.m_NodeList->item(row, column);
	if (item == nullptr)
	{
		item = new QTableWidgetItem();
		ui.m_NodeList->setItem(row, column, item);
	}
	item->setText(text);
}
