#include <iostream>
#include "fb_separation.h"
#include "path_analysis.h"
#include "path_detection.h"
#include <utils/timer.h>
#include <PathDetectionLib/DefaultPathDetector.h>
#include <PathDetectionLib/PredictedRoiPathDetector.h>
int main()
{
	// std::string g_InputFolder = "~/testowe_dane";
	utils::timer t(true);
	auto videos = load_files("/home/mc/testowe_dane");
	t.print("loading files: ");

	utils::timer t2(true);
	auto frames = convert_videos(videos);
	t2.print("converting: ");

	auto rects = fb_separation_timing(frames);
//	auto paths1 = path_detection_timinig(frames, rects, std::make_shared<clustering::PredictedRoiPathDetector>());
	auto paths2 = path_detection_timinig(frames, rects, std::make_shared<clustering::DefaultPathDetector>());

	//std::cout << "Analysis 1" << std::endl;
	//path_analysis_timing(paths1);

	std::cout << "Analysis 2" << std::endl;
	path_analysis_timing(paths2);
	return 0;
}
