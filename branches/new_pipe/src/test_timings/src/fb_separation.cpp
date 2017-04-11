#include "fb_separation.h"
#include <iostream>
#include <random>
#include <list>
#include <sequence/IStreamedVideo.h>
#include <sequence/Video.h>
#include <utils/timer.h>
#include <boost/filesystem.hpp>
#include <string>
#include <sequence/Cluster.h>
#include <sequence/BackgroundSeparation.h>

Frames convert_videos(const VideoList& vl)
{
	Frames res;

	cv::Mat frame;
	for (auto& video : vl) {
		while(video->getNextFrame(frame)) {
			res.push_back(frame);
		}
	}
		

	return res;
}

VideoList load_files(const std::string& directory_path)
{
	VideoList result;

	std::vector<std::pair<boost::filesystem::path, int>> names;
	for (auto it = boost::filesystem::directory_iterator(directory_path); it != boost::filesystem::directory_iterator(); ++it)
	{
		auto path = it->path();

		std::string ext = path.extension().string();
		if (ext != ".vsq" && ext != ".cvs" && ext != ".avi" && ext != ".mp4")
			continue;

		std::string idStr = path.stem().string();
		size_t pos = idStr.find_last_of('.');
		if (pos != std::string::npos)
		{
			idStr = idStr.substr(pos + 1);

			names.push_back(std::make_pair(path, std::stoi(idStr)));
		}
	}

	std::sort(names.begin(), names.end(), [](const std::pair<boost::filesystem::path, int>& p0, const std::pair<boost::filesystem::path, int>& p1)
	{
		return p0.second < p1.second;
	});

	size_t loaded = 0;
	size_t progress = 0;
	std::cout << "Loading..." << std::endl;
	for (auto& e : names)
	{
		auto video = sequence::IStreamedVideo::create(e.first.string());
		++loaded;
		size_t realProgress = (loaded * 100) / names.size();
		if (realProgress >= progress + 10)
		{
			progress = realProgress;
			std::cout << progress << "%" << std::endl;
		}
		if (video && video->getNumFrames() > 0)
		{
			result.push_back(video);
		}
	}
	std::cout << result.size() << " videos loaded." << std::endl;
	return result;
}

template <typename It>
double avg(It begin, It end)
{
	double sum = 0;
	for (auto it = begin; it != end; ++it) {
		sum += *it;
	}

	return sum / std::distance(begin, end);
}

void print_frames_time(const std::vector<double>& frame_time)
{
	std::cout << "avg frame time: " << avg(frame_time.begin(), frame_time.end()) << std::endl;
	
	int stride = 10;
	auto i = frame_time.begin();
	auto j = i + stride;

	while (j < frame_time.end()) {
		std::cout << "part frame time: " << avg(i, j) << std::endl;
		i = j;
		j += stride;
	}

	std::cout << "part frame time: " << avg(i, frame_time.end()) << std::endl;
}


RectVect fb_separation_timing(const Frames& frames)
{
	RectVect rects;
	std::cout << "test 1" << std::endl;
	utils::timer whole(true);		
	std::vector<double> frame_time;
	sequence::FBSeparator separator;
    separator.reset();	

	for (auto& frame : frames) {
		utils::timer t(true);
		auto crumbles = separator.separate(frame);
		rects.push_back(crumbles);
		frame_time.push_back(t.secondsSinceStart());
	}
	whole.print("FULL FB TEST TIME: ");

	print_frames_time(frame_time);

	return rects;
}

