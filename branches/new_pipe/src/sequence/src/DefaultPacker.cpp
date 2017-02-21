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
	//if (!m_seperator) {
        resetBackgroundSubtractor();
    //}
}

void DefaultPacker::compressFrame(size_t frameId, const cv::Mat& frame, sequence::IVideo::Timestamp timestamp)
{
	//cv::Mat mask, background, backgroundGray, frameGray, difference;

	std::vector<sequence::CompressedVideo::Crumble> crumbles;
	
	try
	{
		/*m_BackgroundSubtractor->apply(frame, mask);
		m_BackgroundSubtractor->getBackgroundImage(background);

		cv::cvtColor(background, backgroundGray, CV_RGB2GRAY);
		cv::cvtColor(frame, frameGray, CV_RGB2GRAY);

		cv::absdiff(frameGray, backgroundGray, difference);
		cv::threshold(difference, difference, m_DifferenceThreshold, 255, cv::THRESH_BINARY);
		
		if (!m_LastBackground.empty() && cv::countNonZero(difference) > mask.rows * mask.cols * m_NewBackgroundMinPixels)
		{
			if (frameId > 0)
			{
				m_CompressedVideo->addBackground(m_LastBackground, m_BackgroundFrame);
				m_BackgroundFrame = frameId;
			}
			resetBackgroundSubtractor();
			m_BackgroundSubtractor->apply(frame, mask);
			m_BackgroundSubtractor->getBackgroundImage(m_LastBackground);
		}
		else
		{
			m_LastBackground = background;

			cv::Mat kernelOpen = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			cv::Mat kernelClose = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
			cv::morphologyEx(difference, difference, cv::MORPH_ERODE, kernelOpen, cv::Point(-1, -1), 1);
			cv::morphologyEx(difference, difference, cv::MORPH_DILATE, kernelClose, cv::Point(-1, -1), 1);

			std::vector<std::vector<cv::Point>> contours;
			cv::findContours(difference, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

			//std::vector<cv::Rect> originalContours;

			for (int i = 0; i < m_MergeCrumblesIterations; ++i)
			{
				cv::Mat boundingFrame = cv::Mat::zeros(difference.rows, difference.cols, difference.type());
				for (auto& c : contours)
				{
					cv::Rect br = cv::boundingRect(c);
					cv::rectangle(boundingFrame, br, 255, -1);
					//if (i == 0)
					//{
					//	originalContours.push_back(br);
					//}
				}
				size_t contoursCount = contours.size();
				contours.clear();
				cv::findContours(boundingFrame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
				if (contours.size() >= contoursCount)
				{
					break;
				}
			}

			std::vector<cv::Rect> boundingRects;
			for (auto& c : contours)
			{
				cv::Rect br = cv::boundingRect(c);
				boundingRects.push_back(br);
			}

			//int divideIterations = 4;
			//divideBoundingRects(originalContours, boundingRects, divideIterations);

			for (auto& br : boundingRects)
			{
				if (br.area() > m_MinCrumbleArea)
				{
					crumbles.push_back(sequence::CompressedVideo::Crumble(br.tl(), cv::Mat(frame, br)));
				}
			}
		}*/

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
    //if (!m_seperator) {
    //    m_seperator = std::make_unique<sequence::FBSeparator>();
    //}
    m_separator.reset();
	//m_BackgroundSubtractor = cv::cuda::createBackgroundSubtractorMOG2(m_BackgroundHistory);
}