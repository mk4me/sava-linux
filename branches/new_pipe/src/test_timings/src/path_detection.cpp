#include "path_detection.h"
#include <iostream>

#include <sequence/Cluster.h>
#include <sequence/PathStream.h>
#include <utils/timer.h>
#include <PathAnalysisAlgorithms/StreamAnalyzer.h>
#include <PathDetectionLib/IPathDetector.h>

PathsVec path_detection_timinig(const Frames& frames, const RectVect& rv, std::shared_ptr<clustering::IPathDetector> m_PathDetector)
{
	std::cout << "test detection" << std::endl;
	
//	std::shared_ptr<clustering::IPathDetector> m_PathDetector;
	sequence::PathStream m_PathStream;
	clustering::StreamAnalyzer m_StreamPathAnalysis;
	
	m_PathDetector->initialize();
	m_PathDetector->setPathStream(m_PathStream);
	
	auto count = frames.size();
	assert(count == rv.size());
	PathsVec res;

	std::vector<double> frame_time;
	for (size_t i = 0; i < count; ++i) { 
		utils::timer t(true);	
		m_PathStream.addFrame();
		m_PathDetector->setCrumbles(rv[i]);
		m_PathDetector->processFrame(frames[i]);
		Paths paths;
		m_PathStream.getPaths(paths);
		frame_time.push_back(t.secondsSinceStart());
		res.push_back(paths);
	}

	print_frames_time(frame_time);
	return res;
}
