#include <iostream>
#include "regressionTest.h"
#include "timingTest.h"

int main(int argc, const char* argv[])
{
    std::string path = ".";
	regressionTests(path);
	timingTests(path);

	std::cout << "Done" << std::endl;
}