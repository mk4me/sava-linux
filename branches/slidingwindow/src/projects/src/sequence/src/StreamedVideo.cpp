#include "StreamedVideo.h"

namespace sequence
{
	StreamedVideo::StreamedVideo(const std::string& filename)
	{
		m_Capture.open(filename);
	}

	bool StreamedVideo::getNextFrame(cv::Mat& frame)
	{
		if (!m_Capture.isOpened())
			return false;

		cv::Mat f;
		if (!m_Capture.read(f))
			return false;

		frame = f;
		return true;
	}

	void StreamedVideo::setPosition(size_t frame)
	{
		if (m_Capture.isOpened())
			m_Capture.set(cv::CAP_PROP_POS_FRAMES, (double)frame);
	}

	size_t StreamedVideo::getNumFrames() const
	{
		if (!m_Capture.isOpened())
			return 0;

		return static_cast<int>(m_Capture.get(cv::CAP_PROP_FRAME_COUNT));
	}

}

