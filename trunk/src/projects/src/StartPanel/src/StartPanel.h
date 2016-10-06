#ifndef STARTPANEL_H
#define STARTPANEL_H

#include "ui_StartPanel.h"
#include "ConfigDialog.h"

class StartPanel : public QMainWindow
{
	Q_OBJECT

public:
	StartPanel(QWidget *parent = 0);
	~StartPanel();

private slots:
	void showConfigDialog();
	void runAnnotationEditor();

private:
	Ui::StartPanelClass ui;

	ConfigDialog* m_ConfigDialog;
};

#endif // STARTPANEL_H
