#include "AquisitionPipe.h"
#include <sequence/Video.h>
#include <config/Aquisition.h>
#include <config/Diagnostic.h>
#include <utils/Filesystem.h>
#include <utils/ProgramOptions.h>
#include <utils/AxisRawReader.h>
#include <utils/PipeProcessUtils.h>
#include <utils/FileLock.h>
#include <boost/timer/timer.hpp>
#include <utils/Application.h>


int main(int argc, const char** argv)
{
    AquisitionParams aqparams;
	ProgramOptions options;
	options.add<std::string>("ip", "  camera IP string e.g. \"1.2.3.4\" (\"http://\" is added automatically)");
	options.add<std::string>("user", "  camera user name");
	options.add<std::string>("pass", "  camera password");
	options.add<size_t>("fps", "  desired frame rate (range: 1 to 25) - 0 means maximum rate (optional)");
	options.add<size_t>("c", "JPEG compression rate from 1 - lowest to 99 - highest (optional)");
	options.add<size_t>("fw", "frame width (optional)", utils::camera::AxisRawReader::DEFAULT_WIDTH);
	options.add<size_t>("fh", "frame height (optional)", utils::camera::AxisRawReader::DEFAULT_HEIGHT);
	options.parse(argc, argv);
	try {
        bool success = true;

        config::Aquisition &config = config::Aquisition::getInstance();
        config.load();

        success &= options.get<std::string>("user", aqparams.user);
        success &= options.get<std::string>("pass", aqparams.password);
        success &= options.get<std::string>("ip", aqparams.ip);

        if (!options.get<size_t>("fps", aqparams.fps))
            aqparams.fps = config.getFps();
        if (!options.get<size_t>("c", aqparams.compression))
            aqparams.compression = config.getCompression();
        success &= options.get<size_t>("fw", aqparams.frameWidth);
        success &= options.get<size_t>("fh", aqparams.frameHeight);

        std::cout << "================================================================================\n";
        std::cout << "               Camera IP: " << aqparams.ip << "\n";
        std::cout << "                    User: " << aqparams.user << "\n";
        std::cout << "                Password: " << aqparams.password << "\n";
        std::cout << "        Frame rate (fps): " << aqparams.fps << "\n";
        std::cout << "        JPEG compression: " << aqparams.compression << "\n";
        std::cout << "             Frame width: " << aqparams.frameWidth << "\n";
        std::cout << "            Frame height: " << aqparams.frameHeight << "\n";
        std::cout << "================================================================================\n\n";


        config::Diagnostic::getInstance().load();
        if (config::Diagnostic::getInstance().getLogMemoryUsage()) {
            utils::Application::getInstance()->enableMomoryLogging();
        }

        if (success) {
            AquisitionPipe ap(aqparams);
            ap.start();

            while(ap.isRunning())
            {
                boost::this_thread::sleep(boost::posix_time::seconds(1));
                std::cout << "Current Fifo size: " << AquisitionFifo::frames.size() << std::endl;
            }
        } else {
            std::cerr << "Parameter error." << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown error." << std::endl;
	}

	return 0;
}