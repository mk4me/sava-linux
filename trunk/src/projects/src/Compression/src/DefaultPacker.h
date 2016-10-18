#pragma once
#ifndef DefaultPacker_h__
#define DefaultPacker_h__

#include "IPacker.h"

namespace cv
{
	class BackgroundSubtractor;
}

namespace sequence
{
	class IVideo;
	class CompressedVideo;
}

class DefaultPacker : public IPacker
{
public:
	DefaultPacker(int imageCompression, int backgroundHistory = 300, int differenceThreshold = 20, float newBackgroundMinPixels = 0.2, int minCrumbleArea = 100, int mergeCrumblesIterations = 3);

	virtual void createSequence() override;
	virtual void compressFrame(size_t frameId, const cv::Mat& frame, sequence::IVideo::Timestamp timestamp) override;
	virtual void save(const std::string& filename) override;

private:
	std::shared_ptr<sequence::CompressedVideo> m_CompressedVideo;
	cv::Ptr<cv::BackgroundSubtractor> m_BackgroundSubtractor;
	cv::Mat m_LastBackground;
	size_t m_BackgroundFrame;

	int m_ImageCompression;
	int m_BackgroundHistory;	// 300
	int m_DifferenceThreshold;	// 20
	float m_NewBackgroundMinPixels;		// 0.2
	int m_MinCrumbleArea;		// 100
	int m_MergeCrumblesIterations;	// 3

	void resetBackgroundSubtractor();
};

#endif // DefaultPacker_h__