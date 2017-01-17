#include "NodeSelectorDialog.h"

#include "config/Network.h"

NodeSelectorDialog::NodeSelectorDialog(QWidget *parent, int maxNodes)
	: QDialog(parent)
	, m_MaxNodes(maxNodes)
{
	ui.setupUi(this);
}

NodeSelectorDialog::~NodeSelectorDialog()
{

}

void NodeSelectorDialog::load(const std::vector<int64_t>& nodesList)
{
	config::Network& config = config::Network::getInstance();
	for (int i = 0; i < config.getNumNodes(); ++i)
		ui.m_NodesCombo->addItem(config.getName(i).c_str(), QVariant::fromValue(config.getId(i)));

	ui.m_NodesList->clear();
	ui.m_NodesList->blockSignals(true);
	for (int64_t id : nodesList)
	{
		QListWidgetItem* item = new QListWidgetItem(config.findName(id).c_str());
		item->setData(Qt::UserRole, QVariant::fromValue(id));
		ui.m_NodesList->addItem(item);
	}
	ui.m_NodesList->blockSignals(false);

	ui.m_AddButton->setEnabled(ui.m_NodesCombo->count() && ui.m_NodesList->count() < m_MaxNodes);
}

std::vector<int64_t> NodeSelectorDialog::getNodes() const
{
	std::vector<int64_t> nodes;
	for (int i = 0; i < ui.m_NodesList->count(); ++i)
		nodes.push_back(ui.m_NodesList->item(i)->data(Qt::UserRole).value<int64_t>());
	return nodes;
}

void NodeSelectorDialog::onCurrentRowChanged(int row)
{
	ui.m_RemoveButton->setEnabled(row >= 0);
}

void NodeSelectorDialog::addNode()
{
	QListWidgetItem* item = new QListWidgetItem(ui.m_NodesCombo->currentText());
	item->setData(Qt::UserRole, ui.m_NodesCombo->currentData());
	ui.m_NodesList->addItem(item);

	ui.m_AddButton->setEnabled(ui.m_NodesCombo->count() > 0 && ui.m_NodesList->count() < m_MaxNodes);

}

void NodeSelectorDialog::removeNode()
{
	if (ui.m_NodesList->currentRow() < 0)
		return;

	delete ui.m_NodesList->takeItem(ui.m_NodesList->currentRow());

	ui.m_AddButton->setEnabled(ui.m_NodesCombo->count() > 0 && ui.m_NodesList->count() < m_MaxNodes);
}
