#include "EditorMessageManager.h"
#include "QtCore/QDebug"

#include <iostream>

/*--------------------------------------------------------------------*/
void EditorMessageManager::registerMessage(EditorMessageType i_Type, EditorMessageListener* i_Listener)
{
	m_Listeners[i_Type].insert(i_Listener);
}

/*--------------------------------------------------------------------*/
void EditorMessageManager::unregisterMessage(EditorMessageType i_Type, EditorMessageListener* i_Listener)
{
	std::set<EditorMessageListener*>& set = m_Listeners[i_Type];
	auto it = set.find(i_Listener);
	if (it != set.end())
		set.erase(it);
}

/*--------------------------------------------------------------------*/
void EditorMessageManager::unregisterMessages(EditorMessageListener* i_Listener)
{
	for (auto itSet = m_Listeners.begin(); itSet != m_Listeners.end(); ++itSet)
	{
		auto it = itSet->find(i_Listener);
		if (it != itSet->end())
			itSet->erase(it);
	}
}

/*--------------------------------------------------------------------*/
void EditorMessageManager::sendMessage(EditorMessageType i_Type, int i_ExtraParam /*= -1*/)
{
	//qDebug() << getMessageName(i_Type).c_str();

	std::set<EditorMessageListener*>& set = m_Listeners[i_Type];
	for (auto it = set.begin(); it != set.end(); ++it)
		(*it)->onMessage(i_Type, i_ExtraParam);
}

/*--------------------------------------------------------------------*/
EditorMessageManager::ListenerContainer EditorMessageManager::create()
{
	ListenerContainer ret;
	for (int i = 0; i < MESSAGE_MAX; i++)
		ret.push_back(std::set<EditorMessageListener*>());
	return ret;
}

/*--------------------------------------------------------------------*/
std::string EditorMessageManager::getMessageName(EditorMessageType i_Message)
{
	switch (i_Message)
	{
		case MESSAGE_PREPARE_FRAME_CHANGE: return "MESSAGE_PREPARE_FRAME_CHANGE";
		case MESSAGE_FRAME_CHANGED:	return "MESSAGE_FRAME_CHANGED";
		case MESSAGE_START_PLAYING:	return "MESSAGE_START_PLAYING";
		case MESSAGE_STOP_PLAYING: return "MESSAGE_STOP_PLAYING";
		case MESSAGE_PLAYING_DIRECTION_CHANGED: return "MESSAGE_PLAYING_DIRECTION_CHANGED";
		case MESSAGE_SEQUENCE_LOADED: return "MESSAGE_SEQUENCE_LOADED";
		case MESSAGE_SCENE_OBJECT_UPDATE: return "MESSAGE_SCENE_OBJECT_UPDATE";
		case MESSAGE_SCENE_OBJECT_CREATED: return "MESSAGE_SCENE_OBJECT_CREATED";
		case MESSAGE_SCENE_OBJECT_DELETED: return "MESSAGE_SCENE_OBJECT_DELETED";
		case MESSAGE_SCENE_SELECTION_CHANGED: return "MESSAGE_SCENE_SELECTION_CHANGED";
		case MESSAGE_SCENE_UNSELECT_ALL: return "MESSAGE_SCENE_UNSELECT_ALL";
		case MESSAGE_CONFIG_CHANGED: return "MESSAGE_CONFIG_CHANGED";
		case MESSAGE_ACTION_CHANGED: return "MESSAGE_ACTION_CHANGED";
		case MESSAGE_TRAJECTORY_CHANGED: return "MESSAGE_TRAJECTORY_CHANGED";
		case MESSAGE_TRAJECTORY_PART_CHANGED: return "MESSAGE_TRAJECTORY_PART_CHANGED";
		case MESSAGE_LOCK_SCENE_OBJECT: return "MESSAGE_LOCK_SCENE_OBJECT";
		case MESSAGE_UNCLOCK_SCENE_OBJECT: return "MESSAGE_UNCLOCK_SCENE_OBJECT";
		case MESSAGE_COMPLEX_ACTION_UPDATED: return "MESSAGE_COMPLEX_ACTION_UPDATED";
	}

	return "MESSAGE_UNKNOW";
}

EditorMessageManager::ListenerContainer EditorMessageManager::m_Listeners = EditorMessageManager::create();


