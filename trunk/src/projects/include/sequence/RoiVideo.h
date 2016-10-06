#pragma once
#ifndef RoiVideo_h__
#define RoiVideo_h__

#include "IStreamedVideo.h"
#include "IVideo.h"
#include "Cluster.h"

namespace sequence
{	
	class RoiVideo : public IStreamedVideo
	{
	public:
		RoiVideo(const std::string& filename);
		~RoiVideo();

		virtual size_t getNumFrames() const;
		virtual bool getNextFrame(cv::Mat& frame);

		virtual void setPosition(size_t frame);

	private:
		std::shared_ptr<Cluster> m_Cluster;
		std::shared_ptr<IVideo> m_Video;
		size_t m_CurrentFrame;
	};
}

#endif // RoiVideo_h__
