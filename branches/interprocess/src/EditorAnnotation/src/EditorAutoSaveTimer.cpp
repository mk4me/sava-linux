#include "EditorAutoSaveTimer.h"

/*--------------------------------------------------------------------*/
EditorAutoSaveTimer::EditorAutoSaveTimer(QObject *parent)
	: QTimer(parent)
	, m_IsEnabled(false)
{
	EditorMessageManager::registerMessage(MESSAGE_CONFIG_CHANGED, this);
}

/*--------------------------------------------------------------------*/
EditorAutoSaveTimer::~EditorAutoSaveTimer()
{
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorAutoSaveTimer::update()
{
	m_IsEnabled = EditorConfig::getInstance().isAutoSaveEnabled();

	int l_Interval = EditorConfig::getInstance().getAutoSaveTimerInterval();
	setInterval(l_Interval);

	if (m_IsEnabled)
		start();
	else
		stop();
}

/*--------------------------------------------------------------------*/
void EditorAutoSaveTimer::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	if (i_Message == MESSAGE_CONFIG_CHANGED)
	{
		update();
	}
}
