#ifndef REPLAYTIMER_H
#define REPLAYTIMER_H

#include <QObject>
#include <QTimer>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <sequence/IVideo.h>

class ReplayTimer : public QObject
{
	Q_OBJECT

public:
	ReplayTimer(QObject *parent);
	~ReplayTimer();

	void init(const std::vector<boost::posix_time::ptime>& _newFramesTimes, 
		const boost::posix_time::ptime& _beginTime,
		const boost::posix_time::ptime& _endTime);

	void start();
	void stop();
	void replay();

	bool isPlaying() const;
	bool isFinished() const;

	size_t getCurrentFrame() const { return m_CurrentFrame; }
	void setCurrentFrame(size_t _frame);

	size_t getFirstFrame() const { return m_FirstFrame; }
	size_t getLastFrame() const { return m_LastFrame; }

signals:
	void started();
	void stopped();
	void frameChanged(size_t frame);
	void frameOutOfRange();

private slots:
	void onTick();

private:
	void resetOffset();
	bool isValid(size_t _frame) const;
	size_t timeToFrame(const boost::posix_time::ptime& time, bool *ok = nullptr) const;

private:
	QTimer m_Timer;

	size_t m_CurrentFrame;
	size_t m_FirstFrame;
	size_t m_LastFrame;

	std::vector<sequence::IVideo::Timestamp> m_FramesTimes;

	boost::posix_time::time_duration m_Offset;
};

#endif // REPLAYTIMER_H
