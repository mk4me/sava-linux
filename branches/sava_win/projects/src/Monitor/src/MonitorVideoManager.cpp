#include "MonitorVideoManager.h"
#include "QtCore/QTimer"
#include "sequence/Cluster.h"
#include "utils/Log.h"


MonitorVideoManager::MonitorVideoManager(QObject *parent)
	: QObject(parent)
	, m_IsSynchronizing(false)
{

#if PROFILE_OFFSET_TIME
	m_Profile_ResultsFile.setFileName("Monitor_Profiler.txt");
	m_Profile_ResultsFile.open(QIODevice::WriteOnly);
	m_Profile_ElapsedTimer.start();
#endif

	connect(this, SIGNAL(loaded()), this, SLOT(onLoaded()));

}


MonitorVideoManager::~MonitorVideoManager(){
	stopLoadingProcess();

#if PROFILE_OFFSET_TIME
	if (m_Profile_ResultsFile.isOpen())
		m_Profile_ResultsFile.close();
#endif

}

/*--------------------------------------------------------------------*/
MonitorVideoManager::ActionPairVec& MonitorVideoManager::getClusterActionPairs(size_t _frameNr) const
{
	static size_t sCurrentFrameNr = SIZE_MAX;
	static MonitorVideoManager::ActionPairVec sCachedPairs;

	if (sCurrentFrameNr != _frameNr)
	{
		sCachedPairs.clear();
		for (ActionPair _pair : m_SortedObjectsVec)
		{
			if (_frameNr < _pair.first->getStartFrame())
				continue;

			if (_frameNr <= _pair.first->getEndFrame())
				sCachedPairs.push_back(_pair);
		}

		sCurrentFrameNr = _frameNr;
	}

	return sCachedPairs;
}


bool MonitorVideoManager::isSynchronizeNecessary() const {
	return MonitorPipe::getInstance().getInputQueueSize() != 0;
}

void MonitorVideoManager::start()
{
	//wait for monitor pipe
	if (MonitorPipe::getPointer() == nullptr){
		QTimer::singleShot(1, this, SLOT(start()));
		return;
	}

	//try to load on start
	load();
}


void MonitorVideoManager::load(){
	startLoadingProcess();
}

void MonitorVideoManager::synchronize()
{
	if (isSynchronizeNecessary())
	{
		while (MonitorPipe::getInstance().getInputQueueSize() != 0)
			MonitorPipe::getInstance().pushDeleteVideo(MonitorPipe::getInstance().popInputVideo());

		m_QueneSpeeder.update(0);
		startLoadingProcess();
	}
}

void MonitorVideoManager::onLoaded()
{
	stopLoadingProcess();
}

void MonitorVideoManager::startLoadingProcess(){
	m_LoadThread = boost::thread(&MonitorVideoManager::loadingProcess, this);
}


void MonitorVideoManager::loadingProcess()
{

	m_QueneSpeeder.update(MonitorPipe::getInstance().getInputQueueSize());

	m_MetaVideo = MonitorPipe::getInstance().popInputVideo();
	if (m_MetaVideo)
	{
		calcInnerData();

		emit loaded();
	}
}

void MonitorVideoManager::stopLoadingProcess()
{
	if (m_LoadThread.joinable())
		m_LoadThread.join();

}


bool MonitorVideoManager::isLoadingProcessRunning() const{
	return m_LoadThread.get_id() != boost::thread::id();
}


void MonitorVideoManager::calcInnerData()
{
	size_t _framesSize = m_MetaVideo->getVideo()->getNumFrames();
	assert(_framesSize != 0);

	//calculate video offset time
	if (m_QueneSpeeder.isOffsetResetEnabled())
	{
		m_OffsetTime = boost::posix_time::microsec_clock::local_time() - m_MetaVideo->getVideo()->getFrameTime(0);

#if PROFILE_OFFSET_TIME
		QTextStream entry(&m_Profile_ResultsFile);
		entry << QString::number(m_Profile_ElapsedTimer.elapsed() / 1000.0f) << ";" << boost::posix_time::to_simple_string(m_OffsetTime).c_str();
		entry << "\n";
		entry.flush();
#endif
	}

	//calculate video times
	m_VideoTimes.clear();
	m_VideoTimes.reserve(_framesSize);
	for (size_t i = 0; i < _framesSize; i++)
		m_VideoTimes.push_back(m_MetaVideo->getVideo()->getFrameTime(i) + m_OffsetTime);

	//sort cluster action pairs
	size_t objectsNum = m_MetaVideo->getObjectsNum();
	m_SortedObjectsVec.clear();
	m_SortedObjectsVec.reserve(objectsNum);
	for (size_t i = 0; i < objectsNum; i++)
		m_SortedObjectsVec.push_back(std::make_pair(m_MetaVideo->getObjectCluster(i), m_MetaVideo->getObjectAction(i)) );

	std::sort(m_SortedObjectsVec.begin(), m_SortedObjectsVec.end(), [](ActionPair& a, ActionPair& b) {
		return a.first->getStartFrame() < b.first->getStartFrame();
	});	
}


