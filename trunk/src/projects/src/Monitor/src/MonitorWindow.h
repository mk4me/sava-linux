#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <QMainWindow>
#include "ui_MonitorWindow.h"
#include <QKeyEvent>

#include "FrameTimer.h"

class MonitorWindow : public QMainWindow
{
	Q_OBJECT

public:
	MonitorWindow(QWidget *parent = 0);
	~MonitorWindow();

protected:
	virtual void keyPressEvent(QKeyEvent *) override;

private:
	Ui::MonitorWindow ui;

	FrameTimer m_FrameTimer;
};

#endif // MONITORWINDOW_H
