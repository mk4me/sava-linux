#include "SystemDatabase.h"

#include "sequence/Action.h"
#include "sequence/Cluster.h"

#include "config/Action.h"
#include "config/Directory.h"
#include "config/Glossary.h"

#include "utils/Filesystem.h"

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

utils::Database SystemDatabase::generateDatabase()
{
	config::Action::getInstance().load();
	config::Glossary::getInstance().load();
	config::Directory::getInstance().load();

	utils::Database database;
	collectAll(database);
	return reorganize(database);
}

bool SystemDatabase::isActionFile(const std::string& path)
{
	if (path.length() < 5)
		return false;

	return path.compare(path.length() - 4, 4, ".acn") == 0;
}

void SystemDatabase::collectAll(utils::Database& database)
{
	size_t minFrames = config::Action::getInstance().getMinVideoFrames();

	bfs::recursive_directory_iterator endIt;
	bfs::recursive_directory_iterator dirIter(config::Directory::getInstance().getVideosPath());
	for (; dirIter != endIt; ++dirIter)
	{
		const std::string& filePath = dirIter->path().string();
		if (!isActionFile(filePath))
			continue;

		sequence::Action action(filePath);
		if (action.getSplit() == sequence::Action::GENERATED)
			continue;

		std::string clusterFileName = filePath.substr(0, filePath.length() - 4) + ".clu";
		if (!utils::Filesystem::exists(clusterFileName))
			continue;

		sequence::Cluster cluster(clusterFileName);
		if (cluster.getStartFrame() == sequence::Cluster::NPOS || cluster.getEndFrame() - cluster.getStartFrame() < minFrames)
			continue;

		database.addVideo(clusterFileName, action.getActionId(), (utils::Database::Split)action.getSplit());
	}
}

utils::Database SystemDatabase::reorganize(const utils::Database& database)
{
	const int cTestCount = config::Action::getInstance().getTestSetLength();
	const int cTrainCount = config::Action::getInstance().getTrainSetLength();

	utils::Database newDatabase;
	for (auto action : config::Glossary::getInstance().getEditorActions())
	{
		if (!action.train)
			continue;

		size_t trainCount = database.getNumVideos(action.id, utils::Database::TRAIN);
		size_t testCount = database.getNumVideos(action.id, utils::Database::TEST);
		int unassignedCount = (int)database.getNumVideos(action.id, utils::Database::UNASSIGNED);

		if (trainCount < cTrainCount)
		{
			unassignedCount -= (int)(cTrainCount - trainCount);
			if (unassignedCount < 0)
				continue;
		}
		if (testCount + unassignedCount < cTestCount)
			continue;

		int index = (int)newDatabase.getNumActions();

		std::vector<std::string> vTrain = database.getVideos(action.id, utils::Database::TRAIN);
		std::vector<std::string> vTest = database.getVideos(action.id, utils::Database::TEST);
		std::vector<std::string> vUnassigned = database.getVideos(action.id, utils::Database::UNASSIGNED);

		std::random_shuffle(vTrain.begin(), vTrain.end());
		std::random_shuffle(vTest.begin(), vTest.end());
		std::random_shuffle(vUnassigned.begin(), vUnassigned.end());

		if (trainCount < cTrainCount)
		{
			size_t num = cTrainCount - trainCount;
			vTrain.insert(vTrain.end(), vUnassigned.begin(), vUnassigned.begin() + num);
			vUnassigned.erase(vUnassigned.begin(), vUnassigned.begin() + num);
		}

		if (testCount < cTestCount)
		{
			size_t num = cTestCount - testCount;
			vTest.insert(vTest.end(), vUnassigned.begin(), vUnassigned.begin() + num);
		}

		for (size_t j = 0; j < cTrainCount; ++j)
			newDatabase.addVideo(vTrain[j], index, utils::Database::TRAIN);
		for (size_t j = 0; j < cTestCount; ++j)
			newDatabase.addVideo(vTest[j], index, utils::Database::TEST);

		newDatabase.setActionName(index, action.name);
	}

	return newDatabase;
}

