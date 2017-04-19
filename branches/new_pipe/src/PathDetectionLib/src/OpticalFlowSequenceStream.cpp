/*#include "PathDetectionLib/OpticalFlowSequenceStream.h"

#include "PathDetectionLib/OpticalFlowSequenceDetector.h"

#include <sequence/CompressedVideo.h>
#include <sequence/PathStream.h>

#include <config/Camera.h>

#include <opencv2/imgproc.hpp>

#include <boost/timer/timer.hpp>
#include <opencv2/video/tracking.hpp>


//#include <opencv2/cudaobjdetect.hpp>
//#include <opencv2/cudabgsegm.hpp>

#include <random>

namespace clustering
{

	OpticalFlowSequenceDetector::OpticalFlowSequenceDetector(bool visualize)
		: m_Visualize(visualize)
	{
	}

	OpticalFlowSequenceDetector::~OpticalFlowSequenceDetector()
	{

	}

	void OpticalFlowSequenceDetector::registerParameters(ProgramOptions& programOptions)
	{
		programOptions.add<std::string>("mask", "regions to omit");
	}

	bool OpticalFlowSequenceDetector::loadParameters(const ProgramOptions& options)
	{
		config::Camera::getInstance().load();
		std::string mask;
		cv::Mat cameraMask;
		if (options.get("mask", mask))
			cameraMask = config::Camera::getInstance().getMask(mask);

		m_PathDetector.initialize();
		m_PathDetector.setDetectionInterval(17);
		m_PathDetector.setCameraMask(cameraMask);

		return true;
	}

	static void download(const cv::cuda::GpuMat& dMat, std::vector<uchar>& vec)
	{
		vec.resize(dMat.cols);
		cv::Mat mat(1, dMat.cols, CV_8UC1, (void*)&vec[0]);
		dMat.download(mat);
	}

	static void download(const cv::cuda::GpuMat& dMat, std::vector<cv::Point2f>& vec)
	{
		vec.resize(dMat.cols);
		cv::Mat mat(1, dMat.cols, CV_32FC2, (void*)&vec[0]);
		dMat.download(mat);
	}

	static void upload(cv::cuda::GpuMat& dMat, std::vector<cv::Point2f>& vec)
	{
		cv::Mat mat(1, (int)vec.size(), CV_32FC2, (void*)&vec[0]);
		dMat.upload(mat);
	}

	void OpticalFlowSequenceDetector::process(const std::string& inputSequence, const std::string& outputPaths)
	{
		std::cout << "Processing file " << inputSequence << std::endl;
		boost::timer::cpu_timer timer;

		auto seq = sequence::IVideo::create(inputSequence);

		sequence::PathStream pathStream;
		m_PathDetector.setPathStream(pathStream);

		size_t maxPathsCount = 0;

		for (size_t frameId = 0; frameId < seq->getNumFrames(); ++frameId)
		{
			pathStream.addFrame();

			m_VideoFrame = seq->getFrameImage(frameId);
			std::vector<cv::Rect> crumbles = seq->getFrameCrumbles(frameId);

			m_PathDetector.setCrumbles(crumbles);
			m_PathDetector.processFrame(m_VideoFrame);

			maxPathsCount = std::max(maxPathsCount, m_PathDetector.getPathsCount());

			if (m_Visualize)
				visualize();
		}

		pathStream.save(outputPaths);

		timer.stop();

		auto elapsed = timer.elapsed().wall / 1000000;

		std::cout << "done.\n";
		std::cout << "Process time: " << elapsed << " ms\n";
		std::cout << "Process fps: " << float(seq->getNumFrames()) * 1000.0f / float(elapsed) << "\n";
		std::cout << "Max paths count: " << maxPathsCount;

		if (seq->getNumFrames() > 0)
		{
			auto lastFrameTime = seq->getFrameTime(seq->getNumFrames() - 1);
			auto timeDiff = boost::posix_time::microsec_clock::local_time() - lastFrameTime;
			std::cout << "\nCurrent timeout: " << timeDiff;
		}
		std::cout << std::endl;
	}

	void OpticalFlowSequenceDetector::show()
	{
		std::lock_guard<std::mutex> lock(m_VisualizeMutex);
		utils::zoomShow("video", m_VisualizationFrame, m_ZoomObjects);
		cv::waitKey(1);
	}

	void OpticalFlowSequenceDetector::visualize()
	{
		utils::ZoomObjectCollection zoomObjects;
		m_PathDetector.visualize(zoomObjects);

		std::lock_guard<std::mutex> lock(m_VisualizeMutex);
		m_VideoFrame.copyTo(m_VisualizationFrame);
		m_ZoomObjects = zoomObjects;
	}
}*/