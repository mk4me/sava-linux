#ifndef NODESELECTORDIALOG_H
#define NODESELECTORDIALOG_H

#include <QtWidgets/QDialog>
#include "ui_NodeSelectorDialog.h"
#include <inttypes.h>
class NodeSelectorDialog : public QDialog
{
	Q_OBJECT

public:
	NodeSelectorDialog(QWidget *parent = 0, int maxNodes = 20);
	~NodeSelectorDialog();

	void load(const std::vector<int64_t>& nodesList);
	std::vector<int64_t> getNodes() const;

private slots:
	void onCurrentRowChanged(int row);
	
	void addNode();
	void removeNode();

private:
	Ui::NodeSelectorDialog ui;
	int m_MaxNodes;

};

#endif // NODESELECTORDIALOG_H
