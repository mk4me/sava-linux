#pragma once
#ifndef SystemDatabase_h__
#define SystemDatabase_h__

#include "utils/Database.h"

class SystemDatabase
{
public:
	static std::string m_videoPath; //for dp opti system
	static utils::Database generateDatabase();
	static std::string getVideoPath(); //for dp opti system
	static void setVideoPath(std::string setVidPath); // for dp opti system
private:
	static bool isActionFile(const std::string& path);

	static void collectAll(utils::Database& database);
	static utils::Database reorganize(const utils::Database& database);
};


#endif // SystemDatabase_h__
