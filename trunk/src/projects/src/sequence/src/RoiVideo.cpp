#include "RoiVideo.h"

#include <boost/filesystem.hpp>

namespace sequence
{	
	RoiVideo::RoiVideo(const std::string& filename) 
	{
		m_Cluster = std::make_shared<Cluster>(filename);
		m_CurrentFrame = m_Cluster->getStartFrame();

		// finds pattern "pattern.xx.ext"
		std::string filePattern = filename.substr(0, filename.find_last_of('.', filename.find_last_of('.') - 1));
		std::string fName = filePattern + ".cvs";
		if (!boost::filesystem::exists(fName))
		{
			fName = filePattern + ".vsq";
			if (!boost::filesystem::exists(fName))
				return;
		}
		
		m_Video = IVideo::create(fName);
	}

	RoiVideo::~RoiVideo()
	{
	
	}
	
	bool RoiVideo::getNextFrame(cv::Mat& frame)
	{
		if (!m_Video || !m_Cluster || m_CurrentFrame == Cluster::NPOS)
			return false;

		if (m_CurrentFrame > m_Cluster->getEndFrame())
			return false;

		frame = m_Video->getFrameImage(m_CurrentFrame, m_Cluster->getFrameAt(m_CurrentFrame));
		++m_CurrentFrame;
		return !frame.empty();
	}

	void RoiVideo::setPosition(size_t frame)
	{
		m_CurrentFrame = frame;
	}

	size_t RoiVideo::getNumFrames() const
	{
		if (!m_Cluster || m_Cluster->getStartFrame() == Cluster::NPOS)
			return 0;

		return m_Cluster->getEndFrame() - m_Cluster->getStartFrame() + 1;
	}

}
