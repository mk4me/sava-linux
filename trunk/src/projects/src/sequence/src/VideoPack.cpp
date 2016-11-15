#include "VideoPack.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <fstream>

namespace sequence
{
	const std::string VideoPack::c_EmptyString;

	template<class Archive>
	void sequence::VideoPack::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_FileNames;
		ar & m_CenterVideoIndex;
	}

	VideoPack::VideoPack(const std::vector<std::string>& videoFiles)
		: m_FileNames(videoFiles)
		, m_CenterVideoIndex(-1)
	{
		loadVideos();
	}

	VideoPack::VideoPack(const std::string& filename)
	{
		load(filename);
	}

	size_t VideoPack::getNumFrames() const
	{
		return m_Length;
	}

	bool VideoPack::getNextFrame(cv::Mat& frame)
	{
		while (m_CurrentFile < m_Files.size())
		{
			if (!m_Files[m_CurrentFile].video->getNextFrame(frame))
			{
				++m_CurrentFile;
				if (m_CurrentFile < m_Files.size())
					m_Files[m_CurrentFile].video->setPosition(0);
				continue;
			}
			return true;
		}

		return false;
	}

	void VideoPack::setPosition(size_t frame)
	{
		m_CurrentFile = 0;
		while (m_CurrentFile < m_Files.size() && frame >= m_Files[m_CurrentFile].endPos)
			++m_CurrentFile;

		if (m_CurrentFile < m_Files.size())
			m_Files[m_CurrentFile].video->setPosition(m_CurrentFile == 0 ? frame : frame - m_Files[m_CurrentFile - 1].endPos);
	}

	bool VideoPack::load(const std::string& filename)
	{
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			loadVideos();

			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::VideoPack::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool VideoPack::save(const std::string& filename) const
	{
		try
		{
			std::ofstream ofs(filename, std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::VideoPack::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	const std::string& VideoPack::getCenterVideoName() const
	{
		if (m_CenterVideoIndex < 0 || m_CenterVideoIndex >= m_FileNames.size())
			return c_EmptyString;

		return m_FileNames[m_CenterVideoIndex];
	}

	void VideoPack::loadVideos()
	{
		m_CurrentFile = 0;
		m_Length = 0;
		for (const std::string& file : m_FileNames)
		{
			auto video = IStreamedVideo::create(file);
			m_Length += video->getNumFrames();
			m_Files.push_back(Video(video, m_Length));
		}
	}

}
