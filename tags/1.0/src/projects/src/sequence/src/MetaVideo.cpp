#include "MetaVideo.h"

#include "CompressedVideo.h"
#include "GpuVideo.h"
#include "Cluster.h"
#include "Action.h"

#include "utils/Filesystem.h"

namespace sequence
{
	MetaVideo::MetaVideo(const std::string& filename)
	{
		load(filename);
	}

	MetaVideo::~MetaVideo()
	{

	}

	bool MetaVideo::load(const std::string& filename)
	{
		m_FileName = filename;
		try
		{
			m_Video = IVideo::create(m_FileName + ".cvs");
			if (!m_Video)
				return false;

			bool success = true;
			for (int i = 0;; ++i)
			{
				std::string clusterFile = m_FileName + "." + std::to_string(i) + ".clu";
				std::string actionFile = m_FileName + "." + std::to_string(i) + ".acn";

				if (!utils::Filesystem::exists(clusterFile))
					break;

				auto cluster = std::make_shared<Cluster>();
				success &= cluster->load(clusterFile);

				std::shared_ptr<Action> action;
				if (utils::Filesystem::exists(actionFile))
				{
					action = std::make_shared<Action>();
					success &= action->load(actionFile);
				}
				m_Objects.push_back(Object(cluster, action));
			}

			return success;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::MetaVideo::load() exception: " << e.what() << std::endl;
			return false;			
		}
	}

}