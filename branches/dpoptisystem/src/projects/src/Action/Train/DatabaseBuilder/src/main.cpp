#include "SystemDatabase.h"
#include "HMDBdatabase.h"
#include "dputils.h" 
#include "dplog.h"

#include "utils/Filesystem.h"

#include <fstream>
#include <iostream>

#include "config/Directory.h"

void printToFile(utils::Database& database, std::string OutputDir);

int main(int argc, char **argv)
{
	//Parse OPP parameters
	std::string coreParams = dp::oppGetParamsFromArgs(argc, argv);
	
	//Use Dp logs
	dp::dpLog log;
	log.initLogFile(coreParams);

	log.dbg("<cpp>params:");
	log.dbgl(coreParams);

	std::string testNameStr = dp::oppGetValueForKey("testName", coreParams);
	log.dbg("<cpp>testNameStr:");
	log.dbgl(testNameStr);

	std::string dbOutDir = dp::oppGetValueForKey("dbOutDir", coreParams) +"/";
	log.dbg("<cpp>dbOutDir:");
	log.dbgl(dbOutDir);

	std::string movies = dp::oppGetValueForKey("moviesDir", coreParams) + "/";
	log.dbg("<cpp>moviesDir:");
	log.dbgl(movies);

	log.closeLogFile();
	/*
	// Write OPP string to result file
	log.initResultFile(coreParams);
	log.addResult("testName=" + testNameStr + ";accuracy=0.91");
	log.closeResultFile();
	*/
	std::string g_OutputDIr = utils::Filesystem::getAppPath() + dbOutDir;
	
	std::cout << "GET VIDEO PATH 1:" << config::Directory::getInstance().getVideosPath() << std::endl;
	//config::Directory::getInstance().setVideosPath(movies);
	
	std::cout << "GET VIDEO PATH 2:" << config::Directory::getInstance().getVideosPath() << std::endl;
#if 1
	SystemDatabase::setVideoPath(utils::Filesystem::getAppPath() + movies);
	utils::Database database = SystemDatabase::generateDatabase();
#else
	HmdbDatabase hmdb;
	hmdb.generateDatabase("D:\\Shared\\Datasets\\hmdb51_test\\");
	utils::Database database = hmdb.getDatabase();
#endif
	config::Directory::getInstance().setVideosPath(movies);
	std::cout << "GET VIDEO PATH 3:" << config::Directory::getInstance().getVideosPath() << std::endl;
	printToFile(database, g_OutputDIr);
	utils::Database::setDatabaseDir(g_OutputDIr);
	std::cout << "GET VIDEO PATH 9:" << config::Directory::getInstance().getVideosPath() << std::endl;
	database.save();
}

void printToFile(utils::Database& database, std::string OutputDir)
{
	std::cout << "GET VIDEO PATH 4:" << config::Directory::getInstance().getVideosPath() << std::endl;
	std::string fileName = OutputDir + "database.txt";
	std::cout << "GET VIDEO PATH 5:" << config::Directory::getInstance().getVideosPath() << std::endl;
	std::ofstream file;
	file.open(fileName, std::ios::out | std::ios::trunc);
	if (!file.is_open())
	{
		std::cerr << "Can't open summary file: " << fileName << std::endl;
		return;
	}

	file << "Class count: " << database.getNumActions() << std::endl;

	for (int i : database.getActionIdList())
	{
		file << std::endl << "Class " << i << " ["
			<< database.getActionName(i)
			<< ']' << std::endl << "--TRAIN--" << std::endl;

		const auto& trainVideos = database.getVideos(i, utils::Database::TRAIN);
		for (size_t j = 0; j < trainVideos.size(); ++j)
			file << '\t' << j << ' ' << trainVideos[j] << std::endl;

		file << "--TEST--" << std::endl;

		const auto& testVideos = database.getVideos(i, utils::Database::TEST);
		for (size_t j = 0; j < testVideos.size(); ++j)
			file << '\t' << j << ' ' << testVideos[j] << std::endl;
	}
}
