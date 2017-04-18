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
#include <boost/serialization/vector.hpp>

#define CONFIG_FILE_NAME "EditorConfig.cfg"
#define CONFIG_DIR_NAME "editor"


/// <summary>
/// Zawiera informacje o konfiguracji edytora.
/// </summary>
/// <seealso cref="Singleton{EditorConfig}" />
class EditorConfig : public Singleton < EditorConfig >
{

public:
	EditorConfig();

	/** \brief get configuration file path*/
	std::string getConfigFilePath();
	/** \brief load configuration from file*/
	bool load();
	/** \brief save configuration to file*/
	bool save();

	void setSceneObjectsBoundingBoxVisibled(bool i_Visible) { setValue(&m_IsSceneObjectsBoundingBoxVisibled, i_Visible); }
	bool isSceneObjectsBoundingBoxVisibled() const { return m_IsSceneObjectsBoundingBoxVisibled; }

	void setSceneObjectsLabelVisibled(bool i_Visible) { setValue(&m_IsSceneObjectsLabelVisibled, i_Visible); }
	bool isSceneObjectsLabelVisibled() const{ return m_IsSceneObjectsLabelVisibled; }

	void setSequenceMultiSelection(bool i_IsMultiselection) { setValue(&m_IsSequenceMultiSelection, i_IsMultiselection);  }
	bool isSequenceMultiSelection() const { return m_IsSequenceMultiSelection; }

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
		ar & m_IsAutoReload;
		ar & m_IsSequenceMultiSelection;

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
	bool m_IsAutoReload;
	bool m_IsSequenceMultiSelection;

	bool m_IsAutoSaveEnabled;
	int m_AutoSaveTimerInterval;

	//user config
	std::vector<std::string> m_LastVideosPaths;
	std::string m_LastVideoAVIPath;
};


#endif // _EDITOR_CONFIG_H