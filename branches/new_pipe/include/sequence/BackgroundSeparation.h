#pragma once
#ifndef BackgroundSeperation_h__
#define BackgroundSeperation_h__

#include <opencv2/core.hpp>
#include <sequence/CompressedVideo.h>

namespace cv
{
	class BackgroundSubtractor;
}

namespace sequence
{

	class FBSeparator
	{
    public:
        typedef std::vector<cv::Rect> Rectangles;
        typedef std::vector<std::vector<cv::Point>> Contours;
	public:
        FBSeparator(int history = 300, int diffTreshold = 20, float newBgMinPixels = 0.2f,
                    int minCrumbleArea = 100, int mergeCrumblesIterations = 3, const cv::Mat mask = cv::Mat());
		virtual ~FBSeparator();

	public:
        Rectangles separate(const cv::Mat& frame, std::function<void (const cv::Mat&)> bgUpdate = [](const cv::Mat&){});
		void reset();
        const cv::Mat& getLastBackground() { return m_LastBackground; }

    private:
        void fillRects(const cv::Mat& frame, const std::vector<std::vector<cv::Point>>& contours, Rectangles& crumbles);
        Contours findContours(const cv::Mat& difference);

    private:
		cv::Ptr<cv::BackgroundSubtractor> m_BackgroundSubtractor;
        int m_BackgroundHistory;	// 300
        int m_DifferenceThreshold;	// 20
        float m_NewBackgroundMinPixels;		// 0.2
        int m_MinCrumbleArea;		// 100
        int m_MergeCrumblesIterations;	// 3
        cv::Mat m_LastBackground;
        cv::Mat m_CameraMask;
	};

};



#endif // CompressedVideo_h__