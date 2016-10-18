#pragma once
#ifndef IPacker_h__
#define IPacker_h__

#include <sequence/IVideo.h>

#include <opencv2/core.hpp>

class IPacker
{
public:
	virtual void createSequence() = 0;
	virtual void compressFrame(size_t frameId, const cv::Mat& frame, sequence::IVideo::Timestamp timestamp) = 0;
	virtual void save(const std::string& filename) = 0;
};

#endif // IPacker_h__