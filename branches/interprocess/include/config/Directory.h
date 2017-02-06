#pragma once
#ifndef Directory_h__
#define Directory_h__

#include <string>

namespace boost
{
	namespace serialization
	{
		class access;
	}
}

namespace config
{
	class Directory
	{
		friend class boost::serialization::access;
	
	public:
		static Directory& getInstance();

		bool load();
		bool save() const;

		const std::string& getVideosPath() const { return m_VideosPath; }
		void setVideosPath(const std::string& path);

		const std::string& getTemporaryPath() const { return m_TemporaryPath; }
		void setTemporaryPath(const std::string& path);

		const std::string& getAlertsPath() const { return m_AlertsPath; }
		void setAlertsPath(const std::string& path);

	private:
		Directory() { }
		~Directory() { }

		std::string m_VideosPath;
		std::string m_TemporaryPath;
		std::string m_AlertsPath;
		
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // Directory_h__
