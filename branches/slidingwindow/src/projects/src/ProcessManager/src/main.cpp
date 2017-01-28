#include "ProcessManager.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ProcessManager w;
	w.show();
	return a.exec();
}