#include "PathDetectionLib/PredictedRoiPathDetector.h"

#include <config/PathDetection.h>

namespace clustering
{

	PredictedRoiPathDetector::PredictedRoiPathDetector() : m_PredictedRoiEnabled(false)
		, m_PredictedRoiRadius(5)
		, m_Distance0(10, 3)
		, m_Distance1(30, 6)
	{
		setNullRoiFilter();
	}

	void PredictedRoiPathDetector::initialize()
	{
		auto& config = config::PathDetection::getInstance();

		switch (config.getRoiFilter())
		{
		case config::PathDetection::ROI_FILTER_NULL:
			setNullRoiFilter();
			break;
		case config::PathDetection::ROI_FILTER_VARIANCE:
			setGradientMagnitudeRoiFilter(config.getRoiFilterWindow(), config.getRoiFilterThreshold(), config.isRoiFilterSobel());
			break;
		case config::PathDetection::ROI_FILTER_GRADIENT_MAGNITUDE:
			setVarianceRoiFilter(config.getRoiFilterWindow(), config.getRoiFilterThreshold());
			break;
		}

		m_PredictedRoiRadius = config.getPredictedRoiRadius();
		m_Distance0.setMinPathLength(config.getDistance0MinPathLenght());
		m_Distance0.setMinPathLength(config.getDistance0MinPathDistance());
		m_Distance1.setMinPathLength(config.getDistance1MinPathLenght());
		m_Distance1.setMinPathLength(config.getDistance1MinPathDistance());

		m_PathRemoveFilterParams.T1 = config.getPathRemoveFilterT1();
		m_PathRemoveFilterParams.T2 = config.getPathRemoveFilterT2();
		m_PathRemoveFilterParams.N = config.getPathRemoveFilterN();
	}

	void PredictedRoiPathDetector::processFrame(const cv::Mat& frame)
	{
		m_VideoFrame = frame;

		m_PredictedRoiFilter->update(frame);
		
		m_ROIRegions.clear();
		cv::Mat originalMask;
		std::vector<cv::Mat> masks;

		if (m_PredictedRoiEnabled)
		{
			cv::Mat image = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC1);
			for (const cv::Rect& r : getCrumbles())
			{
				cv::rectangle(image, r, cv::Scalar(255), -1);
			}
			originalMask = image.clone();
			for (auto& p : m_Paths)
			{
				if (p->getLength() > 10)
				{
					if (!m_PredictedRoiFilter->check(p->getWorldPoint(0)))
					{
						continue;
					}

					cv::Point point = p->getPredictedNextWorldPoint();
					cv::circle(image, point, 5, cv::Scalar(255), -1);
					//cv::circle(imageVis, point, 10, CV_RGB(255, 0, 0), 1);
				}
			}
			std::vector<std::vector<cv::Point>> contours;
			cv::findContours(image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

			for (int i = 0; i < contours.size(); ++i)
			{
				auto& contour = contours[i];
				cv::Mat m = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC1);
				cv::drawContours(m, contours, i, cv::Scalar(255), -1);
				cv::Rect bb = cv::boundingRect(contour);
				if (bb.width < 5 || bb.height < 5)
					continue;
				m_ROIRegions.push_back(bb);
				cv::Mat mask(m, bb);
				masks.push_back(mask);
			}
		}
		else
		{
			m_ROIRegions = getCrumbles();
		}

		processRegions(masks, originalMask);

		savePaths(getPathStream());
		
