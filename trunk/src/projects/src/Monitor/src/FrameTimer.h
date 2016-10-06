#ifndef FRAMETIMER_H
#define FRAMETIMER_H

#include <QObject>
#include <QTimer>
#include <QDebug>

class FrameTimer : public QObject
{
	Q_OBJECT

public:
	FrameTimer(QObject *parent = 0);
	~FrameTimer();

signals:
	void frameChanged(size_t frame);
	void frameOutOfRange();

public:
	bool isPlaying() const;
	bool getCurrentFrame() const;
	void setFramesTimes(const std::vector<size_t> _frames);

public slots:
	void play();
	void stop();

private slots:
	void onTick();
	void onFrameChanged(size_t frame);
	void onFrameOutOfRange();

private:
	void tick();
	size_t timeToFrame(size_t time, bool *ok = nullptr) const;

private:
	bool m_IsPlaying;

	size_t m_CurrentFrame;
	size_t m_CurrentTime;
	std::vector<size_t> m_FramesTimes;

	size_t m_SavedTickCount;

};

#endif // FRAMETIMER_H
