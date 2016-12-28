#pragma once
#include <stddef.h>

class MonitorQueueSpeeder
{
	
public:
	MonitorQueueSpeeder();

	void update(size_t queneSize);

	size_t getQueueSize() const { return m_QueueSize; }
	float getSpeed() const { return m_Speed; }
	bool isOffsetResetEnabled() const { return m_ForceOffsetReset; }

	void setEnabled(bool enabled) { m_IsEnabled = enabled; }

private:
	float getSpeedByQueueSize() const;

private:
	bool m_IsEnabled;

	size_t m_MinQueueSize;
	size_t m_MinMiddleQueueSize;
	size_t m_MaxMiddleQueueSize;
	size_t m_MaxQueueSize;

	float m_MaxSpeed;
	float m_MinSpeed;

	size_t m_QueueSize;
	float m_Speed;
	bool m_ForceOffsetReset;
};

