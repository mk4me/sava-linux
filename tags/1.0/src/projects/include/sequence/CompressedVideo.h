#pragma once
#ifndef CompressedVideo_h__
#define CompressedVideo_h__

#include "Image.h"
#include "IVideo.h"
#include "IStreamedVideo.h"

#include <utils/CvSerialization.h>

#include <boost/date_time/posix_time/time_serialize.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <vector>
#include <map>

namespace sequence
{ 
	/// <summary>
	/// klasa do serializacji sekwencji wideo skompresowanej za pomoc¹ obszarów ruchomych. Zawiera tak¿e zoptymalizowana funkcjê zwracaj¹c¹ tylko wybrany obszar obrazu.
	/// </summary>
	/// <seealso cref="IVideo" />
	class CompressedVideo : public IVideo
	{
	public:
		static const unsigned TYPE;
		static const size_t CURRENT_FRAME = -1;
		static const int DEFAULT_COMPRESSION;

		struct Crumble
		{
			cv::Point location;
			cv::Mat image;

			Crumble() { }
			Crumble(const cv::Point& location, const cv::Mat& image) : location(location), image(image) {}
		};

		explicit CompressedVideo(int imageCompression = DEFAULT_COMPRESSION);
		explicit CompressedVideo(const std::string& filename);
		~CompressedVideo() { }
		
		bool load(const std::string& filename);
		bool save(const std::string& filename) const override;
		bool saveAsText(const std::string& filename) const override;

		void addFrame(Timestamp time, const std::vector<Crumble>& crumbles = std::vector<Crumble>());		
		void addBackground(const cv::Mat& background, size_t startFrame = CURRENT_FRAME);

		virtual size_t getNumFrames() const { return m_Frames.size(); }
		virtual cv::Mat getFrameImage(size_t frameNum) const;
		virtual cv::Mat getFrameImage(size_t frameNum, const cv::Rect& roi) const override;
		virtual bool getNextFrame(cv::Mat& frame) override;

		virtual Timestamp getFrameTime(size_t frameNum) const;
		cv::Mat getFrameBackground(size_t frameNum) const;
		std::vector<cv::Rect> getFrameCrumbles(size_t frameNum) const;

		virtual void setPosition(size_t frame);

	private:

		class CrumbleStorage
		{
		public:
			CrumbleStorage() { }
			CrumbleStorage(const cv::Point& location, const cv::Mat& image);
			CrumbleStorage(const cv::Point& location, const cv::Mat& image, int compression);
			//CrumbleStorage(const CrumbleStorage&) = default;
			cv::Rect getLocation() const { return m_Location; }
			cv::Mat getImage() const { return m_Image; }
		private:
			Image m_Image;
			cv::Rect m_Location;

			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		class Frame
		{
		public:
			Frame() { }
			Frame(Frame&& f) : m_Time(f.m_Time), m_Crumbles(std::move(f.m_Crumbles)) { }
			//Frame(Frame&) = default;
			Frame(const Frame&) = default;
			Frame(Timestamp time, const std::vector<CrumbleStorage>& crumbles) : m_Time(time), m_Crumbles(crumbles) { }

			Timestamp getTime() const { return m_Time; }
			const std::vector<CrumbleStorage>& getCrumbles() const { return m_Crumbles; }
		private:
			Timestamp m_Time;
			std::vector<CrumbleStorage> m_Crumbles;

			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		std::vector<Frame> m_Frames;
		std::map<size_t, Image> m_Backgrounds;
		mutable size_t m_LastBackgroundId;
		mutable cv::Mat m_LastBackgroundImage;
		size_t m_CurrentFrame;

		int m_ImageCompression;

		void assemblyCrumbles(size_t frameNum, const cv::Rect roi, cv::Mat& image) const;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void CompressedVideo::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Frames;
		ar & m_Backgrounds;
	}

	template<class Archive>
	void CompressedVideo::CrumbleStorage::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Image;
		ar & m_Location;
	}

	template<class Archive>
	void CompressedVideo::Frame::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Time;
		ar & m_Crumbles;
	}

}

#endif // CompressedVideo_h__