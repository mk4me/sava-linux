#pragma once
#ifndef Config_Aquisition_h__
#define Config_Aquisition_h__

#include <boost/serialization/access.hpp>

namespace config
{
	class Aquisition
	{
	public:
		Aquisition();

		bool load();
		bool save() const;

		int getSeqLength() const { return m_SeqLength; }
		void setSeqLength(int val) { m_SeqLength = val; }
		size_t getFps() const { return m_Fps; }
		void setFps(size_t val) { m_Fps = val; }
		size_t getCompression() const { return m_Compression; }
		void setCompression(size_t val) { m_Compression = val; }

	private:
		int m_SeqLength;
		size_t m_Fps;
		size_t m_Compression;
		
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void Aquisition::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_SeqLength;
		ar & m_Fps;
		ar & m_Compression;
	}
}

#endif // Config_Aquisition_h__