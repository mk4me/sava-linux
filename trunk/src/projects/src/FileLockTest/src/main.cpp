#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "utils/FileLock.h"

int main(int argc, const char* argv[])
{
	utils::FileLock fl("/home/mc/tst.loc");
	bool res = fl.lock();
	if (res) {
		std::cout << "Locked" << std::endl;
	} else {
		std::cout << "Unable to lock" << std::endl;
	}

	while(true);
}