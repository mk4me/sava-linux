#include "PathAnalysis.h"
#include <utils/Application.h>

int main(int argc, const char** argv)
{
	utils::Application::registerExitFunction();

	utils::Application::getInstance()->registerModule<clustering::PathAnalysis>("analysis", "path analysis");

	utils::Application::getInstance()->setAppTitle("System analizy sciezek");
	return utils::Application::getInstance()->run<QApplication>(argc, argv);
}