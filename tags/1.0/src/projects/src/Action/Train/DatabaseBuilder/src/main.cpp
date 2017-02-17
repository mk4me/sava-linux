#include "SystemDatabase.h"
#include "HMDBdatabase.h"

#include "utils/Filesystem.h"

#include <fstream>
#include <iostream>

void printToFile(utils::Database& database);

int main(int argc, char **argv)
{
#if 1
	utils::Database database = SystemDatabase::generateDatabase();
#else
	HmdbDatabase hmdb;
	hmdb.generateDatabase("D:\\Shared\\Datasets\\hmdb51_test\\");
	utils::Database database = hmdb.getDatabase();
#endif
	
	printToFile(database);

	database.save();
}

void printToFile(utils::Database& database)
{
	std::string fileName = utils::Filesystem::getDataPath() + "action/database.txt";
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
