#include "ReplayVideo.h"
#include "MonitorAlertManager.h"


ReplayVideo::ReplayVideo()
{
}


ReplayVideo::~ReplayVideo()
{
}

void ReplayVideo::reload(const std::vector<std::string>& _videoFiles)
{
	assert(!_videoFiles.empty());

	clear();

	m_FramesSizes.resize(_videoFiles.size(), 0);
	
	//iterates through all video files
	for (int i = 0; i < _videoFiles.size(); i++)
	{
		//load vides
		std::string fullPath = MonitorAlertManager::getInstance().getAlertsDirPath() + "/" + _videoFiles.at(i);
		auto video = sequence::IVideo::create(fullPath);

		//add to videos
		m_Videos.push_back(video);

		//create video frame sizes 
		if (i == 0)
			m_FramesSizes[i] = video->getNumFrames();
		else
			m_FramesSizes[i] = m_FramesSizes[i - 1] + video->getNumFrames();

		//create video times vector
		for (size_t i = 0; i < video->getNumFrames(); i++)
			m_FramesTimes.push_back(video->getFrameTime(i));
	}
}

void ReplayVideo::clear()
{
	for (auto& video : m_Videos)
		video.reset();

	m_Videos.clear();
	m_FramesSizes.clear();
	m_FramesTimes.clear();
}

std::vector<sequence::IVideo::Timestamp>& ReplayVideo::getFramesTimes()
{
	return m_FramesTimes;
}

cv::Mat ReplayVideo::getFrameImage(size_t _frameNr) const
{
	if (_frameNr < getFramesCount())
	{
		auto it = std::lower_bound(m_FramesSizes.begin(), m_FramesSizes.end(), _frameNr + 1);
		size_t videoNr = it - m_FramesSizes.begin();
		size_t realFrameNr;

		if (videoNr == 0)
			realFrameNr = _frameNr;
		else
			realFrameNr = _frameNr - m_FramesSizes[videoNr - 1];
		
		return m_Videos.at(videoNr)->getFrameImage(realFrameNr);

	}

	return cv::Mat();
}

size_t ReplayVideo::getFramesCount() const
{
	if (m_FramesSizes.empty())
		return 0;

	return m_FramesSizes.back();
}
