#ifndef ACTIONDEFINITIONWIDGET_H
#define ACTIONDEFINITIONWIDGET_H

#include "QtWidgets/QWidget"
#include "ui_actionDefinitionWidget.h"

#include "scheduler.h"

class ActionDefinitionWidget : public QWidget, public Ui::actionDefinitionWidget
{
	Q_OBJECT

public:
	ActionDefinitionWidget(QWidget *parent = 0);
	~ActionDefinitionWidget();

	const aq_qtutils::CAction& getAction() const { return m_Action; }
	void setAction(const aq_qtutils::CAction& action);

private slots:
	void addProcess();
	void duplicateProcess();
	void removeProcess();
	void currentProcessChanged(QListWidgetItem* current, QListWidgetItem* previous);

	void addParameter();
	void removeParameter();
	void parameterChanged(int row, int column);
	void currentParameterChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	void machineIpChanged();

private:
	aq_qtutils::CAction m_Action;
};

#endif // ACTIONDEFINITIONWIDGET_H
