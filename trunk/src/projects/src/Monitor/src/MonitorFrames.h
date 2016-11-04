#ifndef FRAMETIMER_H
#define FRAMETIMER_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include "sequence/IVideo.h"

#include "utils/Singleton.h"

class MonitorFrames : public QObject, public Singleton<MonitorFrames>
{
	Q_OBJECT

public:
	MonitorFrames(QObject *parent = 0);
	~MonitorFrames(){};

	void tickEnabled() { m_TickEnabled = true; }
	void tickDisabled() { m_TickEnabled = false; }

signals:
	void frameChanged(size_t frame);
	void frameOutOfRange();

public:
	size_t getCurrentFrame() const { return m_CurrentFrame; }
	size_t getFramesSize() const { return m_FramesTimes.size(); }

	void setFramesTimes(const std::vector<boost::posix_time::ptime>& _newFramesTimes);

private slots:
	void onTick();

private:
	size_t timeToFrame(const boost::posix_time::ptime& time, bool *ok = nullptr) const;

private:
	bool m_TickEnabled;

	std::vector<sequence::IVideo::Timestamp> m_FramesTimes;
	size_t m_CurrentFrame;

	QTimer* m_TickTimer;
};

#endif // FRAMETIMER_H
