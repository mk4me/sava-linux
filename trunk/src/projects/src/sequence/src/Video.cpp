#include "Video.h"
#include "VideoUtils.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <cassert>

namespace sequence
{
	const unsigned Video::TYPE = IVideo::SEQUENCE_VIDEO;

	Video::Video() : m_CurrentFrame(0)
	{
	}

	Video::Video(const std::string& filename)
	{
		load(filename);
	}

	bool Video::load(const std::string& filename)
	{
		m_CurrentFrame = 0;
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			unsigned type;
			ia >> type;
			assert(type == TYPE && "Invalid file type");
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::Video::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Video::save(const std::string& filename) const
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
			std::cerr << "sequence::Video::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	void Video::addFrame(Timestamp time, const std::vector<uchar>& data)
	{
		m_Frames.push_back(std::move(Frame(time, data)));
	}

	void Video::addFrame(Timestamp time, const cv::Mat& image)
	{
		m_Frames.push_back(std::move(Frame(time, image)));
	}

	cv::Mat Video::getFrameImage(size_t frameNum) const
	{
		if (m_Frames.empty())
			return cv::Mat();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		return m_Frames[frameNum].getImage();
	}

	cv::Mat Video::getFrameImage(size_t frameNum, const cv::Rect& roi) const
	{
		cv::Mat frame = getFrameImage(frameNum);
		return VideoUtils::getImageRoi(frame, roi);
	}

	Video::Timestamp Video::getFrameTime(size_t frameNum) const
	{
		if (m_Frames.empty())
			return Timestamp();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		return m_Frames[frameNum].getTimestamp();
	}

	bool Video::getNextFrame(cv::Mat& frame)
	{
		if (m_CurrentFrame >= m_Frames.size())
			return false;

		frame = getFrameImage(m_CurrentFrame++);
		return true;
	}

	void Video::setPosition(size_t frame)
	{
		m_CurrentFrame = frame;
	}
}