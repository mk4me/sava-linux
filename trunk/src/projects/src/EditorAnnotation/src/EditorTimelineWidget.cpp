#include "EditorTimelineWidget.h"
#include <QtCore/QDebug>
#include "EditorMessageManager.h"

EditorTimelineWidget::EditorTimelineWidget(QWidget *parent)
: QWidget(parent)
{
	ui.setupUi(this);

	m_Source = EditorTimeline::getPointer();

	initGui();

	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_START_PLAYING, this);
	EditorMessageManager::registerMessage(MESSAGE_STOP_PLAYING, this);
	EditorMessageManager::registerMessage(MESSAGE_PLAYING_DIRECTION_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_NEW_SEQUENCE_LOAD_FINISH, this);
}

/*--------------------------------------------------------------------*/
EditorTimelineWidget::~EditorTimelineWidget()
{
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorTimelineWidget::initGui()
{
	m_PlayIcon = getIcon(QStyle::SP_MediaPlay);
	m_StopIcon = getIcon(QStyle::SP_MediaPause);
	ui.m_PlayStopButton->setIcon(m_PlayIcon);
	ui.m_PlayStopButton->setToolTip("Play");

	ui.m_NextFrameButton->setIcon(getIcon(QStyle::SP_MediaSeekForward));
	ui.m_NextFrameButton->setToolTip("Next Frame");

	ui.m_PrevFrameButton->setIcon(getIcon(QStyle::SP_MediaSeekBackward));
	ui.m_PrevFrameButton->setToolTip("Prev Frame");

	ui.m_SkipForwardButton->setIcon(getIcon(QStyle::SP_MediaSkipForward));
	ui.m_SkipForwardButton->setToolTip("Skip Forward");

	ui.m_SkipBackwardButton->setIcon(getIcon(QStyle::SP_MediaSkipBackward));
	ui.m_SkipBackwardButton->setToolTip("Skip Backward");

	m_DirectionForwardIcon = getIcon(QStyle::SP_ArrowRight);
	m_DirectionBackwardIcon = getIcon(QStyle::SP_ArrowLeft);
	ui.m_DirectionButton->setIcon(m_DirectionForwardIcon);
	ui.m_DirectionButton->setToolTip("Change Direction");

	ui.m_SpeedSpinBox->setValue(m_Source->getSpeed());
	ui.m_SpeedSlider->setValue(m_Source->getSpeed() * 10);
}

/*--------------------------------------------------------------------*/
QIcon EditorTimelineWidget::getIcon(QStyle::StandardPixmap i_Style)
{
	return style()->standardIcon(i_Style);
}

/*--------------------------------------------------------------------*/
void EditorTimelineWidget::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
		case MESSAGE_START_PLAYING:
		case MESSAGE_STOP_PLAYING:
			ui.m_PlayStopButton->setIcon(m_Source->isPlaying() ? m_StopIcon : m_PlayIcon);
			break;

		case MESSAGE_PLAYING_DIRECTION_CHANGED:
			ui.m_DirectionButton->setIcon(m_Source->isPlayBackward() ? m_DirectionBackwardIcon : m_DirectionForwardIcon);
			break;

		case MESSAGE_NEW_SEQUENCE_LOAD_FINISH:
			if (i_ExtraParam != 0) {
				m_Source->setFrameCount(EditorSequencesManager::getInstance().getSequence()->getVideo()->getNumFrames());
				m_Source->setCurrentFrame(0);
				updateFrameInfoLabel();
			}
			break;

		case MESSAGE_FRAME_CHANGED:
			updateFrameInfoLabel();
			break;
	}
}

/*--------------------------------------------------------------------*/
void EditorTimelineWidget::speedSliderChanged(int i_Value)
{
	m_Source->setSpeed(i_Value / 10.0);
	ui.m_SpeedSpinBox->setValue(i_Value / 10.0);
}

/*--------------------------------------------------------------------*/
void EditorTimelineWidget::speedSpinBoxChanged(double i_Value)
{
	m_Source->setSpeed(i_Value);
	ui.m_SpeedSlider->setValue(i_Value * 10);
}

/*--------------------------------------------------------------------*/
void EditorTimelineWidget::updateFrameInfoLabel()
{
	ui.m_FrameInfoLable->setText(QString::number(m_Source->getCurrentFrame() + 1) + "/" + QString::number(m_Source->getFrameCount()));
}




