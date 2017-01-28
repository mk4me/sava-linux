#include "Database.h"

#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/filesystem.hpp>

#include <fstream>

namespace  utils
{
	const std::string Database::c_DatabaseDir = utils::Filesystem::getDataPath() + "action/";
	const std::string Database::c_DatabasePath = c_DatabaseDir + "database.dat";
	const std::string Database::c_EmptyString;

	bool Database::load()
	{
		try
		{
			std::ifstream ifs(c_DatabasePath, std::ios::binary);
			//boost::archive::text_iarchive ia(ifs);
            boost::archive::binary_iarchive ia(ifs);
			ia & m_Videos;
	
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	
	bool Database::save() const
	{
		try
		{
			boost::filesystem::create_directories(c_DatabaseDir);
			std::ofstream ofs(c_DatabasePath, std::ios::binary);
			boost::archive::binary_oarchive oa(ofs);
			oa & m_Videos;
	
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	
	void Database::addVideo(const std::string& videoFile, int actionId, Split split)
	{	
		std::string videoName = videoFile;
		std::replace(videoName.begin(), videoName.end(), '\\', '/');
		VideoList& videos = m_Videos[actionId].second[split];
		if (std::find(videos.begin(), videos.end(), videoName) == videos.end())
			videos.push_back(videoName);
	}
	
	const std::string& Database::getActionName(int actionId) const
	{
		auto it = m_Videos.find(actionId);
		if (it == m_Videos.end())
			return c_EmptyString;

		return it->second.first;
	}

	Database::ActionIdList Database::getActionIdList() const
	{
		ActionIdList list;
		for (auto it : m_Videos)
			list.push_back(it.first);

		return list;
	}

	size_t Database::getNumVideos(int actionId, Split split) const
	{
		auto it = m_Videos.find(actionId);
		if(it == m_Videos.end())
			return 0;

		return it->second.second[split].size();
	}

	const Database::VideoList& Database::getVideos(int actionId, Split split) const
	{
		static VideoList emptyList;
	
		auto it = m_Videos.find(actionId);
		if (it == m_Videos.end())
			return emptyList;
	
		return  it->second.second[split];
	}
}
