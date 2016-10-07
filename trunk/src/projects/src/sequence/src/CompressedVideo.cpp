#include "CompressedVideo.h"
#include "VideoUtils.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <opencv2/imgproc.hpp>

namespace sequence
{
	const unsigned CompressedVideo::TYPE = IVideo::SEQUENCE_COMPRESSED_VIDEO;

	const int CompressedVideo::DEFAULT_COMPRESSION = Image::DEFAULT_COMPRESSION;

	CompressedVideo::CompressedVideo(const std::string& filename)
		: m_LastBackgroundId(CURRENT_FRAME)
	{
		load(filename);
	}
	
	CompressedVideo::CompressedVideo(int imageCompression /*= DEFAULT_COMPRESSION*/) 
		: m_LastBackgroundId(CURRENT_FRAME)
		, m_ImageCompression(imageCompression)
		, m_CurrentFrame(0)
	{
	}

	bool CompressedVideo::load(const std::string& filename)
	{
		m_CurrentFrame = 0;
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			int type;
			ia >> type;
			assert(type == TYPE && "Invalid file type");
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::CompressedVideo::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool CompressedVideo::save(const std::string& filename) const
	{
		try
		{
			std::ofstream ofs(filename, std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << TYPE;
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::CompressedVideo::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	void CompressedVideo::addFrame(Timestamp time, const std::vector<Crumble>& crumbles /*= std::vector<Crumble>()*/)
	{
		std::vector<CrumbleStorage> crumbleStorage;
		std::transform(crumbles.begin(), crumbles.end(), std::back_inserter(crumbleStorage), [this](const Crumble& c){ return CrumbleStorage(c.location, c.image, m_ImageCompression); });
		m_Frames.push_back(std::move(Frame(time, crumbleStorage)));
	}

	void CompressedVideo::addBackground(const cv::Mat& background, size_t startFrame /*= CURRENT_FRAME*/)
	{
		m_Backgrounds[CURRENT_FRAME == startFrame ? m_Frames.size() : startFrame] = std::move(Image(background, m_ImageCompression));
	}

	cv::Mat CompressedVideo::getFrameImage(size_t frameNum) const
	{
		if (m_Frames.empty())
			return cv::Mat();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		cv::Mat image = getFrameBackground(frameNum).clone();
		const cv::Rect roi(0, 0, image.cols, image.rows);	

		assemblyCrumbles(frameNum, roi, image);		
		return image;
	}

	cv::Mat CompressedVideo::getFrameImage(size_t frameNum, const cv::Rect& roi) const
	{
		if (m_Frames.empty())
			return cv::Mat();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		cv::Mat image = getFrameBackground(frameNum).clone();

		assemblyCrumbles(frameNum, roi, image);
		return VideoUtils::getImageRoi(image, roi);
	}

	void CompressedVideo::assemblyCrumbles(size_t frameNum, const cv::Rect roi, cv::Mat& image) const
	{
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		const std::vector<CrumbleStorage>& crumbles = m_Frames[frameNum].getCrumbles();

		for (auto& crumble: crumbles)
		{
			cv::Rect destRoi = crumble.getLocation() & roi;
			if (destRoi.area() == 0)
				continue;
			cv::Mat destImage = image(destRoi);
			cv::Rect crumbleRoi(destRoi.tl() - crumble.getLocation().tl(), destRoi.size());
			cv::Mat crumbleImage(crumble.getImage(), crumbleRoi);
			crumbleImage.copyTo(destImage);
#ifdef _DEBUG
			//cv::rectangle(image, roi, CV_RGB(0, 255, 0), 2);
#endif
		}
	}

	bool CompressedVideo::getNextFrame(cv::Mat& frame)
	{
		if (m_CurrentFrame >= m_Frames.size())
			return false;

		frame = getFrameImage(m_CurrentFrame++);
		return true;
	}

	CompressedVideo::Timestamp CompressedVideo::getFrameTime(size_t frameNum) const
	{
		if (m_Frames.empty())
			return Timestamp();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		return m_Frames[frameNum].getTime();
	}

	cv::Mat CompressedVideo::getFrameBackground(size_t frameNum) const
	{
		if (m_Backgrounds.empty())
			return cv::Mat();
		if (frameNum < m_Backgrounds.begin()->first)
		{
			return cv::Mat::zeros(0, 0, CV_8UC3);
		}
		auto it = std::prev(m_Backgrounds.upper_bound(frameNum));
		if (CURRENT_FRAME == m_LastBackgroundId || it->first != m_LastBackgroundId)
		{
			m_LastBackgroundId = it->first;
			m_LastBackgroundImage = it->second;
		}
		return m_LastBackgroundImage;
	}

	std::vector<cv::Rect> CompressedVideo::getFrameCrumbles(size_t frameNum) const
	{
		if (m_Frames.empty())
			return std::vector<cv::Rect>();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		auto & crumbles = m_Frames[frameNum].getCrumbles();
		std::vector<cv::Rect> retCrumbles;
		std::transform(crumbles.begin(), crumbles.end(), std::back_inserter(retCrumbles), [](const CrumbleStorage& crumble){ return crumble.getLocation(); });
		return retCrumbles;
	}

	CompressedVideo::CrumbleStorage::CrumbleStorage(const cv::Point& location, const cv::Mat& image)
		: m_Image(image)
	{
		m_Location.x = location.x;
		m_Location.y = location.y;
		m_Location.width = image.cols;
		m_Location.height = image.rows;
	}

	CompressedVideo::CrumbleStorage::CrumbleStorage(const cv::Point& location, const cv::Mat& image, int compression)
		: m_Image(image, compression)
	{
		m_Location.x = location.x;
		m_Location.y = location.y;
		m_Location.width = image.cols;
		m_Location.height = image.rows;
	}

	void CompressedVideo::setPosition(size_t frame)
	{
		m_CurrentFrame = frame;
	}
}
