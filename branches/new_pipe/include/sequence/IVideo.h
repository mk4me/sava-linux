#pragma once
#ifndef IVideo_h__
#define IVideo_h__

#include "IStreamedVideo.h"

#include <opencv/cv.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <string>

namespace sequence
{
	/// <summary>
	/// interfejs odczytu video wraz ze znacznikami czasowymi oraz obszarami ruchu.
	/// </summary>
	/// <seealso cref="IStreamedVideo" />
	class IVideo : public IStreamedVideo
	{
	public:
		static const unsigned SEQUENCE_VIDEO;
		static const unsigned SEQUENCE_COMPRESSED_VIDEO;
		static const unsigned SEQUENCE_GPU_VIDEO;

		typedef boost::posix_time::ptime Timestamp;

		static std::shared_ptr<IVideo> create(const std::string& filename);
		static std::shared_ptr<IVideo> createFromText(const std::string& filename);

		virtual ~IVideo() { }

		virtual bool save(const std::string& filename) const = 0;
		virtual bool saveAsText(const std::string& filename) const = 0;

		virtual size_t getNumFrames() const = 0;
		virtual cv::Mat getFrameImage(size_t frameNum) const = 0;
		virtual cv::Mat getFrameImage(size_t frameNum, const cv::Rect& roi) const = 0;
		virtual Timestamp getFrameTime(size_t frameNum) const = 0;
		virtual std::vector<cv::Rect> getFrameCrumbles(size_t frameNum) const { return std::vector<cv::Rect>(); }
	};
}

#endif // IVideo_h__