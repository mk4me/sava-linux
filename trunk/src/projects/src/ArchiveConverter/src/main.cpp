#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "ArchiveConverter.h"

namespace po = boost::program_options;

const std::string version = "0.1";

int main(int argc, const char* argv[])
{
	std::string inDir, outDir;
	po::options_description desc("Options:");
	desc.add_options()
		("version,v", "print version string")
		("help,h", "produces this help message")
		("input-directory-path,I", po::value<std::string>(&inDir), "Directory with files to convert")
		("output-directory-path,O", po::value<std::string>(&outDir), "Output directory")
		("from-text,t", "conversion from text archiver")
		;

	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (...) {
		std::cout << "Type --help to get list of available options." << std::endl;
		return false;
	}

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return false;
	}

	if (vm.count("version")) {
		std::cout << version << std::endl;
		return false;
	}

	try {
		if (vm.count("from-text")) {
			ArchiveConverter::convertToBinary(inDir, outDir);
		} else {
			ArchiveConverter::convertToText(inDir, outDir);
		}
	}
	catch (std::exception& e) {
		std::cerr << "Problem occured: " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown error" << std::endl;
	}
	
	return true;
}