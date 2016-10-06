
#include "boost/thread/thread.hpp"
#include "boost/bind.hpp"
#include <conio.h>

#include "QtWidgets/QApplication"
#include "EditorWindow.h"
#include "EditorSingletons.h"

//#include "vld.h"

EditorSingletos gSingletons;

int main(int argc, const char* argv[])
{
	QApplication app(argc, const_cast<char**>(argv));
	QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");

	EditorWindow editor;
	editor.showMaximized();
	app.exec();
}
