#include "IVideo.h"

#include "CompressedVideo.h"
#include "Video.h"
#include "GpuVideo.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <cassert>

namespace sequence
{
	const unsigned IVideo::SEQUENCE_VIDEO = 'v' | 's' << 8 | 'q' << 16;
	const unsigned IVideo::SEQUENCE_COMPRESSED_VIDEO = 'c' | 'v' << 8 | 's' << 16;
	const unsigned IVideo::SEQUENCE_GPU_VIDEO = 'g' | 'v' << 8 | 's' << 16;

	std::shared_ptr<IVideo> IVideo::create(const std::string& filename)
	{
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			int type;
			ia >> type;
			switch (type)
			{
			case IVideo::SEQUENCE_VIDEO:
			{
				auto obj = std::make_shared<Video>();
				ia >> *obj;
				return obj;
			}
			case IVideo::SEQUENCE_COMPRESSED_VIDEO:
			{
				auto obj = std::make_shared<CompressedVideo>();
				ia >> *obj;
				return obj;
			}
			case IVideo::SEQUENCE_GPU_VIDEO:
			{
				std::runtime_error("sava-linux.rev");
				/*auto obj = std::make_shared<GpuVideo>();
				ia >> *obj;
				return obj;*/
			}
			}
		}
		catch (const std::exception& ex)
		{
			std::cerr << "IVideo::create error: " << ex.what() << std::endl;
		}

		return std::shared_ptr<IVideo>();
	}
}