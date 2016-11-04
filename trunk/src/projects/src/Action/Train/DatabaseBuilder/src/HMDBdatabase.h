#pragma once
#ifndef HMDBdatabase_h__
#define HMDBdatabase_h__

#include "utils/Database.h"

class HmdbDatabase
{
public:
	void generateDatabase(const std::string& videoPath);
	const utils::Database& getDatabase() const { return m_Database; }

private:
	typedef std::vector<std::pair<std::string, bool>> ClassNames;

	ClassNames m_Classes;
	utils::Database m_Database;

	void getClassNames(const std::string& fileName);
	void addClass(const std::string& directory, int index, int split);
};

#endif // HMDBdatabase_h__
