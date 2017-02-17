#include "Descriptor.h"

#include <utils/Application.h>
#include <utils/GpuUtils.h>

int main(int argc, const char** argv)
{
	Descriptor::setOutFileExtension("mbh");
	
	utils::Application::registerExitFunction();

	utils::Application::getInstance()->registerModule<Descriptor>("descriptor", "descriptor");
	utils::Application::getInstance()->setAppTitle("Mbh descritptor");

	std::cout << "Initializing GPU..." << std::endl;
	utils::GpuUtils::forceGpuInitialization();

	return utils::Application::getInstance()->run<QApplication>(argc, argv);
}