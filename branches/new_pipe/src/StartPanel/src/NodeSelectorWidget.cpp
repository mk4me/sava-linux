#include "NodeSelectorWidget.h"
#include "NodeSelectorDialog.h"

#include "config/Network.h"

NodeSelectorWidget::NodeSelectorWidget(QWidget *parent, int maxNodes)
	: QWidget(parent)
	, m_MaxNodes(maxNodes)
{
	ui.setupUi(this);
}

NodeSelectorWidget::~NodeSelectorWidget()
{

}

void NodeSelectorWidget::selectNodes()
{
	NodeSelectorDialog dialog(nullptr, m_MaxNodes);
	dialog.load(m_Nodes);
	if (dialog.exec() == QDialog::Accepted)
		setNodes(dialog.getNodes());
}

void NodeSelectorWidget::setNodes(const std::vector<int64_t>& nodes)
{
	m_Nodes = nodes;
	refreshList();
}

void NodeSelectorWidget::refreshList()
{
	QString viewText;
	QString tooltipText;

	for (const int64_t id : m_Nodes)
	{
		const std::string& nodeName = config::Network::getInstance().findName(id);
		viewText += QString("; ") + nodeName.c_str();
		tooltipText += QString("\n") + nodeName.c_str();
	}

	ui.m_NodeList->setText(viewText.mid(2));
	ui.m_NodeList->setToolTip(tooltipText.mid(1));
}

void NodeSelectorWidget::showEvent(QShowEvent *)
{
	refreshList();
}
