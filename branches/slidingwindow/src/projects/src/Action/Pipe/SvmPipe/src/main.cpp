#include "Recognizer.h"

#include <utils/Application.h>

int main(int argc, const char** argv)
{
	utils::Application::registerExitFunction();

	utils::Application::getInstance()->registerModule<Recognizer>("recognizer", "recognizer");
	utils::Application::getInstance()->setAppTitle("Action recognizer");

	return utils::Application::getInstance()->run<QApplication>(argc, argv);
}