#include "MonitorPipe.h"

#include "MonitorWindow.h"
#include "MilestoneMonitorPipe.h"

#include <utils/Application.h>

//#include "vld.h"

class MonitorWindowCreator : public MonitorPipe::VisualizationCreator
{
public:
	virtual void createVisualization()
	{
		static MonitorWindow w;
		w.showMaximized();
	}
};

int main(int argc, const char** argv)
{
	std::shared_ptr<QApplication> app = std::make_shared<QApplication>(argc, const_cast<char**>(argv));

	MonitorWindowCreator mwc;
	MonitorPipe::setVisualizationCreator(&mwc);

	utils::Application::registerExitFunction();

	utils::Application::getInstance()->registerModule<MonitorPipe>("monitor", "monitor", true);
	utils::Application::getInstance()->registerModule<MilestoneMonitorPipe>("milestone", "milestone");

	utils::Application::getInstance()->setAppTitle("Monitor");
	return utils::Application::getInstance()->run(argc, argv, app);
}