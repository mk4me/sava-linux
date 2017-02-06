#include "Compression.h"
#include <utils/Application.h>

int main(int argc, const char** argv)
{
	utils::Application::registerExitFunction();

	utils::Application::getInstance()->registerModule<Compression>("compression", "compression");

	utils::Application::getInstance()->setAppTitle("System semi-automatycznej akwizycji danych");
	return utils::Application::getInstance()->run<QApplication>(argc, argv);
}