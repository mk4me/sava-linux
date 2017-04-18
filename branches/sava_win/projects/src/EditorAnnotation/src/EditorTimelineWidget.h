#pragma once
#ifndef EDITORTIMELINEWIDGET_H
#define EDITORTIMELINEWIDGET_H

#include "EditorSingletons.h"
#include "ui_EditorTimelineWidget.h"
#include "EditorMessageManager.h"
#include <QtWidgets/QWidget>
#include <QtCore/QDebug>
#include <QtWidgets/QStyle>
#include "EditorTimeline.h"


/// <summary>
/// Kontrolka wywietlaj¹ca i zarz¹dzaj¹ca czasem wideo.
/// </summary>
/// <seealso cref="EditorMessageListener" /> 
class EditorTimelineWidget : public QWidget, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorTimelineWidget(QWidget *parent = 0);
	~EditorTimelineWidget();

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private slots:
	void tooglePlay() { m_Source->tooglePlay(); }
	void toogleDirection() { m_Source->toogleDirection(); }
	void nextFrame()  { m_Source->nextFrame(); }
	void prevFrame()  { m_Source->prevFrame(); }
	void skipForward();
	void skipBackward();
	void speedSpinBoxChanged(double i_Value);
	void speedSliderChanged(int i_Value);

private:
	void initGui();
	void updateFrameInfoLabel();
	QIcon getIcon(QStyle::StandardPixmap i_Style);

private:
	Ui::EditorTimelineWidget ui;

	QIcon m_PlayIcon;
	QIcon m_StopIcon;
	QIcon m_DirectionForwardIcon;
	QIcon m_DirectionBackwardIcon;

	EditorTimeline* m_Source;
	EditorSequencesManager* m_SequenceManager;
};

#endif // EDITORTIMELINEWIDGET_H
