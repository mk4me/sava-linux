#include "path_detection.h"
#include <iostream>

#include <sequence/Cluster.h>
#include <sequence/PathStream.h>
#include <utils/timer.h>
#include <PathAnalysisAlgorithms/StreamAnalyzer.h>
#include <PathDetectionLib/IPathDetector.h>
#include <PathDetectionLib/OpticalFlowPathDetector.h>
#include <PathDetectionLib/DefaultPathDetector.h>

PathsVec path_detection_timinig(const Frames& frames, const RectVect& rv, std::shared_ptr<clustering::IPathDetector> m_PathDetector)
{
	std::cout << "test detection" << std::endl;
	
	//hack : zahardcodowany path detector
	//auto pathDetector = std::make_shared<clustering::OpticalFlowPathDetector>();
    auto pathDetector = std::make_shared<clustering::DefaultPathDetector>();
	m_PathDetector = pathDetector;

//	std::shared_ptr<clustering::IPathDetector> m_PathDetector;
	sequence::PathStream m_PathStream;
    m_PathStream.clear();
	//clustering::StreamAnalyzer m_StreamPathAnalysis;
	
	m_PathDetector->initialize();
	//ypathDetector->setDetectionInterval(17);
	m_PathDetector->setPathStream(m_PathStream);
	
	auto count = frames.size();
	assert(count == rv.size());
	PathsVec res;

    std::set<sequence::PathStream::Id> ids;
	std::vector<double> frame_time;
	size_t maxPathsCount = 0; 

	for (size_t i = 0; i < count; ++i) { 
		utils::timer t(true);	
		m_PathStream.addFrame();
		m_PathDetector->setCrumbles(rv[i]);
		m_PathDetector->processFrame(frames[i]);
		Paths paths;
        utils::timer t2(true);
		m_PathStream.getLastestPaths(paths);
        std::cout << "t2 time:" << t2.secondsSinceStart() << std::endl;
		frame_time.push_back(t.secondsSinceStart());

        auto pathsCount = m_PathDetector->getPathsCount();
        // hack: dlaczego pierwsze klatki maja wiecej niz 30k klatek?
        if (i < 10 && pathsCount > 20000) {
            res.push_back(Paths());
        } else {
            res.push_back(paths);
        }

        maxPathsCount = std::max(maxPathsCount, pathsCount);
        std::cout << "paths size for " << i << ": " << pathsCount << ", path.size(): " << paths.size() << std::endl; //paths.size() << std::endl;
        for (auto& p : paths) {
                ids.insert(p.first);
        }
	}
    std::cout << "IDS SIZE!: " << ids.size() << std::endl;

	print_frames_time(frame_time);
	return res;
}
