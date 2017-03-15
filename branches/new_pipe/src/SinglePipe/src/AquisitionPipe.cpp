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

void AquisitionPipe::visualize()
{
	std::lock_guard<std::mutex> lock(m_VisualizeMutex);
	utils::zoomShow("Dbg", m_VisualizationFrame, m_ZoomObjects);
	cv::waitKey(1);
}

bool AquisitionPipe::start()
{

	try {
        m_fbSeparator.reset();
        m_FrameReader = std::make_shared<utils::camera::AxisRawReader>(m_Params.ip,
                             m_Params.user + ":" + m_Params.password, m_Params.fps,
                             m_Params.compression, m_Params.frameWidth, m_Params.frameHeight);

		if (config::PathDetection::getInstance().isPredictedRoiDetector())
			m_PathDetector = std::make_shared<PredictedRoiPathDetector>();
		else
			m_PathDetector = std::make_shared<DefaultPathDetector>();
		m_PathDetector->initialize();
		m_PathDetector->setPathStream(m_PathStream);
		m_StreamPathAnalysis.loadParameters();
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
        // for current frame
		utils::camera::MJPGFrame jpegFrame;
		m_FrameReader->popRawFrame(jpegFrame);
        auto frame = sequence::Video::Frame(jpegFrame.m_TimeStamp, std::move(jpegFrame.m_JPEGFrame));

        auto crumbles = m_fbSeparator.separate(frame.getCvMat());
        //auto pair = std::make_pair(frame, crumbles);
        //AquisitionFifo::frames.push(pair);

        // detecting paths
		m_PathStream.addFrame();
		m_PathDetector->setCrumbles(std::move(crumbles));
		m_PathDetector->processFrame(frame.getCvMat());
		std::map<sequence::PathStream::Id, sequence::PathStream::Path> paths;
		m_PathStream.getPaths(paths);

		std::vector<sequence::Cluster> clusters = m_StreamPathAnalysis.processFrame(paths);

		utils::ZoomObjectCollection zoomObjects;
		m_PathDetector->visualize(zoomObjects);
		m_StreamPathAnalysis.visualize(zoomObjects);
		std::lock_guard<std::mutex> lock(m_VisualizeMutex);
		m_ZoomObjects = zoomObjects;
		frame.getCvMat().copyTo(m_VisualizationFrame);
	}
}


