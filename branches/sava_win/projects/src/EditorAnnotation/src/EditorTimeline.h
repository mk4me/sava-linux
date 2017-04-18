#pragma once
#ifndef _EDITOR_TIMELINE_H_
#define _EDITOR_TIMELINE_H_


#include "assert.h"
#include <utils/Singleton.h>
#include "EditorMessageManager.h"
// sava-linux.rev
//#include "boost/detail/winapi/time.hpp"

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QElapsedTimer>
#include <QtCore/QDebug>


template<typename Type>
static Type clamp(Type value, Type minVal, Type maxVal) {
	return value < minVal ? minVal : (value > maxVal ? maxVal : value);
}


/// <summary>
/// Klasa zarz¹dzaj¹ca czasem wideo.
/// </summary>
/// <seealso cref="Singleton{EditorTimeline}" />
class EditorTimeline : public QObject, public Singleton < EditorTimeline >
{
	Q_OBJECT

	static const int FRAMES_PER_SECOND = 25;

public:
	EditorTimeline();

	bool isFrameValid(int i_FrameNr);

	int getCurrentFrame() const { return m_CurrentFrame; }
	void setCurrentFrame(int i_FrameNr);

	int getFrameCount() const { return m_FrameCount; }
	void setFrameCount(int i_FrameCount);

	float getSpeed() const { return m_Speed; }
	void setSpeed(float i_Speed);

	bool isPlayBackward() const { return m_IsPlayBackward; }
	void setPlayBackward(bool i_PlayBackward);

	bool isPlaying() const { return m_IsPlaying; }
	void play();
	void stop();

	float getProgress() const;

	void nextFrame();
	void prevFrame();
	void skipForward();
	void skipBackward();
	void tooglePlay();
	void toogleDirection();

private slots:
	void onTick();

private:
	void tick();
	bool onFrameUpdate(size_t delta);
	bool setFrame(int frameNr);

	int timeToFrame(size_t time) const;
	size_t frameToTime(int frameNr) const;

private:
	int m_CurrentFrame;
	int m_FrameCount;
	bool m_IsPlaying;
	bool m_IsPlayBackward;
	float m_Speed;

	size_t m_CurrentTime;		//[in miliseconds]
	size_t m_SavedTickCount;	//[in miliseconds]

	QTimer m_TickTimer;
};

#endif // _EDITOR_TIMELINE_H_