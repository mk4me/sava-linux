#include "PathDetectionLib/OpticalFlowPathDetector.h"

#include <utils/GpuUtils.h>

namespace clustering
{
	void OpticalFlowPathDetector::initialize()
	{
		m_Time = 0;

		utils::GpuUtils::forceGpuInitialization();
	}

	void OpticalFlowPathDetector::processFrame(const cv::Mat& frame)
	{
		m_LastCrumbles.clear();
		m_LastCrumbles.swap(getCrumbles());

		m_VideoFrame = frame;

		cv::cuda::GpuMat gpuFrame;
		cv::cuda::GpuMat gpuFrameGray;

		cv::cuda::GpuMat gpuMask;

		cv::cuda::GpuMat gpuPrevPts;
		cv::cuda::GpuMat gpuNextPts;
		cv::cuda::GpuMat gpuStatus;

		gpuFrame.upload(m_VideoFrame);
		cv::cuda::cvtColor(gpuFrame, gpuFrameGray, CV_BGR2GRAY);

		if (!m_Detector)
		{
			m_Detector = cv::cuda::createGoodFeaturesToTrackDetector(gpuFrameGray.type(), (int)getConfig().getMaxPathsCount(), 0.02, 2.0);
			m_OptFlow = cv::cuda::SparsePyrLKOpticalFlow::create(cv::Size(21, 21), 6, 30);
		}

		std::list<std::shared_ptr<Path>> invalidPaths;
		std::list<std::shared_ptr<Path>> finishedPaths;
		
		std::vector<cv::Point2f> nextPtsNew;
		std::vector<cv::Point2f> nextPts;

		auto temp = nextPtsNew.size();
		if (!m_LastGpuFrameGray.empty())
		{
			std::vector<cv::Point2f> prevPtsPaths;

			for (auto pIt = m_Paths.begin(); pIt != m_Paths.end();)
			{
				auto path = *pIt;
				if (path->getEndTime() <= m_Time - 1 || path->isEnded(10))
				{
					finishedPaths.push_back(path);
					pIt = m_Paths.erase(pIt);
				}
				else if (!path->isValid())
				{
					invalidPaths.push_back(path);
					pIt = m_Paths.erase(pIt);
				}
				else
				{
					prevPtsPaths.push_back(path->getPoint(m_Time - 1));
					++pIt;
				}
			}

			if (!prevPtsPaths.empty())
			{
				utils::GpuUtils::upload(gpuPrevPts, prevPtsPaths);

				m_OptFlow->calc(m_LastGpuFrameGray, gpuFrameGray, gpuPrevPts, gpuNextPts, gpuStatus);
				
				std::vector<uchar> status;
				utils::GpuUtils::download(gpuStatus, status);
				utils::GpuUtils::download(gpuNextPts, nextPts);

				{
					assert(status.size() == m_Paths.size());
					auto pIt = m_Paths.begin();
					for (size_t i = 0; i < status.size(); ++i, ++pIt)
					{
						auto& path = *pIt;
						if (status[i])
						{
							path->addPoint(nextPts[i]);
						}
					}
				}
			}
		}
		temp = nextPtsNew.size();
		if (m_Time % m_DetectionInterval == 0)
		{
			if (m_Detector)
			{
				cv::Mat mask = cv::Mat::zeros(gpuFrameGray.rows, gpuFrameGray.cols, CV_8UC1);
				cv::Mat cameraMask = getCameraMask();
				if (!m_LastCrumbles.empty())
				{
					for (auto& r : m_LastCrumbles)
					{
						cv::rectangle(mask, r, cv::Scalar(255), -1);
					}
					if (!cameraMask.empty())
						cv::min(mask, cameraMask, mask);
				}
				else
				{
					if (!cameraMask.empty())
						mask = cameraMask;
					else
						mask.setTo(cv::Scalar(255));
				}

				gpuMask.upload(mask);
				m_Detector->detect(gpuFrameGray, gpuPrevPts, gpuMask);
				utils::GpuUtils::download(gpuPrevPts, nextPtsNew);

				/*const size_t maxPathCount2 = getConfig().getMaxPathsCount();
				if (maxPathCount2 > 0 && nextPtsNew.size() > maxPathCount2)
				{
					nextPtsNew.resize(maxPathCount2);
				}*/
			}
			else
			{
				const int step = 10;
				for (int x = 0; x < 1920; x += step)
				{
					for (int y = 0; y < 1080; y += step)
					{
						nextPtsNew.push_back(cv::Point2f(x, y));
					}
				}
				std::cout << "wesz..." << std::endl;
			}
		}
		temp = nextPtsNew.size();
		if (!nextPts.empty())
		{
			for (auto it = nextPtsNew.begin(); it != nextPtsNew.end();)
			{
				bool found = false;
				for (auto& point : nextPts)
				{
					if (abs(point.x - it->x) < 2.0f && abs(point.y - it->y) < 2.0f)
					{
						found = true;
						break;
					}
				}
				if (found)
					it = nextPtsNew.erase(it);
				else
					++it;
			}
		}

		auto nextPtsNewSize = nextPtsNew.size();
		for (size_t i = 0; i < nextPtsNewSize; ++i)
		{
			auto path = std::make_shared<Path>(m_Time);
			path->addPoint(nextPtsNew[i]);
			m_Paths.push_back(path);
		}

		const size_t maxPathCount2 = getConfig().getMaxPathsCount();
		if (maxPathCount2 > 0 && m_Paths.size() > maxPathCount2)
		{
			std::random_shuffle(m_Paths.begin(), m_Paths.end());
			while (maxPathCount2 > 0 && m_Paths.size() > maxPathCount2)
			{
				finishedPaths.push_back(m_Paths.back());
				m_Paths.pop_back();
			}
		}		

		sequence::PathStream& pathStream = getPathStream();

		for (auto& path : m_Paths)
		{
			if (m_Time < path->getBeginTime())
				continue;
			if (m_Time < path->getEndTime())
			{
				pathStream.addPath(path->getId(), path->getPoint(m_Time));
			}
			else if (m_Time == path->getEndTime())
			{
				pathStream.addPath(path->getId(), cv::Point(-2, -2));
			}
		}
		for (auto& path : finishedPaths)
		{
			/*if (m_Time < path->getBeginTime())
				continue;
			if (m_Time < path->getEndTime())
			{
				pathStream.addPath(path->getId(), path->getPoint(m_Time));
			}
			else if (m_Time == path->getEndTime())
			{
				
			}*/
			pathStream.addPath(path->getId(), cv::Point(-2, -2));
		}
		for (auto& path : invalidPaths)
		{
			pathStream.addPath(path->getId(), cv::Point(-2, -2));
		}

		m_LastGpuFrameGray = gpuFrameGray;
		++m_Time;
	}

