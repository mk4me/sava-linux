#include "PathStream.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <iostream>

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

	void PathStream::getPaths(std::map<Id, Path>& outPaths) const
	{
		std::map<Id, Path> paths;
		for (Time t = 0; t < (Time)m_Frames.size(); ++t)
		{
			auto& frame = m_Frames[t];
			for (auto& p : frame)
			{
				Path& path = paths[p.id];
				path.points.insert(std::make_pair(t, p.point));
				path.id = p.id;
			}
		}
		for (auto& p : paths)
		{
			Path& path = p.second;
			if (path.points.size() > 1)
			{
				std::map<Time, Point> newPoints;
				auto prev = path.points.begin();
				Time pt = prev->first;
				cv::Point pp = prev->second;

				for (auto next = std::next(prev); next != path.points.end(); ++next)
				{
					Time nt = next->first;
					cv::Point np = next->second;
					if (np.x < 0)
						break;

					newPoints.insert(std::make_pair(pt, pp));

					for (Time t = pt + 1; t < nt; ++t)
					{
						float tf = (t - pt) / float(nt - pt);
						Point p;
						p.x = pp.x + int((np.x - pp.x) * tf);
						p.y = pp.y + int((np.y - pp.y) * tf);
						newPoints.insert(std::make_pair(t, p));
					}

					pt = nt;
					pp = np;
				}
				newPoints.insert(std::make_pair(pt, pp));
				path.points.swap(newPoints);
			}
		}
		outPaths = std::move(paths);
	}

	void PathStream::clear()
	{
		std::vector<Frame>().swap(m_Frames);
		m_FrameCursor = -1;
		m_PathCursor = -1;
	}

}