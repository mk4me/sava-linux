#ifndef NODESELECTORWIDGET_H
#define NODESELECTORWIDGET_H

#include <QtWidgets/QWidget>
#include "ui_NodeSelectorWidget.h"

#include <vector>
#include <inttypes.h>

class NodeSelectorWidget : public QWidget
{
	Q_OBJECT

public:
	NodeSelectorWidget(QWidget *parent = 0, int maxNodes = 20);
	~NodeSelectorWidget();

	const std::vector<int64_t>& getNodes() const { return m_Nodes; }

	void setMaxNodes(int maxNodes) { m_MaxNodes = maxNodes; }

public slots:
	void selectNodes();
	void setNodes(const std::vector<int64_t>& nodes);
	void refreshList();

private:
	Ui::NodeSelectorWidget ui;

	std::vector<int64_t> m_Nodes;
	int m_MaxNodes;

	virtual void showEvent(QShowEvent *) override;
};

#endif // NODESELECTORWIDGET_H