	void OpticalFlowPathDetector::visualize(utils::ZoomObjectCollection& zoomObjects)
	{
		for (auto& path : m_Paths)
		{
			if (path->size() > 1)
			{
				cv::Point2f p1 = path->at(0);
				for (size_t i = 1; i < path->size(); ++i)
				{
					cv::Point2f p2 = path->at(i);
					utils::zoomLine(zoomObjects, p1, p2, m_ColorMap[path->getId()]);
					p1 = p2;
				}
			}
		}
	}

	config::PathDetection& OpticalFlowPathDetector::getConfig()
	{
		return config::PathDetection::getInstance();
	}

	long OpticalFlowPathDetector::Path::m_NextId;

	OpticalFlowPathDetector::Path::Path(size_t beginTime)
		: m_Length(0)
		, m_BeginTime(beginTime)
	{
		m_Id = m_NextId++;
	}

	void OpticalFlowPathDetector::Path::addPoint(const cv::Point2f& point)
	{
		if (m_Points.empty())
		{
			m_Points.push_back(point);
		}
		else
		{
			float d = dist(m_Points.back(), point);
			if (d < MAX_DISTANCE)
			{
				m_Points.push_back(point);
				m_Length += d;
			}
		}
	}

	bool OpticalFlowPathDetector::Path::isEnded(size_t lastPoints)
	{
		if (m_Points.size() > 100)
			return true;

		if (m_Points.size() < lastPoints)
			return false;

		float len = 0;
		for (size_t p = 1; p < lastPoints; ++p)
		{
			len += dist(m_Points[m_Points.size() - p], m_Points[m_Points.size() - p - 1]);
		}
		return len < 1.0f;
	}

	bool OpticalFlowPathDetector::Path::isValid()
	{
		return m_Points.size() < 20 || m_Length > 0.3f;
	}
}