#include "StartPanel.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	StartPanel w;
	w.show();
	return a.exec();
}
