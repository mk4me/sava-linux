#include "StartPanel.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");
	StartPanel w;
	w.show();
	return a.exec();
}
