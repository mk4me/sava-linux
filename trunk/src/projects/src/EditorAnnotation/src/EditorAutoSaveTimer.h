#pragma once
#ifndef EDITORAUTOSAVETIMER_H
#define EDITORAUTOSAVETIMER_H

#include <QtCore/QTimer>

#include "EditorSingletons.h"
#include "EditorMessageManager.h"

class EditorAutoSaveTimer : public QTimer, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorAutoSaveTimer(QObject *parent);
	~EditorAutoSaveTimer();

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private:
	void update();

private:
	bool m_IsEnabled;
	
};

#endif // EDITORAUTOSAVETIMER_H
