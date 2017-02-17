#pragma once

#include <vector>
#include <memory>
#include "sequence/IVideo.h"

class ReplayVideo
{

public:
	ReplayVideo();
	~ReplayVideo();

	void reload(const std::vector<std::string>& _videoFiles);
	void clear();

	std::vector<sequence::IVideo::Timestamp>& getFramesTimes();
	cv::Mat getFrameImage(size_t _frameNr) const;
	size_t getFramesCount() const;

private:
	std::vector<std::shared_ptr<sequence::IVideo>> m_Videos;

	std::vector<size_t> m_FramesSizes;
	std::vector<sequence::IVideo::Timestamp> m_FramesTimes;
};

typedef std::shared_ptr<ReplayVideo> ReplayVideoPtr;

