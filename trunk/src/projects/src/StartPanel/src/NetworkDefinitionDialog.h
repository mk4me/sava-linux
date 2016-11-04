#ifndef NETWORKDEFINITIONDIALOG_H
#define NETWORKDEFINITIONDIALOG_H

#include <QtWidgets/QDialog>
#include "ui_NetworkDefinitionDialog.h"

class NetworkDefinitionDialog : public QDialog
{
	Q_OBJECT

public:
	NetworkDefinitionDialog(QWidget *parent = 0);
	~NetworkDefinitionDialog();

	void setFields(const QString& name, const QString& ip);

	QString getName() const { return ui.m_Name->text(); }
	QString getIp() const { return ui.m_Ip->text(); }

private:
	Ui::NetworkDefinitionDialog ui;
};

#endif // NETWORKDEFINITIONDIALOG_H
