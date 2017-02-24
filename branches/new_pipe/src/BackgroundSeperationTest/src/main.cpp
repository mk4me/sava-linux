#include <iostream>
#include <sstream>
#include <fstream>
#include "regressionTest.h"
#include "timingTest.h"

int main(int argc, const char* argv[])
{
    std::string path = ".";
	regressionTests(path);
	timingTests(path);
	//std::ostringstream ss1, ss2;
	std::ofstream file1("/home/mc/programming/sava_data/temp/csvlog_branch.txt");
	std::ofstream file2("/home/mc/programming/sava_data/temp/csvlog_trunk.txt");
	dropCSVInfo(file1, "/home/mc/programming/sava_data/temp/out_branch");
	dropCSVInfo(file2, "/home/mc/programming/sava_data/temp/out_trunk");
	std::cout << "Done" << std::endl;
}