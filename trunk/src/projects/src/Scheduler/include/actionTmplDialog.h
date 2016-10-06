#ifndef ACTIONTMPLDIALOG_H
#define ACTIONTMPLDIALOG_H

#include "QtWidgets/QDialog"
#include "QtCore/QMetaType"
#include "ui_actionTmplDialog.h"

#include "scheduler.h"

#include <vector>

class ActionTmplDialog : public QDialog, public Ui::actionTmplDialog
{
	Q_OBJECT
	
public:
	ActionTmplDialog(QWidget *parent = 0);
	~ActionTmplDialog();

public:
	QStringList getTemplateNames() const;

private slots:
	void addTemplate();
	void removeTemplate();
	void currentTemplateChanged(QListWidgetItem* current, QListWidgetItem* previous);

	void apply();
};

Q_DECLARE_METATYPE(aq_qtutils::CAction)

#endif // ACTIONTMPLDIALOG_H
