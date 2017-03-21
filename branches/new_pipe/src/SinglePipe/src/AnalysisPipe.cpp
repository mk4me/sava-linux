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

using namespace clustering;


AnalysisPipe::AnalysisPipe(const std::shared_ptr<AquisitionFifo>& fifo, const std::shared_ptr<AnalysisFifo>& analysisFifo) :
	m_aquisitionFifo(fifo),
	m_analysisFifo(analysisFifo)
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

	while (m_AnalysisRunning)
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
	}
}


