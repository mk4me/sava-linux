#include "FrameTimer.h"
#include "boost\detail\winapi\time.hpp"

FrameTimer::FrameTimer(QObject *parent)
	: QObject(parent)
	, m_CurrentFrame(0)
	, m_CurrentTime(0)
	, m_IsPlaying(false)
	, m_SavedTickCount(0)
{
	connect(this, SIGNAL(frameChanged(size_t)), this, SLOT(onFrameChanged(size_t)) );
	connect(this, SIGNAL(frameOutOfRange()), this, SLOT(onFrameOutOfRange()));
}

//
FrameTimer::~FrameTimer()
{

}

//
bool FrameTimer::getCurrentFrame() const{
	return m_CurrentFrame;
}

//
bool FrameTimer::isPlaying() const{
	return m_IsPlaying;
}

//
void FrameTimer::setFramesTimes(const std::vector<size_t> _frames)
{
	m_FramesTimes.clear();
	m_FramesTimes = _frames;
}

//
void FrameTimer::play()
{
	m_IsPlaying = true;
	m_SavedTickCount = GetTickCount();
	tick();
}

//
void FrameTimer::stop(){
	m_IsPlaying = false;
}

//
void FrameTimer::tick(){
	QTimer::singleShot(10, this, SLOT(onTick()));
}

//
size_t FrameTimer::timeToFrame(size_t time, bool *ok /* = nullptr */) const
{
	size_t retVal = 0;
	bool isFounded = false;

	if (m_CurrentFrame < m_FramesTimes.size())
	{
		auto foundIt = std::lower_bound(m_FramesTimes.begin() + m_CurrentFrame, m_FramesTimes.end(), time);
		if (foundIt != m_FramesTimes.end())
		{
			isFounded = true;
			retVal = foundIt - m_FramesTimes.begin();
		}
	}

	if (ok) *ok = isFounded;
	return retVal;
}

//
void FrameTimer::onTick()
{
	if (!m_IsPlaying)
		return;

	//calc delta time
	size_t tickCount = GetTickCount();
	size_t delta = tickCount - m_SavedTickCount;
	m_SavedTickCount = tickCount;

	//calc curren time and current frame number
	m_CurrentTime += delta;

	bool ok;
	size_t frameNr = timeToFrame(m_CurrentTime, &ok);
	if (ok)
	{
		if (m_CurrentFrame != frameNr)
		{
			m_CurrentFrame = frameNr;
			emit frameChanged(m_CurrentFrame);
		}

		tick();
	}
	else
	{
		stop();
		emit frameOutOfRange();
	}

}

//
void FrameTimer::onFrameChanged(size_t frame)
{
	qDebug() << frame;
}

//
void FrameTimer::onFrameOutOfRange()
{
	qDebug() << "Change sequence";
}
