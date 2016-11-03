#pragma once
#ifndef SystemDatabase_h__
#define SystemDatabase_h__

#include "utils\Database.h"

class SystemDatabase
{
public:
	static utils::Database generateDatabase();

private:
	static bool isActionFile(const std::string& path);

	static void collectAll(utils::Database& database);
	static utils::Database reorganize(const utils::Database& database);
};


#endif // SystemDatabase_h__
