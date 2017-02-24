#include "sequence/GpuPacker.h"

#include <sequence/GpuVideo.h>

#include <opencv2/video.hpp>

GpuPacker::GpuPacker(int imageCompression, int backgroundHistory /*= 300*/, int differenceThreshold /*= 20*/, float newBackgroundMinPixels /*= 0.2*/, int minCrumbleArea /*= 100*/, int mergeCrumblesIterations /*= 3*/, const cv::Mat& mask /*= cv::Mat()*/) :
    m_ImageCompression(imageCompression),
    m_separator(backgroundHistory, differenceThreshold, newBackgroundMinPixels, minCrumbleArea, mergeCrumblesIterations, mask)
	//, m_BackgroundHistory(backgroundHistory)	// 300
	//, m_DifferenceThreshold(differenceThreshold)	// 20
	//, m_NewBackgroundMinPixels(newBackgroundMinPixels)		// 0.2
	//, m_MinCrumbleArea(minCrumbleArea)		// 100
	//, m_MergeCrumblesIterations(mergeCrumblesIterations)	// 3
	//, m_CameraMask(mask)
{

}

void GpuPacker::createSequence()
{
	m_GpuVideo = std::make_shared<sequence::GpuVideo>(m_ImageCompression);

	//if (!m_BackgroundSubtractor)
	//	resetBackgroundSubtractor();
	if (!m_separator.wasIniialized()) {
		resetBackgroundSubtractor();
	}
}

void GpuPacker::compressFrame(size_t frameId, const cv::Mat& frame, sequence::IVideo::Timestamp timestamp)
{
	std::vector<cv::Rect> crumbles;
	try
	{
	    auto rects = m_separator.separate(frame);
		m_GpuVideo->addFrame(timestamp, frame, rects);
	}
	catch (...)
	{
		m_GpuVideo->addFrame(timestamp, frame, crumbles);
		throw;
	}	
}

void GpuPacker::save(const std::string& filename)
{
	if (!m_GpuVideo)
		return;
	m_GpuVideo->save(filename);
	m_GpuVideo.reset();
	std::cout << std::endl << "Post-processed file " << filename << " saved." << std::endl;
}

void GpuPacker::resetBackgroundSubtractor()
{
	//m_BackgroundSubtractor = cv::createBackgroundSubtractorMOG2(m_BackgroundHistory);
    m_separator.reset();
}
