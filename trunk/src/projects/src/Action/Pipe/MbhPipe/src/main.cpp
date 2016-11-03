#include "Descriptor.h"
#include <utils/Application.h>

int main(int argc, const char** argv)
{
	Descriptor::setOutFileExtension("mbh");
	
	utils::Application::registerExitFunction();

	utils::Application::getInstance()->registerModule<Descriptor>("descriptor", "descriptor");
	utils::Application::getInstance()->setAppTitle("Mbh descritptor");

	return utils::Application::getInstance()->run<QApplication>(argc, argv);
}