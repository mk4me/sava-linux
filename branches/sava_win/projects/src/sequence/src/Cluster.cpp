#include "Cluster.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"

namespace sequence
{
	const size_t Cluster::NPOS = size_t(-1);

	/// <summary>
	/// The invali d_ cluster{CC2D43FA-BBC4-448A-9D0B-7B57ADF2655C}
	/// </summary>
	const int Cluster::INVALID_CLUSTER = -1;

	Cluster::Cluster(const std::string& filename)
	{
		load(filename);
	}

	template <class Archive>
	bool _clusterLoad(const std::string& filename, Cluster* self)
	{
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			Archive ia(ifs);
			ia >> *self;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::Cluster::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Cluster::load(const std::string& filename)
	{
		return _clusterLoad<boost::archive::binary_iarchive>(filename, this);
	}
	bool Cluster::loadFromText(const std::string& filename)
	{
		return _clusterLoad<boost::archive::text_iarchive>(filename, this);
	}

	template <class Archive>
	bool _clusterSave(const std::string& filename, const Cluster* cluster)
	{
		try
		{
			std::ofstream ofs(filename, std::ios::binary | std::ios::out);
			Archive oa(ofs);
			oa << *cluster;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::Cluster::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Cluster::save(const std::string& filename) const
	{
		return _clusterSave<boost::archive::binary_oarchive>(filename, this);
	}

	bool Cluster::saveAsText(const std::string& filename) const
	{
		return _clusterSave<boost::archive::text_oarchive>(filename, this);
	}

	size_t Cluster::getStartFrame() const
	{
		if (m_FramesPositions.empty())
			return NPOS;
		return m_FramesPositions.begin()->first;
	}

	size_t Cluster::getEndFrame() const
	{
		if (m_FramesPositions.empty())
			return NPOS;
		return m_FramesPositions.rbegin()->first;
	}

	cv::Rect Cluster::getFrameAt(size_t frameNum) const
	{
		bool ok;
		cv::Point p = lerp(m_FramesPositions, frameNum, &ok);
		if (ok)
			return cv::Rect(p, m_FrameSize);
		return cv::Rect();
	}

	std::vector<std::string> Cluster::getPathNames() const
	{
		std::vector<std::string> names;
		names.reserve(m_Paths.size());
		for (auto path: m_Paths)
		{
			names.push_back(path.first);
		}
		return names;
	}

	cv::Point Cluster::getPathAt(const std::string& name, size_t frameNum) const
	{
		auto& path = getPath(name);
		return lerp(path, frameNum);
	}

	const Cluster::Path& Cluster::getPath(const std::string& name) const
	{
		auto it = m_Paths.find(name);
		assert(it != m_Paths.end() && "path doesn't exist");
		return it->second;
	}

	void Cluster::setPath(const std::string& name, const Path& path)
	{
		m_Paths[name] = path;
	}

	void Cluster::setPath(const std::string& name, Path&& path)
	{
		m_Paths[name] = std::move(path);
	}

	void Cluster::removePath(const std::string& name)
	{
		m_Paths.erase(name);
	}

	cv::Point Cluster::lerp(const std::map<size_t, cv::Point>& collection, size_t frameNum, bool* ok /*= nullptr*/)
	{
		auto frameIt = collection.find(frameNum);
		if (frameIt != collection.end())
		{
			if (ok) *ok = true;
			return frameIt->second;			
		}

		if (collection.empty() || frameNum < collection.begin()->first || frameNum > collection.rbegin()->first)
		{
			if (ok) *ok = false;
			return cv::Point();
		}

		auto nextFrameIt = collection.upper_bound(frameNum);
		auto prevFrameIt = std::prev(nextFrameIt);

		float timeFactor = float(frameNum - prevFrameIt->first) / float(nextFrameIt->first - prevFrameIt->first);

		cv::Vec2f prevPos = cv::Vec2i(prevFrameIt->second);
		cv::Vec2f nextPos = cv::Vec2i(nextFrameIt->second);

		if (ok) *ok = true;
		return cv::Vec2i(prevPos + (nextPos - prevPos) * timeFactor);		
	}

}