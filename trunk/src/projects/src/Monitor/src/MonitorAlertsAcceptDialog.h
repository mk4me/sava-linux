#ifndef MONITORALERTSACCEPTDIALOG_H
#define MONITORALERTSACCEPTDIALOG_H

#include <QtWidgets/QDialog>
#include "ui_MonitorAlertsAcceptDialog.h"

class MonitorAlertsAcceptDialog : public QDialog
{
	Q_OBJECT

public:
	MonitorAlertsAcceptDialog(QWidget *parent = 0);
	~MonitorAlertsAcceptDialog();

private:
	Ui::MonitorAlertsAcceptDialog ui;
};

#endif // MONITORALERTSACCEPTDIALOG_H
