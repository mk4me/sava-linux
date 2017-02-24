#include "sequence/DefaultPacker.h"

#include <sequence/CompressedVideo.h>

#include <opencv2/video.hpp>

#include <opencv2/cudabgsegm.hpp>

DefaultPacker::DefaultPacker(int imageCompression, int backgroundHistory /*= 300*/, int differenceThreshold /*= 20*/, float newBackgroundMinPixels /*= 0.2*/, int minCrumbleArea /*= 100*/, int mergeCrumblesIterations /*= 3*/) :
    m_BackgroundFrame(0),
	m_ImageCompression(imageCompression),
    m_separator(backgroundHistory, differenceThreshold, newBackgroundMinPixels, minCrumbleArea, mergeCrumblesIterations)
	//, m_BackgroundHistory(backgroundHistory)	// 300
	//, m_DifferenceThreshold(differenceThreshold)	// 20
	//, m_NewBackgroundMinPixels(newBackgroundMinPixels)		// 0.2
	//, m_MinCrumbleArea(minCrumbleArea)		// 100
	//, m_MergeCrumblesIterations(mergeCrumblesIterations)	// 3
{

}

void DefaultPacker::createSequence()
{
	if (m_ImageCompression != sequence::CompressedVideo::DEFAULT_COMPRESSION)
		m_CompressedVideo = std::make_shared<sequence::CompressedVideo>(m_ImageCompression);
	else
		m_CompressedVideo = std::make_shared<sequence::CompressedVideo>();

	m_BackgroundFrame = 0;
	if (!m_separator.wasIniialized()) {
        resetBackgroundSubtractor();
    }
}

void DefaultPacker::compressFrame(size_t frameId, const cv::Mat& frame, sequence::IVideo::Timestamp timestamp)
{
	std::vector<sequence::CompressedVideo::Crumble> crumbles;
	
	try
	{
        auto updater = [&](const cv::Mat& bck) {
            if (frameId > 0)
            {
                m_CompressedVideo->addBackground(bck, m_BackgroundFrame);
                m_BackgroundFrame = frameId;
            }
        };
        auto rects = m_separator.separate(frame, updater);
        crumbles = convertToCrumbles(frame, rects);
		m_CompressedVideo->addFrame(timestamp, crumbles);
	}
	catch (...)
	{
		m_CompressedVideo->addFrame(timestamp, crumbles);
		throw;
	}	
}

DefaultPacker::Crumbles DefaultPacker::convertToCrumbles(const cv::Mat& frame, const sequence::FBSeparator::Rectangles& rects)
{
    Crumbles crumbles;
    for (auto& br : rects) {
        crumbles.push_back(sequence::CompressedVideo::Crumble(br.tl(), cv::Mat(frame, br)));
    }

    return crumbles;
}

void DefaultPacker::save(const std::string& filename)
{
	if (!m_CompressedVideo)
		return;
	m_CompressedVideo->addBackground(m_separator.getLastBackground(), 0);
	m_CompressedVideo->save(filename);
	m_CompressedVideo.reset();
	std::cout << std::endl << "Post-processed file " << filename << " saved." << std::endl;
}

void DefaultPacker::resetBackgroundSubtractor()
{
    m_separator.reset();
}