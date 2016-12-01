#pragma once
#ifndef Video_h__
#define Video_h__

#include "Image.h"
#include "IVideo.h"
#include "IStreamedVideo.h"

#include <boost/date_time/posix_time/time_serialize.hpp>
#include <boost/serialization/vector.hpp>

#include <string>
#include <vector>


namespace sequence
{
	class Video : public IVideo
	{
	public:
		static const unsigned TYPE;

		Video();
		explicit Video(const std::string& filename);
		~Video() { }

		bool load(const std::string& filename);
		bool save(const std::string& filename) const override;
		bool saveAsText(const std::string& filename) const override;

		void addFrame(Timestamp time, const std::vector<uchar>& data);
		void addFrame(Timestamp time, const cv::Mat& image);

		virtual size_t getNumFrames() const { return m_Frames.size(); }
		virtual cv::Mat getFrameImage(size_t frameNum) const;
		virtual cv::Mat getFrameImage(size_t frameNum, const cv::Rect& roi) const;
		virtual bool getNextFrame(cv::Mat& frame);

		virtual Timestamp getFrameTime(size_t frameNum) const;

		virtual void setPosition(size_t frame);

	private:
		class Frame
		{
		public:
			Frame() { }

			Frame(Timestamp time, const std::vector<uchar>& data) : m_Time(time), m_Image(data) { }
			Frame(Timestamp time, const cv::Mat& image) : m_Time(time), m_Image(image) { }

			Frame(Frame&& f) : m_Time(f.m_Time), m_Image(std::move(f.m_Image)) { }
			Frame(Frame&) = default;
			Frame(const Frame&) = default;
			Frame& operator=(Frame&& f)
			{
				m_Image = std::move(f.m_Image);
				return *this;
			}

			Timestamp getTimestamp() const { return m_Time; }
			cv::Mat getImage() const { return m_Image; }
		private:
			Timestamp m_Time;
			Image m_Image;

			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		std::vector<Frame> m_Frames;
		size_t m_CurrentFrame;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void Video::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Frames;
	}

	template<class Archive>
	void Video::Frame::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Time;
		ar & m_Image;
	}
}

#endif // Video_h__