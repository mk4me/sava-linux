#include "MonitorFrames.h"
#include "boost\detail\winapi\time.hpp"

MonitorFrames::MonitorFrames(QObject *parent)
	: QObject(parent)
	, m_CurrentFrame(0)
	, m_TickEnabled(false)
{
	m_TickTimer = new QTimer(this);
	
	connect(m_TickTimer, SIGNAL(timeout()), this, SLOT(onTick()));

	m_TickTimer->setInterval(20);
	m_TickTimer->start();
}



void MonitorFrames::setFramesTimes(const std::vector<boost::posix_time::ptime>& _newFramesTimes)
{
	assert(!_newFramesTimes.empty() && "Vector of frames times is empty");

	tickDisabled();

	m_FramesTimes.clear();
	m_FramesTimes = _newFramesTimes;
	m_CurrentFrame = 0;

	tickEnabled();
}


size_t MonitorFrames::timeToFrame(const boost::posix_time::ptime& time, bool *ok /* = nullptr */) const
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


void MonitorFrames::onTick()
{
	if (!m_TickEnabled)
		return;

	boost::posix_time::ptime l_CurrentTime(boost::posix_time::microsec_clock::local_time());

	bool ok = false;
	size_t frameNr = timeToFrame(l_CurrentTime, &ok);
	if (ok)
	{
		if (m_CurrentFrame != frameNr)
		{
			m_CurrentFrame = frameNr;
			emit frameChanged(m_CurrentFrame);
		}
	}
	else
	{
		emit frameOutOfRange();
		tickDisabled();
	}
}
