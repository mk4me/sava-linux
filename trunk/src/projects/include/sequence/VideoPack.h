#pragma once
#ifndef VideoPack_h__
#define VideoPack_h__

#include "IStreamedVideo.h"

#include <boost/serialization/access.hpp>

namespace sequence
{
	class VideoPack : public IStreamedVideo
	{
	public:
		explicit VideoPack(const std::vector<std::string>& videoFiles);
		explicit VideoPack(const std::string& filename);
		~VideoPack() { }

		virtual size_t getNumFrames() const override;
		virtual bool getNextFrame(cv::Mat& frame) override;

		virtual void setPosition(size_t frame) override;

		bool load(const std::string& filename);
		bool save(const std::string& filename) const;

		void setCenterVideo(int index) { m_CenterVideoIndex = index; }

		const std::vector<std::string>& getVideoNames() const { return m_FileNames; }
		const std::string& getCenterVideoName() const;

	private:
		struct Video
		{
			Video(const std::shared_ptr<IStreamedVideo>& video, size_t endPos) : video(video), endPos(endPos) { }

			std::shared_ptr<IStreamedVideo> video;
			size_t endPos;
		};
		
		static const std::string c_EmptyString;

		std::vector<std::string> m_FileNames;
		std::vector<Video> m_Files;
		size_t m_Length;
		int m_CenterVideoIndex;

		size_t m_CurrentFile;

		void loadVideos();

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // VideoPack_h__
