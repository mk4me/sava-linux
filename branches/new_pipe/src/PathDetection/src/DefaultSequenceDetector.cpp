#include "DefaultSequenceDetector.h"
#include "path.h"

#include <sequence/CompressedVideo.h>
#include <sequence/PathStream.h>

#include <config/PathDetection.h>

#include <opencv2/xfeatures2d.hpp>

#include <boost/timer/timer.hpp>

#include <random>

#include <omp.h>

namespace clustering
{
	DefaultSequenceDetector::DefaultSequenceDetector(bool visualize) 
		: m_Visualize(visualize)
	{
	}

	DefaultSequenceDetector::~DefaultSequenceDetector()
	{
	}

	void DefaultSequenceDetector::registerParameters(ProgramOptions& programOptions)
	{
	}

	bool DefaultSequenceDetector::loadParameters(const ProgramOptions& options)
	{
		if (config::PathDetection::getInstance().isPredictedRoiDetector())
			m_PathDetector = std::make_shared<PredictedRoiPathDetector>();
		else
			m_PathDetector = std::make_shared<DefaultPathDetector>();
		m_PathDetector->initialize();

		return true;
	}

	void DefaultSequenceDetector::process(const std::string& inputSequence, const std::string& outputPaths)
	{
		std::cout << "Processing file " << inputSequence << std::endl;
		boost::timer::cpu_timer timer;

		auto seq = sequence::IVideo::create(inputSequence);


		sequence::PathStream pathStream;
		m_PathDetector->setPathStream(pathStream);

		std::random_device rd;
		std::mt19937 g(rd());

		static const int maxRoiSize = 300;
		size_t maxPathsCount = 0;

		for (size_t frameId = 0; frameId < seq->getNumFrames(); ++frameId)
		{
			pathStream.addFrame();

			m_VideoFrame = seq->getFrameImage(frameId);
			std::vector<cv::Rect> crumbles = seq->getFrameCrumbles(frameId);
						
			m_PathDetector->setCrumbles(std::move(crumbles));
			m_PathDetector->processFrame(m_VideoFrame);
			maxPathsCount = std::max(maxPathsCount, m_PathDetector->getPathsCount());
			
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

	void DefaultSequenceDetector::show()
	{
		std::lock_guard<std::mutex> lock(m_VisualizeMutex);
		utils::zoomShow("Path detection visualization", m_VisualizationFrame, m_ZoomObjects);
		cv::waitKey(1);
	}
	
	void DefaultSequenceDetector::visualize()
	{
		utils::ZoomObjectCollection zoomObjects;		
		m_PathDetector->visualize(zoomObjects);

#if SAVE_OUTPUT_ENABLED
		{
			cv::Mat tmpImage = m_VideoFrame.clone();

			class DummyZoomTransform : public utils::IZoomTransform
			{
			public:
				virtual cv::Point globalToZoomed(const cv::Point& global) const override { return global; }
				virtual cv::Point zoomedToGlobal(const cv::Point& zoomed) const override { return zoomed; }
				virtual float getRatio() const override { return 1.0f; }
			};
			utils::IZoomTransformConstPtr zoomTransform = std::make_shared<DummyZoomTransform>();
			for (auto& zo : zoomObjects)
			{				
				zo->draw(tmpImage, zoomTransform);
			}
			static int frameNo = 0;
			std::ostringstream oss;
			oss << std::setfill('0') << std::setw(5) << frameNo++;
			cv::imwrite("F:\\2016_05_17\\path_detection\\frame_" + oss.str() + ".jpg", tmpImage);
		}
#endif // SAVE_OUTPUT_ENABLED
		std::lock_guard<std::mutex> lock(m_VisualizeMutex);
		m_VideoFrame.copyTo(m_VisualizationFrame);
		m_ZoomObjects = zoomObjects;
	}
}