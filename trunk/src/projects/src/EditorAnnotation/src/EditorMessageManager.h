#pragma once
#ifndef EDITOR_COMMAND_MANAGER_H_
#define EDITOR_COMMAND_MANAGER_H_

#include <vector>
#include <set>
#include <map>

//@fixme dave (Message must by an object, not only enumerator (becouse we must send also some parameters)

/** \brief enums represents concrete message */
enum EditorMessageType
{
	MESSAGE_PREPARE_FRAME_CHANGE,
	MESSAGE_FRAME_CHANGED,
	MESSAGE_START_PLAYING,
	MESSAGE_STOP_PLAYING,
	MESSAGE_PLAYING_DIRECTION_CHANGED,
	MESSAGE_SEQUENCE_LOADED,
	MESSAGE_SCENE_OBJECT_UPDATE,
	MESSAGE_SCENE_OBJECT_CREATED,
	MESSAGE_SCENE_OBJECT_DELETED,
	MESSAGE_SCENE_SELECTION_CHANGED,
	MESSAGE_SCENE_UNSELECT_ALL,
	MESSAGE_CONFIG_CHANGED,
	MESSAGE_ACTION_CHANGED,
	MESSAGE_TRAJECTORY_CHANGED,
	MESSAGE_TRAJECTORY_PART_CHANGED,
	MESSAGE_LOCK_SCENE_OBJECT,
	MESSAGE_UNCLOCK_SCENE_OBJECT,
	MESSAGE_COMPLEX_ACTION_UPDATED,
	MESSAGE_MAX,
};

/**
* \class EditorMessageListener
*
* \brief Listener, that listen messages
*
*/
class EditorMessageListener
{
public:
	/**
	 *  \brief function to ovveride, when whant to catch message 
	 *  \param i_Message type of catched message
	 *  \param data i_ExtraParam data from message
	 */
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) = 0;
};


/**
* \class EditorMessageManager
*
* \brief Class to manage messages.
*
*/
class EditorMessageManager
{
public:
	/** 
	 * \brief register message 
	 * \param i_Type registering type of message
	 * \param i_Listener registering message listener
	 */
	static void registerMessage(EditorMessageType i_Type, EditorMessageListener* i_Listener);

	/**
	* \brief unregister message
	* \param i_Type unregistering type of message
	* \param i_Listener unregistering message listener
	*/
	static void unregisterMessage(EditorMessageType i_Type, EditorMessageListener* i_Listener);

	/**
	* \brief unregister all listener messages
	* \param i_Listener unregistering messages from this listener
	*/
	static void unregisterMessages(EditorMessageListener* i_Listener);

	/**
	* \brief send message
	* \param i_Type sending message
	* \param data sending data with this message
	*/
	static void sendMessage(EditorMessageType i_Type, int i_ExtraParam = -1);

	/**
	* \brief get message name by its type
	* \param i_Message type of message
	* \return message name
	*/
	static std::string getMessageName(EditorMessageType i_Message);

private:
	typedef std::vector<std::set<EditorMessageListener*>> ListenerContainer;
	static ListenerContainer create();
	static ListenerContainer m_Listeners;
};


#endif // EDITOR_COMMAND_MANAGER_H_
