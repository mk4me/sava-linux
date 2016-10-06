#include "schedulerWidget.h"

int main(int argc, const char* argv[])
{
	QApplication app(argc, const_cast<char**>(argv));
	QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");

	if (argc == 1) 
	{
		SchedulerWidget mainForm;
		mainForm.show();
		app.exec();
	}
	else
	{
		//deamon(argc, argv);
		//recognizeParameters( argc, argv );
	}
}
