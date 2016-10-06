#pragma once
#ifndef IVideo_h__
#define IVideo_h__

#include <opencv/cv.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>

namespace sequence
{
	class IVideo
	{
	public:
		static const unsigned SEQUENCE_VIDEO;
		static const unsigned SEQUENCE_COMPRESSED_VIDEO;

		typedef boost::posix_time::ptime Timestamp;

		static std::shared_ptr<IVideo> create(const std::string& filename);

		virtual ~IVideo() { }

		virtual size_t getNumFrames() const = 0;
		virtual cv::Mat getFrameImage(size_t frameNum) const = 0;
		virtual cv::Mat getFrameImage(size_t frameNum, const cv::Rect& roi) const = 0;
		virtual Timestamp getFrameTime(size_t frameNum) const = 0;
	};
}

#endif // IVideo_h__