#include "PathStream.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>

namespace sequence
{
	PathStream::PathStream(const std::string& filename)
	{
		load(filename);
	}

	bool PathStream::load(const std::string& filename)
	{
		m_FrameCursor = -1;
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::PathStream::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool PathStream::save(const std::string& filename) const
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
			std::cerr << "sequence::PathStream::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	void PathStream::addFrame()
	{
		m_Frames.emplace_back(Frame());
	}

	void PathStream::addPath(Id id, Point point)
	{
		m_Frames.back().push_back(PathStorage(id, point));
	}

	bool PathStream::grabFrame()
	{
		m_PathCursor = -1;
		return ++m_FrameCursor < (long long)m_Frames.size();
	}

	bool PathStream::grabPath(Id& id, Point& point)
	{
		if (m_FrameCursor < 0 || m_FrameCursor >= (long long)m_Frames.size())
			return false;

		auto& frame = m_Frames[m_FrameCursor];
		if (++m_PathCursor >= (long long)frame.size())		
			return false;		

		auto& path = frame[m_PathCursor];
		id = path.id;
		point = path.point;
		return true;
	}

	void PathStream::rewind()
	{
		m_FrameCursor = -1;
	}

	void PathStream::getPaths(std::map<Id, Path>& outPaths)
	{
		std::map<Id, Path> paths;
		for (Time t = 0; t < (Time)m_Frames.size(); ++t)
		{
			auto& frame = m_Frames[t];
			for (auto& p : frame)
			{
				paths[p.id].points.insert(std::make_pair(t, p.point));
			}
		}
		outPaths = std::move(paths);
	}

}