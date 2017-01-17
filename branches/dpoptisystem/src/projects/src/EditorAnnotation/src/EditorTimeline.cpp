
#include "EditorTimeline.h"
#include <utils/timer.h>

/*--------------------------------------------------------------------*/
EditorTimeline::EditorTimeline()
	:m_CurrentFrame(0)
	,m_FrameCount(0)
	,m_IsPlaying(false)
	,m_IsPlayBackward(false)
	,m_Speed(1)
	,m_CurrentTime(0)
{
}


/*--------------------------------------------------------------------*/
bool EditorTimeline::isFrameValid(int i_Frame)
{
	return (m_FrameCount > 0 && i_Frame >= 0 && i_Frame < m_FrameCount);
}

/*--------------------------------------------------------------------*/
void EditorTimeline::setCurrentFrame(int i_FrameNr)
{
	m_CurrentTime = frameToTime(i_FrameNr);
	setFrame(i_FrameNr);
}

/*--------------------------------------------------------------------*/
void EditorTimeline::setFrameCount(int i_FrameCount)
{
	m_FrameCount = i_FrameCount;
}

/*--------------------------------------------------------------------*/
void EditorTimeline::setPlayBackward(bool i_PlayBackward)
{
	/*if (m_IsPlayBackward != i_PlayBackward)
	{
	m_IsPlayBackward = i_PlayBackward;
	EditorMessageManager::sendMessage(MESSAGE_PLAYING_DIRECTION_CHANGED);
	}*/
}

/*--------------------------------------------------------------------*/
void EditorTimeline::setSpeed(float i_Speed)
{
	m_Speed = i_Speed;
	//m_CurrentTime = frameToTime(m_CurrentFrame);
}

/*--------------------------------------------------------------------*/
void EditorTimeline::play()
{
	m_SavedTickCount = utils::getTickCount();
	m_CurrentTime = frameToTime(m_CurrentFrame);

	m_IsPlaying = true;
	tick();
	EditorMessageManager::sendMessage(MESSAGE_START_PLAYING);
}

/*--------------------------------------------------------------------*/
void EditorTimeline::stop()
{
	m_TickTimer.stop();
	m_IsPlaying = false;
	EditorMessageManager::sendMessage(MESSAGE_STOP_PLAYING);
}

/*--------------------------------------------------------------------*/
void EditorTimeline::tooglePlay()
{
	if (m_IsPlaying)
		stop();
	else
		play();
}

/*--------------------------------------------------------------------*/
void EditorTimeline::toogleDirection()
{
	setPlayBackward(!m_IsPlayBackward);
}

/*--------------------------------------------------------------------*/
void EditorTimeline::tick()
{
	m_TickTimer.singleShot(10, this, SLOT(onTick()));
}

/*--------------------------------------------------------------------*/
void EditorTimeline::onTick()
{
	if (!m_IsPlaying)
		return;

	size_t l_TickCount = utils::getTickCount();
	size_t l_Delta = l_TickCount - m_SavedTickCount;
	m_SavedTickCount = l_TickCount;

	if(onFrameUpdate(l_Delta))
		tick();

}

/*--------------------------------------------------------------------*/
bool EditorTimeline::onFrameUpdate(size_t delta)
{
	int l_DirectionSign = m_IsPlayBackward ? -1 : 1;
	m_CurrentTime = m_CurrentTime + l_DirectionSign * delta * m_Speed;

	int l_FrameNr = timeToFrame(m_CurrentTime);

	return setFrame(l_FrameNr);
	//if (!setFrame(l_FrameNr))
	//	stop();
}

/*--------------------------------------------------------------------*/
bool EditorTimeline::setFrame(int frameNr)
{

	bool valid = isFrameValid(frameNr);

	EditorMessageManager::sendMessage(MESSAGE_PREPARE_FRAME_CHANGE, frameNr);

	if (!valid)
		frameNr = clamp(frameNr, 0, m_FrameCount - 1);

	if (m_CurrentFrame != frameNr)
	{
		m_CurrentFrame = frameNr;
		if (valid)
			EditorMessageManager::sendMessage(MESSAGE_FRAME_CHANGED, m_CurrentFrame);
	}

	return valid;
	
}

/*--------------------------------------------------------------------*/
int EditorTimeline::timeToFrame(size_t time) const
{
	return time * 0.001 * FRAMES_PER_SECOND;
}

/*--------------------------------------------------------------------*/
size_t EditorTimeline::frameToTime(int frameNr) const
{
	return (frameNr * 1000.0) / FRAMES_PER_SECOND;
}

/*--------------------------------------------------------------------*/
void EditorTimeline::nextFrame()
{
	if (m_IsPlaying)
		stop();

	setCurrentFrame(m_CurrentFrame + 1);
}

/*--------------------------------------------------------------------*/
void EditorTimeline::prevFrame()
{
	if (m_IsPlaying)
		stop();

	setCurrentFrame(m_CurrentFrame - 1);
}

/*--------------------------------------------------------------------*/
void EditorTimeline::skipForward()
{
	if (m_IsPlaying)
		stop();

	setCurrentFrame(m_FrameCount - 1);
}

/*--------------------------------------------------------------------*/
void EditorTimeline::skipBackward()
{
	if (m_IsPlaying)
		stop();

	setCurrentFrame(0);
}

/*--------------------------------------------------------------------*/
float EditorTimeline::getProgress() const
{
	return (m_FrameCount > 0) ? (float)m_CurrentFrame / ((float)m_FrameCount - 1) : 0;
}




