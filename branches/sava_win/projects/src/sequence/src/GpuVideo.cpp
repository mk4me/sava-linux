#include "GpuVideo.h"
#include "VideoUtils.h"

#include <utils/GpuJpegUtils.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include "boost/archive/text_oarchive.hpp"

namespace sequence
{
	const unsigned GpuVideo::TYPE = IVideo::SEQUENCE_GPU_VIDEO;
	const int GpuVideo::DEFAULT_COMPRESSION = 75;

	std::shared_ptr<utils::GpuJpegDecoder> GpuVideo::m_Decoder;
	std::shared_ptr<utils::GpuJpegEncoder> GpuVideo::m_Encoder;

	GpuVideo::GpuVideo(int imageCompression /*= DEFAULT_COMPRESSION*/)
		: m_ImageCompression(imageCompression)
		, m_CurrentFrame(0)
	{

	}

	GpuVideo::GpuVideo(const std::string& filename)
	{
		load(filename);
	}

	bool GpuVideo::load(const std::string& filename)
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
			std::cerr << "sequence::GpuVideo::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	template <class Archive>
	bool _saveGpuVideo(const GpuVideo* self, const std::string& filename)
	{
		try
		{
			std::ofstream ofs(filename, std::ios::binary | std::ios::out);
			Archive oa(ofs);
			oa << GpuVideo::TYPE;
			oa << *self;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::GpuVideo::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool GpuVideo::saveAsText(const std::string& filename) const
	{
		return _saveGpuVideo<boost::archive::text_oarchive>(this, filename);
	}

	bool GpuVideo::save(const std::string& filename) const
	{
		return _saveGpuVideo<boost::archive::binary_oarchive>(this, filename);
	}

	void GpuVideo::addFrame(Timestamp time, const cv::Mat& image, const std::vector<cv::Rect>& crumbles /*= std::vector<cv::Rect>()*/)
	{
		try
		{
			std::vector<unsigned char> imageData;
			encode(image, imageData);
			m_Frames.push_back(std::move(Frame(time, imageData, crumbles)));
		}
		catch (const std::exception& e)
		{
			std::cerr << "Can't add frame. " << e.what() << std::endl;
		}
	}

	cv::Mat GpuVideo::getFrameImage(size_t frameNum) const
	{
		if (m_Frames.empty())
			return cv::Mat();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		try
		{
			cv::Mat image;
			decode(m_Frames[frameNum].getImageData(), image);
			return image;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Can't decode gpu video. " << e.what() << std::endl;
			return cv::Mat();
		}
	}

	cv::Mat GpuVideo::getFrameImage(size_t frameNum, const cv::Rect& roi) const
	{
		if (m_Frames.empty())
			return cv::Mat();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		cv::Mat image;
		try
		{
			decode(m_Frames[frameNum].getImageData(), image);
			return VideoUtils::getImageRoi(image, roi);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Can't decode gpu video. " << e.what() << std::endl;
			return cv::Mat();
		}
	}

	GpuVideo::Timestamp GpuVideo::getFrameTime(size_t frameNum) const
	{
		if (m_Frames.empty())
			return Timestamp();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		return m_Frames[frameNum].getTime();
	}

	std::vector<cv::Rect> GpuVideo::getFrameCrumbles(size_t frameNum) const
	{
		if (m_Frames.empty())
			return std::vector<cv::Rect>();
		assert(frameNum < m_Frames.size() && "frameNum is out of bound");
		return m_Frames[frameNum].getCrumbles();
	}

	bool GpuVideo::getNextFrame(cv::Mat& frame)
	{
		if (m_CurrentFrame >= m_Frames.size())
			return false;

		frame = getFrameImage(m_CurrentFrame++);
		return true;
	}

	void GpuVideo::setPosition(size_t frame)
	{
		m_CurrentFrame = frame;
	}


	void GpuVideo::encode(const cv::Mat& input, std::vector<unsigned char>& output) const
	{
		if (!m_Encoder)
			m_Encoder = std::make_shared<utils::GpuJpegEncoder>(m_ImageCompression);

		m_Encoder->encode(input, output);
	}

	void GpuVideo::decode(const std::vector<unsigned char>& input, cv::Mat& output) const
	{
		if (!m_Decoder)
			m_Decoder = std::make_shared<utils::GpuJpegDecoder>();

		m_Decoder->decode(input, output);
	}
}