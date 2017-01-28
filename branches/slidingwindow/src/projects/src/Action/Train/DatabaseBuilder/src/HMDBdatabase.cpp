#include "HMDBdatabase.h"

#include <utils/Filesystem.h>

#include <fstream>
#include <string>

void HmdbDatabase::generateDatabase(const std::string& videoPath)
{
	m_Database = utils::Database();
	m_Classes.clear();

	std::string path = utils::Filesystem::unifyPath(videoPath);

	getClassNames(path + "classes.txt");

	for (size_t i = 0; i < m_Classes.size(); ++i)
	{
		if (!m_Classes[i].second)
			continue;

		int index = (int)m_Database.getNumActions();
		addClass(utils::Filesystem::unifyPath(path + std::to_string(i + 1)), index, 1);
		m_Database.setActionName(index, m_Classes[i].first);
	}
}

void HmdbDatabase::getClassNames(const std::string& fileName)
{
	std::ifstream file(fileName);
	std::string name;
	int enabled;

	while (file >> name >> enabled)
		m_Classes.push_back(std::make_pair(name, enabled > 0));
}

void HmdbDatabase::addClass(const std::string& directory, int index, int split)
{
	std::string splitFileName = directory + "split" + std::to_string(split) + ".txt";
	std::ifstream file(splitFileName);

	std::string video;
	int dest;
	while (file >> video >> dest)
	{
		if (dest == 1)
			m_Database.addVideo(directory + video, index, utils::Database::TRAIN);
		else if (dest == 2)
			m_Database.addVideo(directory + video, index, utils::Database::TEST);
	}
}

