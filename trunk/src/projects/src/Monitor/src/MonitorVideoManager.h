#ifndef CLUSTERMANAGER_H
#define CLUSTERMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>

#include "MonitorPipe.h"
#include "sequence/MetaVideo.h"
#include "sequence/IVideo.h"
#include "utils/BlockingQueue.h"


#define PROFILE_OFFSET_TIME 0


class MonitorVideoManager : public QObject, public Singleton<MonitorVideoManager>
{
	Q_OBJECT

public: 
	typedef std::pair<std::shared_ptr<sequence::Cluster>, std::shared_ptr<sequence::Action>> ActionPair;
	typedef std::vector<ActionPair> ActionPairVec;

	MonitorVideoManager(QObject *parent = 0); 
	~MonitorVideoManager();

	std::shared_ptr<sequence::IVideo> getVideo() const;
	const std::vector<sequence::IVideo::Timestamp>& getVideoTimes() { return m_VideoTimes; }
	std::string getVideoPath() const;

	const ActionPairVec& getClusterActionPairs() const { return m_SortedObjectsVec; }
	ActionPairVec getClusterActionPairs(size_t _frameNr) const;

	boost::posix_time::time_duration getOffsetTime() { return m_OffsetTime; }

	bool isSynchronizeNecessary() const;

public slots:
	void start();
	void load();
	void synchronize();

signals:
	void loaded();
	void synchronized();

private slots:
	void onLoaded();

private:
	//current video object
	std::shared_ptr<sequence::MetaVideo> m_MetaVideo;

	//current video time stamps
	std::vector<sequence::IVideo::Timestamp> m_VideoTimes;

	//current video time offset
	boost::posix_time::time_duration m_OffsetTime;

	//objects from current video (clusters + actions)
	ActionPairVec m_SortedObjectsVec;

	//thread to load new video sequence
	boost::thread m_LoadThread;

	//reset offset time 
	bool m_ResetOffsetEnabled;

	//synchronizing flag
	bool m_IsSynchronizing;

	void calcInnerData();

	void loadingProcess();
	void startLoadingProcess();
	void stopLoadingProcess();
	bool isLoadingProcessRunning() const;

#if PROFILE_OFFSET_TIME
	QElapsedTimer m_Profile_ElapsedTimer;
	QFile m_Profile_ResultsFile;
#endif
};

#endif // CLUSTERMANAGER_H
