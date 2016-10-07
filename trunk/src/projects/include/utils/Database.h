#pragma once
#ifndef Database_h__
#define Database_h__

#include <array>
#include <vector>
#include <map>

namespace utils
{
	class Database
	{
	public:
		typedef std::vector<std::string> VideoList;
		typedef std::map<int, std::pair<std::string, std::array<VideoList, 4>>> ActionList;
		typedef std::vector<int> ActionIdList;

		enum Split
		{
			UNASSIGNED = 0,
			TRAIN = 1,
			TEST = 2,
			GENERATED = 3,
		};
	
		Database() { }
		~Database() { }
	
		bool load();
		bool save() const;
	
		void addVideo(const std::string& videoFile, int actionId, Split split);
	
		void setActionName(int actionId, const std::string& name) { m_Videos[actionId].first = name; }
		const std::string& getActionName(int actionId) const;

		size_t getNumActions() const { return m_Videos.size(); }
		ActionIdList getActionIdList() const;
		size_t getNumVideos(int actionId, Split split) const;
		const VideoList& getVideos(int actionId, Split split) const;
	
	private:
		static const std::string c_DatabaseDir;
		static const std::string c_DatabasePath;
		static const std::string c_EmptyString;
	
		ActionList m_Videos;
	};
}

#endif // Database_h__
