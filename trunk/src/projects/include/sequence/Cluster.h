#pragma once
#ifndef Cluster_h__
#define Cluster_h__

#include <utils/CvSerialization.h>

#include <opencv/cv.h>

#include <boost/serialization/map.hpp>

#include <string>
#include <map>

namespace sequence
{
	class Cluster
	{
	public:
		typedef std::map<size_t, cv::Point> Path;
		typedef std::map<size_t, cv::Point> FramesPositionsMap;
		typedef cv::Size FrameSize;		

		/// <summary>
		/// Invalid frame number.
		/// </summary>
		static const size_t NPOS;

		static const int INVALID_CLUSTER;

		explicit Cluster(const std::string& filename);
		explicit Cluster(int clusterId = INVALID_CLUSTER) : m_ClusterId(clusterId) { }
		~Cluster() { }

		bool load(const std::string& filename);
		bool loadFromText(const std::string& filename);
		bool save(const std::string& filename) const;
		bool saveAsText(const std::string& filename) const;

		int getClusterId() const { return m_ClusterId; }		

		/// <summary>
		/// Gets the start frame.
		/// </summary>
		/// <returns>Start frame. Returns NPOS if no frame exists.</returns>
		size_t getStartFrame() const;
		/// <summary>
		/// Gets the end frame.
		/// </summary>
		/// <returns>End frame. Returns NPOS if no frame exists.</returns>
		size_t getEndFrame() const;
		
		/// <summary>
		/// Gets the frame rect. Uses interpolation between key frames.
		/// </summary>
		/// <param name="frameNum">The frame number. If frameNum is less then startFrame or more then endFrame, function returns empty cv::Rect.</param>
		/// <returns>The frame rect.</returns>
		cv::Rect getFrameAt(size_t frameNum) const;

		/// <summary>
		/// Gets the frames positions. 
		/// </summary>
		/// <returns></returns>
		const FramesPositionsMap& getFramesPositions() const { return m_FramesPositions; }
		/// <summary>
		/// Sets the frames positions.
		/// </summary>
		/// <param name="frames">The frames.</param>
		void setFramesPositions(const FramesPositionsMap& frames) { m_FramesPositions = frames; }
		void setFramesPositions(FramesPositionsMap&& frames) { m_FramesPositions = std::move(frames); }
		
		/// <summary>
		/// Gets the size of one frame. All frames have the same size.
		/// </summary>
		/// <returns>The size</returns>
		const FrameSize& getFrameSize() const { return m_FrameSize; }
		/// <summary>
		/// Sets the size of one frame. All frames have the same size.
		/// </summary>
		/// <param name="value">The size.</param>
		void setFrameSize(const FrameSize& value) { m_FrameSize = value; }

		std::vector<std::string> getPathNames() const;
		cv::Point getPathAt(const std::string& name, size_t frameNum) const;

		const Path& getPath(const std::string& name) const;
		void setPath(const std::string& name, const Path& path);
		void setPath(const std::string& name, Path&& path);
		void removePath(const std::string& name);

		/// <summary>
		/// Gets point from frame collection (uses interpolation between key frames).
		/// </summary>
		/// <param name="collection">The collection. Key is a frame number, and value is a position.</param>
		/// <param name="frameNum">The frame number.</param>
		/// <param name="ok">If not nullptr, function sets its value to true if frameNum is inside <startFrame,endFrame></param>
		/// <returns>Interpolated point.</returns>
		static cv::Point lerp(const std::map<size_t, cv::Point>& collection, size_t frameNum, bool* ok = nullptr);
		
	private:
		int m_ClusterId;

		std::map<std::string, Path> m_Paths;
		FramesPositionsMap m_FramesPositions;
		FrameSize m_FrameSize;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void Cluster::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_ClusterId;
		ar & m_Paths;
		ar & m_FramesPositions;
		ar & m_FrameSize;
	}

}

#endif // Cluster_h__