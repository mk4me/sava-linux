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
    m_separator.reset();
}

void GpuPacker::compressFrame(size_t frameId, const cv::Mat& frame, sequence::IVideo::Timestamp timestamp)
{
	cv::Mat mask, background, backgroundGray, frameGray, difference;

	std::vector<cv::Rect> crumbles;

	try
	{
		/*m_BackgroundSubtractor->apply(frame, mask);
		m_BackgroundSubtractor->getBackgroundImage(background);

		cv::cvtColor(background, backgroundGray, CV_RGB2GRAY);
		cv::cvtColor(frame, frameGray, CV_RGB2GRAY);

		cv::absdiff(frameGray, backgroundGray, difference);
		cv::threshold(difference, difference, m_DifferenceThreshold, 255, cv::THRESH_BINARY);

		if (!m_CameraMask.empty())
			cv::min(difference, m_CameraMask, difference);

		if (!m_LastBackground.empty() && cv::countNonZero(difference) > mask.rows * mask.cols * m_NewBackgroundMinPixels)
		{
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
					crumbles.push_back(br);
				}
			}
		} */
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
