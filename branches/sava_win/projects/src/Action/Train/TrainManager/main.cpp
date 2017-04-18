#include "TrainManager.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TrainManager w;
	w.show();
	return a.exec();
}
