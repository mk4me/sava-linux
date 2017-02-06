#pragma once
#ifndef Config_Compression_h__
#define Config_Compression_h__

#include <boost/serialization/access.hpp>

namespace config
{
	class Compression
	{
	public:
		enum
		{
			METHOD_CRUMBLES,
			METHOD_GPU,
		};

		static Compression& getInstance();

		bool load();
		bool save() const;

		int getCompressionMethod() const { return m_CompressionMethod; }
		void setCompressionMethod(int val) { m_CompressionMethod = val; }

		int getFilesInPackage() const { return m_FilesInPackage; }
		void setFilesInPackage(int val) { m_FilesInPackage = val; }
		int getImageCompression() const { return m_ImageCompression; }
		void setImageCompression(int val) { m_ImageCompression = val; }
		int getBackgroundHistory() const { return m_BackgroundHistory; }
		void setBackgroundHistory(int val) { m_BackgroundHistory = val; }
		int getDifferenceThreshold() const { return m_DifferenceThreshold; }
		void setDifferenceThreshold(int val) { m_DifferenceThreshold = val; }
		float getNewBackgroundMinPixels() const { return m_NewBackgroundMinPixels; }
		void setNewBackgroundMinPixels(float val) { m_NewBackgroundMinPixels = val; }
		int getMinCrumbleArea() const { return m_MinCrumbleArea; }
		void setMinCrumbleArea(int val) { m_MinCrumbleArea = val; }
		int getMergeCrumblesIterations() const { return m_MergeCrumblesIterations; }
		void setMergeCrumblesIterations(int val) { m_MergeCrumblesIterations = val; }

	private:
		Compression();
		~Compression();

		int m_FilesInPackage;
		int m_ImageCompression;
		int m_BackgroundHistory;
		int m_DifferenceThreshold;
		float m_NewBackgroundMinPixels;
		int m_MinCrumbleArea;
		int m_MergeCrumblesIterations;

		int m_CompressionMethod;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void Compression::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_FilesInPackage;
		ar & m_ImageCompression;
		ar & m_BackgroundHistory;
		ar & m_DifferenceThreshold;
		ar & m_NewBackgroundMinPixels;
		ar & m_MinCrumbleArea;
		ar & m_MergeCrumblesIterations;

		ar & m_CompressionMethod;
	}
}

#endif // Config_Compression_h__