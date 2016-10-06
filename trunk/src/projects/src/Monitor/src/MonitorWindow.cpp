#include "MonitorWindow.h"

MonitorWindow::MonitorWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_FrameTimer.setFramesTimes({1000, 2000, 3000, 4000, 5000, 6000, 6100, 6200, 6300, 6600, 8000});
}

MonitorWindow::~MonitorWindow()
{

}

void MonitorWindow::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
		case Qt::Key_Escape:
			qApp->quit();
			break;

		case Qt::Key_P:
			m_FrameTimer.play();
			break;

		case Qt::Key_S:
			m_FrameTimer.stop();
			break;
	}
}
