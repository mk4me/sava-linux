#pragma once
#ifndef AquisitionPipe_h__
#define AquisitionPipe_h__

#include <utils/IAppModule.h>
#include <utils/CvZoom.h>
#include <boost/thread.hpp>

#include <atomic>
#include <QtCore/QObject>
#include <utils/AxisRawReader.h>
#include <utils/BlockingQueue.h>
#include <sequence/Video.h>
#include <sequence/BackgroundSeparation.h>
#include <sequence/PathStream.h>
#include <PathAnalysisAlgorithms/StreamAnalyzer.h>
#include <tbb/concurrent_queue.h>


namespace utils
{
	namespace camera
	{
		class ICameraRawReader;
	}
}

namespace  clustering {
	class IPathDetector;
}

class AquisitionFifo
{
public:
	typedef std::pair<boost::posix_time::ptime, cv::Mat> Frame;
	typedef std::shared_ptr<Frame> FramePtr;
    typedef tbb::concurrent_queue<FramePtr> FramesFIFO;
	FramesFIFO frames;
};

struct AquisitionParams
{
	std::string ip;
	std::string user;
	std::string password;
	size_t fps;
	size_t compression = 30;
	size_t frameWidth = utils::camera::AxisRawReader::DEFAULT_WIDTH;
	size_t frameHeight = utils::camera::AxisRawReader::DEFAULT_HEIGHT;
};

class AquisitionPipe
{
public:
	AquisitionPipe(const AquisitionParams& params, const std::shared_ptr<AquisitionFifo>& fifo);
	virtual ~AquisitionPipe() {}

public:
	bool start();
	void stop();
	bool isRunning() const;
private:
	AquisitionParams m_Params;
	std::shared_ptr<utils::camera::ICameraRawReader> m_FrameReader;
	std::atomic<bool> m_AquisitionRunning;
	boost::thread m_AquisitionThread;
    std::shared_ptr<AquisitionFifo> m_fifo;
private:
	void aquisitionThreadFunc();
};

#endif // Aquisition_h__
