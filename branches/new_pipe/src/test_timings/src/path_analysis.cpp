#include "path_analysis.h"
#include <iostream>
#include <utils/timer.h>
#include <sequence/Cluster.h>
#include <PathAnalysisAlgorithms/StreamAnalyzer.h>


void path_analysis_timing(const PathsVec& paths ) 
{
	//std::cout << "PATH ANALYSIS TEST" << std::endl;
	//utils::timer t(true);
	
	auto count = paths.size();
	PathsVec res;

	clustering::StreamAnalyzer m_StreamPathAnalysis;
	m_StreamPathAnalysis.loadParameters();
	std::vector<double> frame_time;
	for (size_t i = 0; i < count; ++i) {
		utils::timer t(true);	
		std::vector<sequence::Cluster> clusters = m_StreamPathAnalysis.processFrame(paths[i]);
        std::cout << "CLUSTERS SIZE : " << clusters.size() << std::endl;
		//FrameClustersPair p;
		frame_time.push_back(t.secondsSinceStart());

	}

	//t.print("Path analysis time: ");
	print_frames_time(frame_time);

}
