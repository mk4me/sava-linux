#include "CleanupDialog.h"
#include "CleanupThread.h"

CleanupDialog::CleanupDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	flags = flags & (~Qt::WindowTitleHint);
	flags = flags | Qt::CustomizeWindowHint;
	setWindowFlags(flags);
}

CleanupDialog::~CleanupDialog()
{

}

void CleanupDialog::showEvent(QShowEvent *)
{
	CleanupThread* cleanupThread = new CleanupThread(this);
	connect(cleanupThread, &CleanupThread::finished, this, &CleanupDialog::accept);
	cleanupThread->start();
}
