#pragma once
#ifndef MetaVideo_h__
#define MetaVideo_h__

#include <memory>
#include <vector>
#include <string>

namespace sequence
{
	class IVideo;
	class Cluster;
	class Action;

	/// <summary>
	/// klasa kontenera zawieraj¹cego pojedyncz¹ sekwencjê sk³adaj¹ca sie z wideo, listy obiektów i ich akcji. Klasa wyszukuje pliki cvs, clu i acn.
	/// </summary>
	class MetaVideo
	{
	public:
		MetaVideo() { }
		explicit MetaVideo(const std::string& filename);
		~MetaVideo();

		bool load(const std::string& filename);

		std::shared_ptr<IVideo> getVideo() { return m_Video; }

		size_t getObjectsNum() const { return m_Objects.size(); }
		std::shared_ptr<Cluster> getObjectCluster(size_t objectNum) { return m_Objects[objectNum].first; }
		std::shared_ptr<Action> getObjectAction(size_t objectNum) { return m_Objects[objectNum].second; }

		std::string getFileName() const { return m_FileName; }

	private:
		std::shared_ptr<IVideo> m_Video;
		typedef std::pair<std::shared_ptr<Cluster>, std::shared_ptr<Action>> Object;
		std::vector<Object> m_Objects;

		std::string m_FileName;
	};
}

#endif // MetaVideo_h__