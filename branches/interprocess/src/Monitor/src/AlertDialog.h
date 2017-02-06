#ifndef ALERTDIALOG_H
#define ALERTDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidgetItem>

#include "ui_AlertDialog.h"
#include "MonitorActionManager.h"

/// <summary>
/// Klasa wywietlaj¹ca okno wraz z list¹ mo¿liwych akcji do wykrycia. 
/// U¿ytkownik ma mo¿liwoæ wyboru wielu akcji, które chcia³by by system wykrywa³ na obrazie z kamery.
/// </summary>
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
