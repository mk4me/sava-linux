#include "StartPanel.h"

#include <QProcess>

StartPanel::StartPanel(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_ConfigDialog = new ConfigDialog(this);
}

StartPanel::~StartPanel()
{

}

void StartPanel::showConfigDialog()
{
	m_ConfigDialog->exec();
}

void StartPanel::runAnnotationEditor()
{
	QProcess::startDetached("EditorAnnotation.exe");
}