		frame.copyTo(m_LastFrame);
	}

	void PredictedRoiPathDetector::savePaths(sequence::PathStream& pathStream)
	{
		for (unsigned int i = 0; i < m_Paths.size(); i++)
		{
			path* path = m_Paths.at(i);
			bool removePath = false;

			// This will be the point to save in the results file for the current path
			// Use the last point
			sequence::PathStream::Point ptToSave;
			sequence::PathStream::Id pathID = path->getID();

			// check if path has already received a point for this frame
			// note that new paths will have "pushedPoint" value set to 1
			if (path->getPushedPoint() == 0)
			{
				// if not push an invalid (bad) point to this path
				cv::KeyPoint badPoint;
				badPoint.pt.x = -1;
				badPoint.pt.y = -1;

				// set the point to save also as invalid (-1,-1)
				ptToSave.x = -1;
				ptToSave.y = -1;

				// Adding a point to a path with either x or y position equal -1 means that
				// a matching point hasn't been found for that path
				int tooManyMissedFrames = path->pushPoint(badPoint, 0, cv::Point2f(-1.0f, -1.0f));

				removePath = tooManyMissedFrames == -1;
			}
			else
			{
				removePath = m_Distance0.check(path) && m_Distance1.check(path);

				if (!removePath)
				{
					const cv::Point2f& p1 = path->getWorldPoint(0);
					cv::Point2f p0(-1, -1);
					float distanceFactor = 1.0f;

					for (int i = 1; i < path->getMaxLength(); ++i, distanceFactor += 1.0f)
					{
						p0 = path->getWorldPoint(i);
						if (p0.x >= 0 && p0.y >= 0)
						{
							break;
						}
					}

					if (p0.x >= 0 && p0.y >= 0 && p1.x >= 0 && p1.y >= 0)
					{
						float dist = path::L2Dist(p0, p1);

						//std::cout << "dist = " << dist << " p0 = [" << p0.x << ";" << p0.y << "] p1 = [" << p1.x << ";" << p1.y << "]" << std::endl;
						//getchar();

						if (dist > m_PathRemoveFilterParams.T1 * distanceFactor)
						{
							const int windowSize = m_PathRemoveFilterParams.N;
							int margin = (windowSize - 1) / 2;

							if (p0.x > margin && p0.y > margin && p0.x + windowSize - margin < m_VideoFrame.cols && p0.y + windowSize - margin < m_VideoFrame.rows &&
								p1.x > margin && p1.y > margin && p1.x + windowSize - margin < m_VideoFrame.cols && p1.y + windowSize - margin < m_VideoFrame.rows)
							{
								cv::Rect r0(p0.x - margin, p0.y - margin, windowSize, windowSize);
								cv::Rect r1(p1.x - margin, p1.y - margin, windowSize, windowSize);

								cv::Mat f0(m_LastFrame, r0);
								cv::Mat f1(m_VideoFrame, r1);
								cv::Mat g0, g1;
								cv::cvtColor(f0, g0, CV_BGR2GRAY);
								cv::cvtColor(f1, g1, CV_BGR2GRAY);

								cv::Mat diff;
								cv::absdiff(g0, g1, diff);
								cv::Scalar d = cv::mean(diff);

								if (d[0] < m_PathRemoveFilterParams.T2)
								{
									removePath = true;
								}
							}
							else
							{
								removePath = true;
							}
						}
					}
				}
			}
			
			// if the path hasn't received any valid point in last (maxMissedFrames)
			if (removePath)
			{
				// set the point to save as the last point for this frame (-2,-2)
				ptToSave.x = -2;
				ptToSave.y = -2;

				// remove this path:
				// call delete to free the object
				// NOTE: delete first calls the destructor
				delete path;

				// and finally remove the pointer from the paths vector
				m_Paths.erase(m_Paths.begin() + i);

				// note that since we have erased a path from "paths" vector, we need to decrement current iterator "i"
				// since path(i+1) has now been moved to path(i)
				i--;
			}
			else
			{
				ptToSave = cv::Point((int)path->getMatchingPoint().pt.x, (int)path->getMatchingPoint().pt.y);
			}

			pathStream.addPath(pathID, ptToSave);
		}
	}

	void PredictedRoiPathDetector::setNullRoiFilter()
	{
		m_PredictedRoiFilter = std::make_shared<NullPredictedRoiFilter>();
	}

	void PredictedRoiPathDetector::setVarianceRoiFilter(int windowSize, int threshold)
	{
		auto filter = std::make_shared<VariancePredictedRoiFilter>();
		filter->windowSize = windowSize;
		filter->threshold = threshold;
		m_PredictedRoiFilter = filter;
	}

	void PredictedRoiPathDetector::setGradientMagnitudeRoiFilter(int windowSize, int threshold, bool useSobel)
	{
		auto filter = std::make_shared<GradientMagnitudePredictedRoiFilter>();
		filter->windowSize = windowSize;
		filter->threshold = threshold;
		filter->useSobel = useSobel;
		m_PredictedRoiFilter = filter;
	}

}