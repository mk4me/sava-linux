#pragma once
#ifndef _EDITOR_CONFIG_H
#define _EDITOR_CONFIG_H

#include <utils/Singleton.h>
#include "EditorMessageManager.h"
#include "QtCore/QDebug"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include "EditorTrajectoryParts.h"

#define CONFIG_FILE_NAME "EditorConfig.cfg"
#define CONFIG_DIR_NAME "editor"

/**
* \class EditorConfig
*
* \brief Class to manage editor configuration.
*/
class EditorConfig : public Singleton<EditorConfig>
{

public:
	EditorConfig();

	/** \brief get configuration file path*/
	std::string getConfigFilePath();
	/** \brief load configuration from file*/
	bool load();
	/** \brief save configuration to file*/
	bool save();

	/** \brief get trajectory parts configuration*/
	EditorTrajectoryParts& getTrajectoryParts() { return m_TrajectoryParts; }

	void setSceneObjectsBoundingBoxVisibled(bool i_Visible) { setValue(&m_IsSceneObjectsBoundingBoxVisibled, i_Visible); }
	bool isSceneObjectsBoundingBoxVisibled() const { return m_IsSceneObjectsBoundingBoxVisibled; }

	void setSceneObjectsLabelVisibled(bool i_Visible) { setValue(&m_IsSceneObjectsLabelVisibled, i_Visible); }
	bool isSceneObjectsLabelVisibled() const{ return m_IsSceneObjectsLabelVisibled; }

	void setSceneTrajectoryVisibled(bool i_Visible) { setValue(&m_IsSceneTrajectoryVisibled, i_Visible); }
	bool isSceneTrajectoryVisibled() const { return m_IsSceneTrajectoryVisibled; }

	void setSequenceMultiSelection(bool i_IsMultiselection) { setValue(&m_IsSequenceMultiSelection, i_IsMultiselection);  }
	bool isSequenceMultiSelection() const { return m_IsSequenceMultiSelection; }

	void setTrajectoryPartsVisibled(bool i_Visible) { setValue(&m_IsTrajectoryPartsAllVisibled, i_Visible); }
	bool isTrajectoryPartsVisibled() const { return m_IsTrajectoryPartsAllVisibled; }

	void setTrajectoryHelpViewVisibled(bool i_Visibled) { setValue(&m_IsTrajectoryHelpViewVisibled, i_Visibled); }
	bool isTrajectoryHelpViewVisibled() const { return m_IsTrajectoryHelpViewVisibled; }

	void setTrajectoryPointRadius(int radius) { setValue(&m_TrajectoryPointRadiu, radius); }
	int getTrajectoryPointRadius() const { return m_TrajectoryPointRadiu; }

	void setTrajectoryLineWidth(int width) { setValue(&m_TrajectoryLineWidth, width); }
	int getTrajectoryLineWidth() const { return m_TrajectoryLineWidth; }

	void setAutoReload(bool i_Enabled) { setValue(&m_IsAutoReload, i_Enabled); }
	bool isAutoReload() const { return m_IsAutoReload; }

	void setAutoSaveEnabled(bool i_Enabled) { setValue(&m_IsAutoSaveEnabled, i_Enabled); }
	bool isAutoSaveEnabled() const { return m_IsAutoSaveEnabled; }

	void setAutoSaveTimerInterval(int i_Interval) { setValue(&m_AutoSaveTimerInterval, i_Interval); }
	int getAutoSaveTimerInterval() const { return m_AutoSaveTimerInterval; }

	//user config
	void setLastVideosPaths(const std::vector<std::string>& i_VidesoPaths) { m_LastVideosPaths = i_VidesoPaths; }
	std::vector<std::string> getLastVideosPaths() const { return m_LastVideosPaths; }

	void setLastVideoAVIPath(const std::string& aviPath) { setValue(&m_LastVideoAVIPath, aviPath); }
	std::string getLastVideoAVIPath() const { return m_LastVideoAVIPath; }

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_IsSceneObjectsBoundingBoxVisibled;
		ar & m_IsSceneObjectsLabelVisibled;
		ar & m_IsSceneTrajectoryVisibled;
		ar & m_IsTrajectoryHelpViewVisibled;
		ar & m_IsAutoReload;
		ar & m_IsSequenceMultiSelection;

		ar & m_IsTrajectoryPartsAllVisibled;
		ar & m_TrajectoryPointRadiu;
		ar & m_TrajectoryLineWidth;
		ar & m_TrajectoryParts;

		ar & m_IsAutoSaveEnabled;
		ar & m_AutoSaveTimerInterval;

		ar & m_LastVideosPaths;
		ar & m_LastVideoAVIPath;
	};

	template<typename Type>
	void setValue(Type* field, Type value)
	{
		*field = value;
		EditorMessageManager::sendMessage(MESSAGE_CONFIG_CHANGED);
	}

private:
	bool m_IsSceneObjectsBoundingBoxVisibled;
	bool m_IsSceneObjectsLabelVisibled;
	bool m_IsSceneTrajectoryVisibled;
	bool m_IsTrajectoryPartsAllVisibled;
	bool m_IsTrajectoryHelpViewVisibled;
	bool m_IsAutoReload;
	bool m_IsSequenceMultiSelection;

	bool m_IsAutoSaveEnabled;
	int m_AutoSaveTimerInterval;

	int m_TrajectoryPointRadiu;
	int m_TrajectoryLineWidth;

	EditorTrajectoryParts m_TrajectoryParts;

	//user config
	std::vector<std::string> m_LastVideosPaths;
	std::string m_LastVideoAVIPath;
};


#endif // _EDITOR_CONFIG_H