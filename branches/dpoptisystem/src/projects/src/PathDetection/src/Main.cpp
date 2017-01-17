#include "PathDetection.h"
#include <utils/Application.h>

int main(int argc, const char** argv)
{
	utils::Application::registerExitFunction();

	utils::Application::getInstance()->registerModule<clustering::PathDetection>("detect", "path detection");

	utils::Application::getInstance()->setAppTitle("System analizy sciezek");
	return utils::Application::getInstance()->run<QApplication>(argc, argv);
}