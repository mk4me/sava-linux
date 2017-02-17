#include "ReplayTimer.h"

ReplayTimer::ReplayTimer(QObject *parent)
	: QObject(parent)
	, m_CurrentFrame(0)
	, m_FirstFrame(0)
	, m_LastFrame(0)
{
	m_Timer.setInterval(20);

	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTick()));
}

ReplayTimer::~ReplayTimer()
{

}

void ReplayTimer::init(const std::vector<boost::posix_time::ptime>& _newFramesTimes, const boost::posix_time::ptime& _beginTime, const boost::posix_time::ptime& _endTime)
{
	m_FramesTimes.clear();
	m_FramesTimes = _newFramesTimes;

	auto beginIt = std::lower_bound(m_FramesTimes.begin(), m_FramesTimes.end(), _beginTime);
	m_FirstFrame = (beginIt != m_FramesTimes.end()) ? (beginIt - m_FramesTimes.begin()) : 0;

	auto endIt = std::lower_bound(m_FramesTimes.begin(), m_FramesTimes.end(), _endTime);
	m_LastFrame = (endIt != m_FramesTimes.end()) ? (endIt - m_FramesTimes.begin()) : (m_FramesTimes.size() - 1);

	m_CurrentFrame = m_FirstFrame;

	resetOffset();
}

void ReplayTimer::start()
{
	if (!isPlaying())
	{
		resetOffset();
		m_Timer.start();
		emit started();
	}
}

void ReplayTimer::stop()
{
	if (isPlaying())
	{
		m_Timer.stop();
		emit stopped();
	}
}

void ReplayTimer::replay()
{
	stop();
	m_CurrentFrame = m_FirstFrame;
	start();
}

bool ReplayTimer::isPlaying() const{
	return m_Timer.isActive();
}

bool ReplayTimer::isFinished() const{
	return (m_CurrentFrame >= m_LastFrame);
}

void ReplayTimer::setCurrentFrame(size_t _frame)
{
	if (_frame != m_CurrentFrame)
	{
		m_CurrentFrame = _frame;
		emit frameChanged(m_CurrentFrame);
	}
}

void ReplayTimer::onTick()
{
	boost::posix_time::ptime l_CurrentTime(boost::posix_time::microsec_clock::local_time() - m_Offset);

	bool ok = false;
	size_t frameNr = timeToFrame(l_CurrentTime, &ok);
	if (ok && isValid(frameNr))
	{
		setCurrentFrame(frameNr);
	}
	else
	{
		stop();
		setCurrentFrame(m_LastFrame);
		emit frameOutOfRange();
	}
}

bool ReplayTimer::isValid(size_t _frame) const {
	return (_frame >= m_FirstFrame) && (_frame <= m_LastFrame);
}

void ReplayTimer::resetOffset() {

	assert(m_FramesTimes.size() >= m_CurrentFrame + 1);

	m_Offset = boost::posix_time::microsec_clock::local_time() - m_FramesTimes.at(m_CurrentFrame);
}

size_t ReplayTimer::timeToFrame(const boost::posix_time::ptime& time, bool *ok /* = nullptr */) const
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

