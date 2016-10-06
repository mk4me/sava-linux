#include "IStreamedVideo.h"
#include "StreamedVideo.h"
#include "Video.h"
#include "CompressedVideo.h"
#include "RoiVideo.h"

#include <map>
namespace sequence
{
	template<typename T>
	static std::shared_ptr<IStreamedVideo> createObj(const std::string& filename)
	{
		return std::make_shared<T>(filename);
	}

	std::shared_ptr<IStreamedVideo> IStreamedVideo::create(const std::string& filename)
	{
		static struct CreatorStruct
		{
			typedef std::shared_ptr<IStreamedVideo>(*CreatrionFunc)(const std::string&);
			typedef std::map<std::string, CreatrionFunc> CreatorsMap;
			
			CreatorsMap creators;

			CreatorStruct()
			{
				creators[".vsq"] = &createObj<Video>;
				creators[".cvs"] = &createObj<CompressedVideo>;
				creators[".clu"] = &createObj<RoiVideo>;
			}

			std::shared_ptr<IStreamedVideo> create(const std::string& filename)
			{
				std::string extension;
				size_t index = filename.find_last_of('.');
				if (index != std::string::npos)
					extension = filename.substr(index);

				auto it = creators.find(extension);
				if (it == creators.end())
					return std::make_shared<StreamedVideo>(filename);

				return it->second(filename);
			}

		} cs;
	
		return cs.create(filename);
	}
	
	IStreamedVideo& IStreamedVideo::operator>>(cv::Mat& frame)
	{
		if (!getNextFrame(frame))
			frame = cv::Mat();
	
		return *this;
	}
}

std::shared_ptr<sequence::IStreamedVideo> operator>>(std::shared_ptr<sequence::IStreamedVideo> video, cv::Mat& frame)
{
	if (!video->getNextFrame(frame))
		frame = cv::Mat();

	return video;
}