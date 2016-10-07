#pragma once
#ifndef PathStream_h__
#define PathStream_h__

#include <utils/CvSerialization.h>

#include <opencv2/core.hpp>

#include <boost/serialization/vector.hpp>

#include <string>
#include <vector>
#include <map>
#include <set>

namespace sequence
{
	/// <summary>
	/// Reads and writes file containing paths.
	/// </summary>
	class PathStream
	{
	public:
		typedef unsigned long long Id;
		typedef cv::Point Point;
		typedef unsigned long long Time;
		struct Path
		{
			Path() { }
			Path(Id id) : id(id) { }
			Path(Path&& p) : id(p.id), points(std::move(p.points)) { }
			Path& operator=(Path&& p) { id = p.id; points = std::move(p.points); return *this; }

			Id id;
			std::map<Time, Point> points;
		};

		/// <summary>
		/// Default constructor.
		/// </summary>
		PathStream() : m_FrameCursor(-1), m_PathCursor(-1) {}
		/// <summary>
		/// Constructor from file.
		/// </summary>
		/// <param name="filename">Name of file to load.</param>
		explicit PathStream(const std::string& filename);
		~PathStream() { }
		
		bool load(const std::string& filename);
		bool save(const std::string& filename) const;

		/// <summary>
		/// Adds frame.
		/// </summary>
		void addFrame();
		/// <summary>
		/// Adds path data to current frame.
		/// </summary>
		/// <param name="id">Path identifier.</param>
		/// <param name="point">Path coordinates in current frame.</param>
		void addPath(Id id, Point point);

		/// <summary>
		/// Grabs frame.
		/// </summary>
		/// <returns>true if frame exists</returns>
		bool grabFrame();
		/// <summary>
		/// Grabs path data from current frame.
		/// </summary>
		/// <param name="id">Path identifier.</param>
		/// <param name="point">Path coordinates in current frame.</param>
		/// <returns>true if path exists</returns>
		bool grabPath(Id& id, Point& point);
		/// <summary>
		/// Reset frame cursor to the beginning. Method grabFrame() must be called next.
		/// </summary>
		void rewind();

		/// <summary>
		/// Gets number of frames.
		/// </summary>
		/// <returns></returns>
		size_t size() const { return m_Frames.size(); }

		/// <summary>
		/// Gets all paths.
		/// </summary>
		/// <param name="outPaths">The out paths.</param>
		void getPaths(std::map<Id, Path>& outPaths) const;

		/// <summary>
		/// Gets the frame cursor.
		/// </summary>
		/// <returns></returns>
		long long getFrameCursor() const { return m_FrameCursor; }

		void clear();

	private:
		struct PathStorage
		{
		public:
			PathStorage() { }
			PathStorage(Id id, Point point) : id(id), point(point) { }

			Id id;
			Point point;

		private:
			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		typedef std::vector<PathStorage> Frame;

		std::vector<Frame> m_Frames;

		long long m_FrameCursor;
		long long m_PathCursor;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
	
	template<class Archive>
	void PathStream::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Frames;
	}

	template<class Archive>
	void PathStream::PathStorage::serialize(Archive & ar, const unsigned int version)
	{
		ar & id;
		ar & point;
	}
}

#endif // PathStream_h__