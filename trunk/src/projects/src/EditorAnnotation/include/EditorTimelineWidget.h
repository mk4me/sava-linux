#ifndef EDITORTIMELINEWIDGET_H
#define EDITORTIMELINEWIDGET_H

#include "EditorSingletons.h"
#include "ui_EditorTimelineWidget.h"
#include "EditorMessageManager.h"
#include <QtWidgets/QWidget>
#include <QtCore/QDebug>
#include <QtWidgets/QStyle>
#include "EditorTimeline.h"

/**
* \class EditorTimelineWidget
*
* \brief Widget to show and manage sequence time.
*
*/
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
	void skipForward()  { m_Source->skipForward(); }
	void skipBackward() { m_Source->skipBackward(); }
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
};

#endif // EDITORTIMELINEWIDGET_H
