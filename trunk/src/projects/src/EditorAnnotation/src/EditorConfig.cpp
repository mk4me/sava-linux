#include "EditorConfig.h"
#include <utils/Filesystem.h>

#include <fstream>

#include <boost/archive//text_iarchive.hpp>
#include <boost/archive//text_oarchive.hpp>

/*--------------------------------------------------------------------*/
EditorConfig::EditorConfig() :
m_IsSceneObjectsBoundingBoxVisibled(true),
m_IsSceneObjectsLabelVisibled(false),
m_IsSceneTrajectoryVisibled(true),
m_IsTrajectoryPartsAllVisibled(true),
m_IsTrajectoryHelpViewVisibled(true),
m_TrajectoryPointRadiu(5),
m_TrajectoryLineWidth(10),
m_LastVideoPath(""),
m_IsAutoReload(true),
m_IsAutoSaveEnabled(true),
m_AutoSaveTimerInterval(1000 * 60 * 10)
{
	
}

/*--------------------------------------------------------------------*/
std::string EditorConfig::getConfigFilePath()
{
	return utils::Filesystem::getConfigPath() + CONFIG_FILE_NAME;
}

/*--------------------------------------------------------------------*/
bool EditorConfig::load()
{
	std::ifstream file(getConfigFilePath());

	if (!file.good())
		return false;

	try{
		boost::archive::text_iarchive iarchive(file);
		iarchive >> EditorConfig::getInstance();
		file.close();
	}
	catch (...)
	{
		std::cout << "Failed to load editor config: " << getConfigFilePath() << std::endl;
	}

	EditorMessageManager::sendMessage(MESSAGE_CONFIG_CHANGED);

	return true;

}

/*--------------------------------------------------------------------*/
bool EditorConfig::save()
{
	std::ofstream file(getConfigFilePath());

	if (!file.good())
		return false;

	try 
	{
		boost::archive::text_oarchive oarchive(file);
		oarchive << EditorConfig::getInstance();
		file.close();

		std::cout << "Config saved. " << std::endl;
	}
	catch (...)
	{
		std::cout << "Can't save file: " << getConfigFilePath() << std::endl;
		return false;
	};

	return true;
}



