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

AquisitionPipe::AquisitionPipe(const AquisitionParams &params, const std::shared_ptr<AquisitionFifo>& fifo) :
	m_Params(params),
    m_fifo(fifo)
{

}

bool AquisitionPipe::isRunning() const
{
    return m_AquisitionRunning;
}

bool AquisitionPipe::start()
{
	try {
        m_FrameReader = std::make_shared<utils::camera::AxisRawReader>(m_Params.ip,
                             m_Params.user + ":" + m_Params.password, m_Params.fps,
                             m_Params.compression, m_Params.frameWidth, m_Params.frameHeight);

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
        auto frame = std::make_shared<sequence::Video::Frame>(jpegFrame.m_TimeStamp, std::move(jpegFrame.m_JPEGFrame));
		//auto p1 = jpegFrame.m_TimeStamp;
		//auto p2 = frame->getCvMat();
		//AquisitionFifo::FramePtr::element_type pair = AquisitionFifo::FramePtr::element_type(p1,p2);
		//auto ptr = std::shared_ptr<AquisitionFifo::FramePtr>(std::make_pair(jpegFrame.m_TimeStamp, frame->getCvMat()));
		AquisitionFifo::FramePtr ptr(new AquisitionFifo::Frame(jpegFrame.m_TimeStamp, frame->getCvMat()));
        m_fifo->frames.push(ptr);
	}
}


