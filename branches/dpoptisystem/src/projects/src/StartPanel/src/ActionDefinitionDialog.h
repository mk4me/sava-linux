#ifndef ACTIONDEFINITIONDIALOG_H
#define ACTIONDEFINITIONDIALOG_H

#include <QtWidgets/QDialog>
#include "ui_ActionDefinitionDialog.h"

class ActionDefinitionDialog : public QDialog
{
	Q_OBJECT

public:
	ActionDefinitionDialog(QWidget *parent = 0);
	~ActionDefinitionDialog();

	QString getLabel() const { return ui.m_Label->text(); }
	void setLabel(const QString& label) { ui.m_Label->setText(label); }

	bool getTraining() const { return ui.m_Training->isChecked(); }
	void setTraining(bool training) { ui.m_Training->setChecked(training); }

private:
	Ui::ActionDefinitionDialog ui;
};

#endif // ACTIONDEFINITIONDIALOG_H
