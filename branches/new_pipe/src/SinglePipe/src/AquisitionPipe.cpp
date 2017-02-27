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
	}
}


