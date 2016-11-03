#include "Descriptor.h"
#include <utils/Application.h>

int main(int argc, const char** argv)
{
	Descriptor::setOutFileExtension("gbh");

	utils::Application::registerExitFunction();

	utils::Application::getInstance()->registerModule<Descriptor>("descriptor", "descriptor");
	utils::Application::getInstance()->setAppTitle("Gbh descritptor");

	return utils::Application::getInstance()->run<QApplication>(argc, argv);
}