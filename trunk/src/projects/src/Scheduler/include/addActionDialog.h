#ifndef ADDACTIONDIALOG_H
#define ADDACTIONDIALOG_H

#include "QtWidgets/QDialog"
#include "ui_addActionDialog.h"

class AddActionDialog : public QDialog, public Ui::AddActionDialog
{
	Q_OBJECT

public:
	AddActionDialog(QWidget* parent = 0);
	AddActionDialog(QWidget* parent, const QStringList& templateNames);
	AddActionDialog(QWidget* parent, const QString& actionName);
	~AddActionDialog();

	QString getActionName() const { return m_ActionName->text(); }
	QString getTemplateName() const { return m_TemplateName->currentText(); }

private slots:
	void manageTemplates();
};

#endif // ADDACTIONDIALOG_H
