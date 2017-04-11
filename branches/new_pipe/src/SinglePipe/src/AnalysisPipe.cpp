#include "AnalysisPipe.h"

#include <vector>
#include <sequence/Video.h>

#include <config/Aquisition.h>
#include <config/Diagnostic.h>

#include <utils/Filesystem.h>
#include <utils/Application.h>
#include <utils/AxisRawReader.h>
#include <utils/PipeProcessUtils.h>
#include <utils/FileLock.h>

#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>
#include <config/PathDetection.h>
#include <PathDetectionLib/DefaultPathDetector.h>
#include <PathDetectionLib/PredictedRoiPathDetector.h>
#include <sequence/Cluster.h>
#include <utils/timer.h>
#include "AquisitionPipe.h"
#include <sstream>
#include <tbb/flow_graph.h>

using namespace clustering;
typedef AquisitionFifo::FramePtr FramePtr;

AnalysisPipe::AnalysisPipe(const std::shared_ptr<AquisitionFifo>& fifo, const std::shared_ptr<AnalysisFifo>& analysisFifo) :
	m_analysisFifo(analysisFifo),
	m_aquisitionFifo(fifo)
{

}

bool AnalysisPipe::isRunning() const
{
    return m_AnalysisRunning;
}

void AnalysisPipe::visualize()
{
	std::lock_guard<std::mutex> lock(m_VisualizeMutex);
	utils::zoomShow("Dbg", m_VisualizationFrame, m_ZoomObjects);
	cv::waitKey(1);
}

bool AnalysisPipe::start()
{

	try {
        m_fbSeparator.reset();
        if (config::PathDetection::getInstance().isPredictedRoiDetector())
			m_PathDetector = std::make_shared<PredictedRoiPathDetector>();
		else
		m_PathDetector = std::make_shared<DefaultPathDetector>();
		m_PathDetector->initialize();
		m_PathDetector->setPathStream(m_PathStream);
		m_StreamPathAnalysis.loadParameters();
		m_AnalysisRunning = true;
		boost::thread t(&AnalysisPipe::analysisThreadFunc, this);
		m_AnalysisThread.swap(t);


		//tbb::flow::limiter_node limiter( g, nthreads*4 );
		//tbb::flow::sequencer_node< TextSlice * > sequencer(g, sequencer_body() );

		tbb::flow::source_node<FramePtr> input( g, [&](FramePtr& frame) -> bool 
		{  
			//	utils::camera::MJPGFrame jpegFrame;
			//	m_FrameReader->popRawFrame(jpegFrame);
			//	auto frame = std::make_shared<sequence::Video::Frame>(jpegFrame.m_TimeStamp, std::move(jpegFrame.m_JPEGFrame));
			//	return frame;
			bool res = false;
			do 
			{
				res = m_aquisitionFifo->frames.try_pop(frame);
				if (!res) {
					std::this_thread::yield();
				} 
			} while (res == false);
            std::cout << "Frame Input." << std::endl;
			return true;
		
		});

		typedef sequence::FBSeparator::Rectangles Rects;
		typedef std::pair<FramePtr, Rects> FrameRectsPair;
		tbb::flow::function_node<FramePtr, FrameRectsPair> fb_transform( g, tbb::flow::serial,
		[&](FramePtr frame) -> FrameRectsPair
		{
            std::cout << "Frame FB Sep." << std::endl;
			Rects rects = m_fbSeparator.separate(frame->second);

			return std::make_pair(frame, rects);	
		});

		typedef std::map<sequence::PathStream::Id, sequence::PathStream::Path> Paths;
		typedef std::pair<FramePtr, Paths> FramePathsPair;
		tbb::flow::function_node<FrameRectsPair, FramePathsPair> pathD_transform( g, tbb::flow::unlimited, 
		[&](FrameRectsPair pair) -> FramePathsPair
		{
            std::cout << "Frame Path detect." << std::endl;
			// detecting paths
			m_PathStream.addFrame();
			m_PathDetector->setCrumbles(std::move(pair.second));
			m_PathDetector->processFrame(pair.first->second);
			std::map<sequence::PathStream::Id, sequence::PathStream::Path> paths;
			m_PathStream.getPaths(paths);
			return std::make_pair(pair.first, paths);
		});



		typedef std::vector<sequence::Cluster> Clusters;
		typedef std::pair<FramePtr, Clusters> FrameClustersPair;
		tbb::flow::function_node<FramePathsPair, FrameClustersPair> pathA_transform( g, tbb::flow::unlimited, 
		[&](FramePathsPair pair) -> FrameClustersPair
		{
            std::cout << "Frame Path analysis." << std::endl;
			std::vector<sequence::Cluster> clusters = m_StreamPathAnalysis.processFrame(pair.second);
			FrameClustersPair p;
			p.first = pair.first;
			p.second = clusters;
			return p;
			//return std::make_pair<FramePtr, Clusters>(pair.first, clusters);
		});


		tbb::flow::function_node<FrameClustersPair, int> output(g, tbb::flow::unlimited, 
		[&](const FrameClustersPair pair) 
		{
			std::cout << "Frame done." << std::endl;
			return 0;
		});

		//tbb::flow::make_edge( input, limiter );
		//tbb::flow::make_edge( limiter, transform );
		tbb::flow::make_edge( input, fb_transform);
		tbb::flow::make_edge( fb_transform, pathD_transform );
		tbb::flow::make_edge( pathD_transform, pathA_transform );
		tbb::flow::make_edge( pathA_transform, output );
		//tbb::flow::make_edge( output, limiter.decrement );

		input.activate();

    } 
	catch (const std::exception& e)
	{
		std::cerr << "Aquisition::start() error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Aquisition::start() Unknown error:" << std::endl;
	}

	return true;
} 

void AnalysisPipe::stop()
{
	m_AnalysisRunning = false;
	if (m_AnalysisThread.joinable())
		m_AnalysisThread.join();
	std::cout << "Quitting..." << std::endl;
}

void AnalysisPipe::analysisThreadFunc()
{
	g.wait_for_all();
	/*while (m_AnalysisRunning)
	{
        // for current frame
        std::stringstream stream;
        utils::timer timelog(true, stream);
		sequence::Video::Frame frame;

		bool res = m_aquisitionFifo->frames.try_pop(frame);
		if (!res) {
			std::this_thread::yield();
		} else {
            timelog.print("Getting frame: ");
			auto crumbles = m_fbSeparator.separate(frame.getCvMat());
            timelog.print("FB Separation: ");
			//auto pair = std::make_pair(frame, crumbles);
			//AquisitionFifo::frames.push(pair);

			// detecting paths
			m_PathStream.addFrame();
			m_PathDetector->setCrumbles(std::move(crumbles));
			m_PathDetector->processFrame(frame.getCvMat());
			std::map<sequence::PathStream::Id, sequence::PathStream::Path> paths;
			m_PathStream.getPaths(paths);

            timelog.print("Detection: ");
			std::vector<sequence::Cluster> clusters = m_StreamPathAnalysis.processFrame(paths);

			m_analysisFifo->frames.push_back(std::make_pair(frame, clusters));
            timelog.print("Analysis: ");

            std::string s = stream.str();
            std::cout << s << std::endl;
			continue;
			utils::ZoomObjectCollection zoomObjects;
			m_PathDetector->visualize(zoomObjects);
			m_StreamPathAnalysis.visualize(zoomObjects);
			std::lock_guard<std::mutex> lock(m_VisualizeMutex);
			m_ZoomObjects = zoomObjects;
			frame.getCvMat().copyTo(m_VisualizationFrame);

		}
	}*/
}


