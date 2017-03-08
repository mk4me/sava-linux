#include "AquisitionPipe.h"

#include <sequence/Video.h>

#include <config/Aquisition.h>
#include <config/Diagnostic.h>

#include <utils/Filesystem.h>
#include <utils/Application.h>
#include <utils/AxisRawReader.h>
#include <utils/PipeProcessUtils.h>
#include <utils/FileLock.h>

#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>
#include <config/PathDetection.h>
#include <PathDetectionLib/DefaultPathDetector.h>
#include <PathDetectionLib/PredictedRoiPathDetector.h>
#include <sequence/Cluster.h>
#include <vector>

using namespace clustering;

utils::BlockingQueue<AquisitionFifo::FrameT> AquisitionFifo::frames;

AquisitionPipe::AquisitionPipe(const AquisitionParams &params) :
	m_Params(params)
{

}

bool AquisitionPipe::isRunning() const
{
    return m_AquisitionRunning;
}

bool AquisitionPipe::start()
{

	try {
        m_separator.reset();
        m_FrameReader = std::make_shared<utils::camera::AxisRawReader>(m_Params.ip,
                             m_Params.user + ":" + m_Params.password, m_Params.fps,
                             m_Params.compression, m_Params.frameWidth, m_Params.frameHeight);

		if (config::PathDetection::getInstance().isPredictedRoiDetector())
			m_PathDetector = std::make_shared<PredictedRoiPathDetector>();
		else
			m_PathDetector = std::make_shared<DefaultPathDetector>();
		m_PathDetector->initialize();
		m_PathDetector->setPathStream(m_PathStream);
		m_StreamAnalyzer.loadParameters();
		m_AquisitionRunning = true;
		boost::thread t(&AquisitionPipe::aquisitionThreadFunc, this);
		m_AquisitionThread.swap(t);


	}
	catch (const std::exception& e)
	{
		std::cerr << "Aquisition::start() error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Aquisition::start() Unknown error:" << std::endl;
	}

	return true;
}

void AquisitionPipe::stop()
{
	m_AquisitionRunning = false;
	if (m_AquisitionThread.joinable())
		m_AquisitionThread.join();
	std::cout << "Quitting..." << std::endl;
}

void AquisitionPipe::aquisitionThreadFunc()
{
	while (m_AquisitionRunning)
	{
		utils::camera::RawMJPGFrame frame;
		m_FrameReader->popRawFrame(frame);
        auto conv = sequence::Video::Frame(frame.m_TimeStamp, std::move(frame.m_RawFrame));
        auto rects = m_separator.separate(conv.getImage());
        auto pair = std::make_pair(conv, rects);
        AquisitionFifo::frames.push(pair);

		//video.addFrame(frame.m_TimeStamp, std::move(frame.m_RawFrame));

		m_PathStream.addFrame();
		m_PathDetector->setCrumbles(std::move(rects));
		m_PathDetector->processFrame(conv.getImage());
		std::map<sequence::PathStream::Id, sequence::PathStream::Path> paths;
		m_PathStream.getPaths(paths);
		std::vector<sequence::Cluster> clusters = m_StreamAnalyzer.processFrame(paths);

	}
}


