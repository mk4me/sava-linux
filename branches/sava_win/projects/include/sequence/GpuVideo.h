#pragma once
#ifndef GpuVideo_h__
#define GpuVideo_h__

#include "IVideo.h"
#include "IStreamedVideo.h"

#include <utils/CvSerialization.h>

#include <boost/date_time/posix_time/time_serialize.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <vector>
#include <map>

namespace utils
{ 
	class GpuJpegDecoder;
	class GpuJpegEncoder;
}

namespace sequence
{
	/// <summary>
	/// klasa do serializacji sekwencji wideo skompresowanej przy u¿yciu biblioteki LibGpuJpeg.
	/// </summary>
	/// <seealso cref="IVideo" />
	class GpuVideo : public IVideo
	{
	public:
		static const unsigned TYPE;
		static const int DEFAULT_COMPRESSION;

		explicit GpuVideo(int imageCompression = DEFAULT_COMPRESSION);
		explicit GpuVideo(const std::string& filename);
		~GpuVideo() { }

		bool load(const std::string& filename);
		bool save(const std::string& filename) const override;
		bool saveAsText(const std::string& filename) const override;

		void addFrame(Timestamp time, const cv::Mat& image, const std::vector<cv::Rect>& crumbles = std::vector<cv::Rect>());

		virtual size_t getNumFrames() const override { return m_Frames.size(); }
		virtual cv::Mat getFrameImage(size_t frameNum) const override;
		virtual cv::Mat getFrameImage(size_t frameNum, const cv::Rect& roi) const override;
		virtual bool getNextFrame(cv::Mat& frame) override;

		virtual Timestamp getFrameTime(size_t frameNum) const override;
		std::vector<cv::Rect> getFrameCrumbles(size_t frameNum) const;

		virtual void setPosition(size_t frame) override;

	private:
		class Frame
		{
		public:
			Frame() { }
			//Frame(Frame&) = default;
			Frame(const Frame&) = default;
			Frame(Frame&& f) : m_Time(f.m_Time), m_ImageData(std::move(f.m_ImageData)), m_Crumbles(std::move(f.m_Crumbles)) { }
			Frame(Timestamp time, const std::vector<unsigned char>& imageData, const std::vector<cv::Rect>& crumbles) : m_Time(time), m_ImageData(imageData), m_Crumbles(crumbles) { }

			Timestamp getTime() const { return m_Time; }
			const std::vector<unsigned char>& getImageData() const { return m_ImageData; }
			const std::vector<cv::Rect>& getCrumbles() const { return m_Crumbles; }
		private:
			Timestamp m_Time;
			std::vector<unsigned char> m_ImageData;
			std::vector<cv::Rect> m_Crumbles;

			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		std::vector<Frame> m_Frames;
		size_t m_CurrentFrame;
		int m_ImageCompression;

		static std::shared_ptr<utils::GpuJpegDecoder> m_Decoder;
		static std::shared_ptr<utils::GpuJpegEncoder> m_Encoder;

		void encode(const cv::Mat& input, std::vector<unsigned char>& output) const;
		void decode(const std::vector<unsigned char>& input, cv::Mat& output) const;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void GpuVideo::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Frames;
		ar & m_ImageCompression;
	}

	template<class Archive>
	void sequence::GpuVideo::Frame::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Time;
		ar & m_ImageData;
		ar & m_Crumbles;
	}
}

#endif // GpuVideo_h__