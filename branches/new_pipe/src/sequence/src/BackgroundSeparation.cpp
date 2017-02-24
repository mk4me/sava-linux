#include <sequence/BackgroundSeparation.h>
#include "CompressedVideo.h"
#include "VideoUtils.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "boost/archive/text_oarchive.hpp"

#include <fstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>

#include <opencv2/cudabgsegm.hpp>

namespace sequence {

	FBSeparator::FBSeparator(int history, int diffTreshold, float newBgMinPixels,
                             int minCrumbleArea, int mergeCrumblesIterations, const cv::Mat mask) :
		m_BackgroundHistory(history),
        m_DifferenceThreshold(diffTreshold),
        m_NewBackgroundMinPixels(newBgMinPixels),
        m_MinCrumbleArea(minCrumbleArea),
        m_MergeCrumblesIterations(mergeCrumblesIterations),
        m_CameraMask(mask)
	{

	}
	FBSeparator::~FBSeparator()
	{

	}

    void FBSeparator::reset()
    {
        m_BackgroundSubtractor = cv::createBackgroundSubtractorMOG2(m_BackgroundHistory);
    }

    bool FBSeparator::wasIniialized() const
    {
        return m_BackgroundSubtractor ? true : false;
    }

    FBSeparator::Rectangles FBSeparator::separate(const cv::Mat& frame,  std::function<void (const cv::Mat&)> bgUpdate)
    {
		cv::Mat mask, background, backgroundGray, frameGray, difference;

        Rectangles crumbles;

		m_BackgroundSubtractor->apply(frame, mask);
        m_BackgroundSubtractor->getBackgroundImage(background);

        cv::cvtColor(background, backgroundGray, CV_RGB2GRAY);
        cv::cvtColor(frame, frameGray, CV_RGB2GRAY);

        if (!m_CameraMask.empty())
            cv::min(difference, m_CameraMask, difference);

        cv::absdiff(frameGray, backgroundGray, difference);
        cv::threshold(difference, difference, m_DifferenceThreshold, 255, cv::THRESH_BINARY);

        if (!m_LastBackground.empty() && cv::countNonZero(difference) > mask.rows * mask.cols * m_NewBackgroundMinPixels)
        {
            /*if (frameId > 0)
            {
                m_CompressedVideo->addBackground(m_LastBackground, m_BackgroundFrame);
                m_BackgroundFrame = frameId;
            }*/
            bgUpdate(m_LastBackground);
            reset();
            m_BackgroundSubtractor->apply(frame, mask);
            m_BackgroundSubtractor->getBackgroundImage(m_LastBackground);
        }
        else
        {
            m_LastBackground = background;

            auto contours = findContours(difference);
            fillRects(frame, contours, crumbles);
        }

        return crumbles;
	}


    void FBSeparator::fillRects(const cv::Mat& frame, const Contours& contours, Rectangles& crumbles) {
        std::vector<cv::Rect> boundingRects;
        for (auto& c : contours)
        {
            cv::Rect br = cv::boundingRect(c);
            boundingRects.push_back(br);
        }

        for (auto& br : boundingRects)
        {
            if (br.area() > m_MinCrumbleArea)
            {
                crumbles.push_back(br);
            }
        }
    }


    FBSeparator::Contours FBSeparator::findContours(const cv::Mat& difference)
    {
        cv::Mat kernelOpen = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::Mat kernelClose = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
        cv::morphologyEx(difference, difference, cv::MORPH_ERODE, kernelOpen, cv::Point(-1, -1), 1);
        cv::morphologyEx(difference, difference, cv::MORPH_DILATE, kernelClose, cv::Point(-1, -1), 1);

        Contours contours;
        cv::findContours(difference, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);


        for (int i = 0; i < m_MergeCrumblesIterations; ++i)
        {
            cv::Mat boundingFrame = cv::Mat::zeros(difference.rows, difference.cols, difference.type());
            for (auto& c : contours)
            {
                cv::Rect br = cv::boundingRect(c);
                cv::rectangle(boundingFrame, br, 255, -1);
            }
            size_t contoursCount = contours.size();
            contours.clear();
            cv::findContours(boundingFrame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
            if (contours.size() >= contoursCount)
            {
                break;
            }
        }

        return contours;
    }
}

