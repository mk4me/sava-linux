#pragma once
#ifndef StreamedVideo_h__
#define StreamedVideo_h__

#include "IStreamedVideo.h"

#include "opencv2/videoio.hpp"

namespace sequence
{
	class StreamedVideo : public IStreamedVideo
	{
	public:
		explicit StreamedVideo(const std::string& filename);
		~StreamedVideo() { }
	
		virtual size_t getNumFrames() const;
		virtual bool getNextFrame(cv::Mat& frame);

		virtual void setPosition(size_t frame);

	private:
		cv::VideoCapture m_Capture;
	};
}

#endif // StreamedVideo_h__
