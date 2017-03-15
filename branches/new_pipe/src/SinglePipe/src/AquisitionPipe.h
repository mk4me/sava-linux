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
#include <tbb/concurrent_vector.h>
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
	typedef std::pair<sequence::Video::Frame, std::vector<cv::Rect>> FrameT;
	static utils::BlockingQueue<FrameT> frames;
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
	AquisitionPipe(const AquisitionParams& params);
	virtual ~AquisitionPipe() {}

public:
	bool start();
	void stop();
	bool isRunning() const;
	void visualize();
private:
	AquisitionParams m_Params;
	std::shared_ptr<utils::camera::ICameraRawReader> m_FrameReader;
	std::atomic<bool> m_AquisitionRunning;
	boost::thread m_AquisitionThread;
	void aquisitionThreadFunc();

	sequence::FBSeparator m_fbSeparator;


	std::shared_ptr<clustering::IPathDetector> m_PathDetector;
	sequence::PathStream m_PathStream;
	clustering::StreamAnalyzer m_StreamPathAnalysis;

	bool m_Visualize;
	std::mutex m_VisualizeMutex;
	cv::Mat m_VisualizationFrame;
	utils::ZoomObjectCollection m_ZoomObjects;
};

#endif // Aquisition_h__