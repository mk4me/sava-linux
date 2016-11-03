#ifndef ALERTDIALOG_H
#define ALERTDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidgetItem>

#include "ui_AlertDialog.h"
#include "MonitorActionManager.h"

class AlertDialog : public QDialog
{
	Q_OBJECT

public:
	AlertDialog(QWidget *parent = 0);
	~AlertDialog();

	void init();


public slots:
	void onItemStateChanged(int state);
	void onSelectAllChanged(int state);

private:
	void select();
	void update_AllCheckBox();

private:
	Ui::AlertDialog ui;
};

#endif // ALERTDIALOG_H
