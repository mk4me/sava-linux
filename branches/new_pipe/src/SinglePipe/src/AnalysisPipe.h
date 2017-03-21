#pragma once
#ifndef AnalysisPipe_h__
#define AnalysisPipe_h__

#include <utils/IAppModule.h>
#include <utils/CvZoom.h>
#include <boost/thread.hpp>

#include <atomic>
#include <QtCore/QObject>
#include <utils/AxisRawReader.h>
#include <utils/BlockingQueue.h>
#include <sequence/Video.h>
#include <sequence/BackgroundSeparation.h>
#include <sequence/PathStream.h>
#include <PathAnalysisAlgorithms/StreamAnalyzer.h>
#include <tbb/concurrent_vector.h>


namespace  clustering {
	class IPathDetector;
}

class AquisitionFifo;
class AnalysisFifo
{
public:
	typedef std::pair<sequence::Video::Frame, std::vector<sequence::Cluster>> FrameT;
	tbb::concurrent_vector<FrameT> frames;
};


class AnalysisPipe
{
public:
    AnalysisPipe(const std::shared_ptr<AquisitionFifo>& fifo, const std::shared_ptr<AnalysisFifo>& analysisFifo);
	virtual ~AnalysisPipe() {}

public:
	bool start();
	void stop();
	bool isRunning() const;
	void visualize();
private:
    std::shared_ptr<AnalysisFifo> m_analysisFifo;
    std::shared_ptr<AquisitionFifo> m_aquisitionFifo;
	std::atomic<bool> m_AnalysisRunning;
	boost::thread m_AnalysisThread;

	sequence::FBSeparator m_fbSeparator;
	std::shared_ptr<clustering::IPathDetector> m_PathDetector;
	sequence::PathStream m_PathStream;
	clustering::StreamAnalyzer m_StreamPathAnalysis;

	bool m_Visualize;
	std::mutex m_VisualizeMutex;
	cv::Mat m_VisualizationFrame;
	utils::ZoomObjectCollection m_ZoomObjects;
    std::shared_ptr<AquisitionFifo> m_fifo;

private:
    void analysisThreadFunc();
};

#endif // Aquisition_h__
