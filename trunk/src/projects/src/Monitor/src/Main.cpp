#include <QtWidgets\QApplication>
#include "MonitorWindow.h"



int main(int argc, const char** argv)
{
	QApplication a(argc, const_cast<char**>(argv));

	MonitorWindow w;
	w.show();

	return a.exec();
} 