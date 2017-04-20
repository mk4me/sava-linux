#include "MonitorQueueSpeeder.h"
#include <QtCore/QDebug>

#include "config/Monitor.h"
#include "utils/Log.h"



MonitorQueueSpeeder::MonitorQueueSpeeder()
	: m_IsEnabled(true)
	, m_Speed(1.f)
	, m_QueueSize(0)
{
	auto& monitorConfig = config::Monitor::getInstance();

	m_MinQueueSize = monitorConfig.getMinQueueSize();
	m_MaxQueueSize = monitorConfig.getMaxQueueSize();
	m_MinSpeed = 1.f / monitorConfig.getTimeScale();
	m_MaxSpeed = 1.f * monitorConfig.getTimeScale();
	

	float len = (m_MaxQueueSize - m_MinQueueSize + 1);

	m_MinMiddleQueueSize = m_MinQueueSize + len / 3.f;
	m_MaxMiddleQueueSize = m_MaxQueueSize - len / 3.f;
}


void MonitorQueueSpeeder::update(size_t queueSize)
{
	static bool warming = true;
	static int warmCount = 0;
	if (warming && warmCount++ > 3) {
		warming = false;
	}


	m_QueueSize = queueSize;

	if (m_IsEnabled && !warming)
	{
		m_Speed = getSpeedByQueueSize();
		m_ForceOffsetReset = (m_Speed != 1);
	}
	else
	{
		m_ForceOffsetReset = (m_QueueSize == 0);
		m_Speed = 1;
	}
}

float MonitorQueueSpeeder::getSpeedByQueueSize() const
{
	if (m_Speed == 1)
	{
		if (m_QueueSize >= m_MaxQueueSize)
			return m_MaxSpeed;

		if (m_QueueSize <= m_MinQueueSize)
			return m_MinSpeed;
	}
	else if (m_Speed > 1)
	{
		if (m_QueueSize <= m_MaxMiddleQueueSize)
			return 1;
	}
	else
	{
		if (m_QueueSize >= m_MinMiddleQueueSize)
			return 1;
	}

	return m_Speed;
}
