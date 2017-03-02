#include <PathDetectionLib/DefaultPathDetector.h>
#include <PathDetectionLib/path.h>

#include <config/PathDetection.h>

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>

#include <omp.h>

#include <random>

namespace clustering
{
	void DefaultPathDetector::initialize()
	{
	}

	void DefaultPathDetector::processFrame(const cv::Mat& frame)
	{
		static const int maxRoiSize = 300;

		m_VideoFrame = frame;

		m_ROIRegions.clear();
		while (!getCrumbles().empty())
		{
			cv::Rect roi = getCrumbles().back();
			getCrumbles().pop_back();

			if (roi.width > maxRoiSize)
			{
				int halfWidth = roi.width / 2;
				cv::Rect a1(roi.x, roi.y, halfWidth, roi.height), a2(roi.x + halfWidth, roi.y, halfWidth, roi.height);
				getCrumbles().push_back(std::move(a1));
				getCrumbles().push_back(std::move(a2));
			}
			else if (roi.height > maxRoiSize)
			{
				int halfHeight = roi.height / 2;
				cv::Rect a1(roi.x, roi.y, roi.width, halfHeight), a2(roi.x, roi.y + halfHeight, roi.width, halfHeight);
				getCrumbles().push_back(std::move(a1));
				getCrumbles().push_back(std::move(a2));
			}
			else
			{
				m_ROIRegions.push_back(roi);
			}
		}

		static std::random_device rd;
		static std::mt19937 g(rd());

		std::shuffle(m_ROIRegions.begin(), m_ROIRegions.end(), g);

		processRegions();

		savePaths(getPathStream());
	}

	config::PathDetection& DefaultPathDetector::getConfig()
	{
		return config::PathDetection::getInstance();
	}

	void DefaultPathDetector::processRegions(const std::vector<cv::Mat>& detectorMasks /*= std::vector<cv::Mat>()*/, cv::Mat appendMask /*= cv::Mat()*/)
	{
		for (auto& p : m_Paths)
			p->_pushedPoint = 0;

		std::vector<ThreadTmpData> threadsTmpData(m_ROIRegions.size());
		/*std::vector<cv::Mat> masks;
		if (!mask.empty())
		{
			for (int i = 0; i < static_cast<int>(m_ROIRegions.size()); ++i)
			{
				cv::Mat m(mask, m_ROIRegions[i]);
				masks.push_back(m);
			}
		}*/

#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < static_cast<int>(m_ROIRegions.size()); ++i)
		{
			const cv::Rect& roi = m_ROIRegions[i];
			ThreadTmpData& tmpData = threadsTmpData[i];
			if (!detectorMasks.empty())
				initPerROIStructures(tmpData, roi, detectorMasks[i]);
			else
				initPerROIStructures(tmpData, roi);
			createCostMatrix(tmpData);
			match(tmpData);
#pragma omp critical (processAppendPointsToPaths)
			{
				appendPointsToPaths(tmpData, appendMask);
			}
			cleanPerROIStructures(tmpData);
		}
	}

	void DefaultPathDetector::savePaths(sequence::PathStream& pathStream)
	{
		for (unsigned int i = 0; i < m_Paths.size(); i++)
		{
			// This will be the point to save in the results file for the current path
			// Use the last point
			sequence::PathStream::Point ptToSave;
			sequence::PathStream::Id pathID = m_Paths.at(i)->getID();

			// check if path has already received a point for this frame
			// note that new paths will have "pushedPoint" value set to 1
			if (m_Paths.at(i)->getPushedPoint() == 0)
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
				int tooManyMissedFrames = m_Paths.at(i)->pushPoint(badPoint, 0, cv::Point2f(-1.0f, -1.0f));

				// if the path hasn't received any valid point in last (maxMissedFrames)
				if (tooManyMissedFrames == -1)
				{
					// set the point to save as the last point for this frame (-2,-2)
					ptToSave.x = -2;
					ptToSave.y = -2;

					// remove this path:
					// call delete to free the object
					// NOTE: delete first calls the destructor
					delete m_Paths.at(i);

					// and finally remove the pointer from the paths vector
					m_Paths.erase(m_Paths.begin() + i);

					// note that since we have erased a path from "paths" vector, we need to decrement current iterator "i"
					// since path(i+1) has now been moved to path(i)
					i--;
				}
			}
			else
			{
				ptToSave = cv::Point((int)m_Paths.at(i)->getMatchingPoint().pt.x, (int)m_Paths.at(i)->getMatchingPoint().pt.y);
			}

			pathStream.addPath(pathID, ptToSave);
		}
	}

	void DefaultPathDetector::initPerROIStructures(ThreadTmpData& tmpData, const cv::Rect& roi, cv::InputArray mask /*= cv::noArray()*/)
	{
		auto& tmpDescs = tmpData.m_TmpDescs;
		auto& tmpKeypoints = tmpData.m_TmpKeypoints;
		auto& tmpWorldPoints = tmpData.m_TmpWorldPoints;
		auto& tmpPaths = tmpData.m_TmpPaths;
		//auto& hungarianProblem = tmpData.m_HungarianProblem;
		//auto& costMatrix = tmpData.m_CostMatrix;

		tmpKeypoints.clear();

		cv::Mat crumble(m_VideoFrame, roi);

		//cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		//cv::morphologyEx(crumble, crumble, cv::MORPH_DILATE, element);

		cv::Ptr<cv::Feature2D> detector;
		if (getConfig().getDetectorId() == 0)
		{
			static double videoFrameArea = m_VideoFrame.cols * m_VideoFrame.rows;
			int nFeatures = getConfig().getSIFTnfeatures() > 0 ? int(double(getConfig().getSIFTnfeatures()) * double(roi.area()) / videoFrameArea) : 0;
			detector = cv::xfeatures2d::SIFT::create(nFeatures, getConfig().getSIFTnOctaveLayers(), getConfig().getSIFTContrastThreshold(), getConfig().getSIFTEdgeThreshold(), getConfig().getSIFTSigma());
		}
		else if (getConfig().getDetectorId() == 1)
			detector = cv::xfeatures2d::SURF::create(getConfig().getSURFHessianThreshold(), getConfig().getSURFnOctaves(), getConfig().getSURFnOctaveLayers(), true, false);

		// Detect keypoints in current ROI
		detector->detect(crumble, tmpKeypoints, mask);
		detector->compute(crumble, tmpKeypoints, tmpDescs);

		for (int i = 0; i < tmpKeypoints.size(); i++)
		{
			// Keypoints from each ROI have their coordinates relative to the ROI's upper-left corner
			// so we need to unify all Keypoints so they have their coordinates relative to the frame's upper-left corner
			tmpKeypoints[i].pt = cv::Point2f(tmpKeypoints[i].pt.x + roi.x, tmpKeypoints[i].pt.y + roi.y);
		}

		// Compute world points from keypoints
		tmpWorldPoints.resize(tmpKeypoints.size());
		for (int i = 0; i < tmpKeypoints.size(); i++)
			tmpWorldPoints.at(i) = tmpKeypoints.at(i).pt;

		tmpPaths.clear();

		const float tolRad = 10.0;
		cv::Rect tolRoi = cv::Rect(roi.x - tolRad, roi.y - tolRad, roi.width + tolRad * 2, roi.height + tolRad * 2);

		for (auto it = m_Paths.begin(); it != m_Paths.end(); ++it)
		{
			cv::Point2f tmpPt = (*it)->getPredictedNextPoint();
			if (tolRoi.contains(tmpPt))
				tmpPaths.push_back(*it);
		}
	}

	void DefaultPathDetector::createCostMatrix(ThreadTmpData& tmpData)
	{
		auto& tmpDescs = tmpData.m_TmpDescs;
		auto& tmpKeypoints = tmpData.m_TmpKeypoints;
		auto& tmpWorldPoints = tmpData.m_TmpWorldPoints;
		auto& tmpPaths = tmpData.m_TmpPaths;
		//auto& hungarianProblem = tmpData.m_HungarianProblem;
		auto& costMatrix = tmpData.m_CostMatrix;

		costMatrix = (float**)calloc(tmpDescs.rows, sizeof(float*));

		for (int i = 0; i < tmpDescs.rows; i++)
		{
			costMatrix[i] = (float*)calloc(tmpPaths.size(), sizeof(float));

			for (unsigned int j = 0; j < tmpPaths.size(); j++)
			{
				if (!tmpPaths.at(j)->validateWithPredictedNextWorldPoint(tmpWorldPoints.at(i), getConfig().getCurrMaxDistFromPredictedNextWorldPoint()))
				{
					costMatrix[i][j] = 999999999999.9f;
					continue;
				}

				float sum = 0;
				cv::KeyPoint matchingPoint = tmpPaths.at(j)->getMatchingPoint();
				const float* matchingDescriptor = tmpPaths.at(j)->getMatchingDescriptor();
				cv::Mat pointDesc = cv::Mat(tmpDescs, cv::Range(i, i + 1), cv::Range(0, tmpDescs.cols));
				std::vector<float> matchingDescriptorData(matchingDescriptor, matchingDescriptor + tmpDescs.cols);
				//cv::InputArray ia(matchingDescriptor, m_TmpDescs.cols);
				sum = cv::norm(pointDesc, matchingDescriptorData, cv::NORM_L2) * getConfig().getDescCostModifier();

				// apply descriptor value modifier
				//sum = sqrtf(sum)*m_DescCostModifier;

				// in case of SURF descriptor, the desc. distance should be multiplied by 100 to be similar to SIFT
				if (getConfig().getDetectorId() == 1)
					sum *= 100;

				// apply distance modifier
				sum += path::L2Dist(tmpPaths.at(j)->getPredictedNextWorldPoint(), tmpWorldPoints.at(i)) * getConfig().getDistanceModifier();

				// apply angle modifier
				float angleDiff = abs(matchingPoint.angle - tmpKeypoints.at(i).angle);

				if (angleDiff > 180)
					angleDiff = 360 - angleDiff;

				sum += angleDiff * getConfig().getAngleModifier();

				// Since we will be filtering the matrix's elements with some threshold
				// we need to ensure that the range of those elements is not too wide
				// That is why we "normalize" the values by dividing them by the sum
				// of all the modifiers
				costMatrix[i][j] = sum / (getConfig().getDescCostModifier() + getConfig().getDistanceModifier() + getConfig().getAngleModifier());
			}
		}
	}

	void DefaultPathDetector::match(ThreadTmpData& tmpData)
	{
		auto& tmpDescs = tmpData.m_TmpDescs;
		//auto& tmpKeypoints = tmpData.m_TmpKeypoints;
		//auto& tmpWorldPoints = tmpData.m_TmpWorldPoints;
		auto& tmpPaths = tmpData.m_TmpPaths;
		auto& hungarianProblem = tmpData.m_HungarianProblem;
		auto& costMatrix = tmpData.m_CostMatrix;

		// initialize the hungarian_problem using the cost matrix
		hungarian_init(&hungarianProblem, costMatrix, tmpDescs.rows, static_cast<int>(tmpPaths.size()), HUNGARIAN_MODE_MINIMIZE_COST);

		// solve the assignment problem
		hungarian_solve(&hungarianProblem);
	}

	void DefaultPathDetector::appendPointsToPaths(ThreadTmpData& tmpData, cv::Mat mask /*= cv::Mat()*/)
	{
		auto& tmpDescs = tmpData.m_TmpDescs;
		auto& tmpKeypoints = tmpData.m_TmpKeypoints;
		auto& tmpWorldPoints = tmpData.m_TmpWorldPoints;
		auto& tmpPaths = tmpData.m_TmpPaths;
		auto& hungarianProblem = tmpData.m_HungarianProblem;
		auto& costMatrix = tmpData.m_CostMatrix;

		// The number of old paths
		size_t oldPathsNum = tmpPaths.size();

		// If matched, append points to appropriate paths
		// otherwise create new paths from them
		for (unsigned int i = 0; i < tmpKeypoints.size(); i++)
		{
			int matchedPt = 0;
			size_t currPath = 0;

			while (currPath < oldPathsNum)
			{
				if (hungarianProblem.assignment[i][currPath] == 1)
				{
					if (costMatrix[i][currPath] < getConfig().getMaxCostThresh() && tmpPaths.at(currPath)->validateWithPredictedNextWorldPoint(tmpWorldPoints.at(i), getConfig().getCurrMaxDistFromPredictedNextWorldPoint()))
					{
						matchedPt = 1;
						tmpPaths.at(currPath)->pushPoint(tmpKeypoints.at(i), (float*)&(tmpDescs.data[tmpDescs.step*i]), tmpWorldPoints.at(i));
					}

					break;
				}

				currPath++;
			}

			if (!matchedPt)
			{
				if (!mask.empty() && mask.at<unsigned char>(cv::Point(tmpKeypoints.at(i).pt)) == 0)
					continue;

				path* newPath = new path(tmpKeypoints.at(i), getConfig().getMaxPathLength(), getConfig().getMaxMissedFramesInPath(), (float*)&(tmpDescs.data[tmpDescs.step*i]), tmpDescs.cols, tmpWorldPoints.at(i));
				m_Paths.push_back(newPath);
			}
		}
	}

	void DefaultPathDetector::cleanPerROIStructures(ThreadTmpData& tmpData)
	{
		auto& tmpDescs = tmpData.m_TmpDescs;
		auto& tmpKeypoints = tmpData.m_TmpKeypoints;
		auto& tmpWorldPoints = tmpData.m_TmpWorldPoints;
		auto& tmpPaths = tmpData.m_TmpPaths;
		auto& hungarianProblem = tmpData.m_HungarianProblem;
		auto& costMatrix = tmpData.m_CostMatrix;

		hungarian_free(&hungarianProblem);

		for (int i = 0; i < tmpDescs.rows; i++)
			free(costMatrix[i]);
		free(costMatrix);

		tmpDescs.release();
	}

	void DefaultPathDetector::visualize(utils::ZoomObjectCollection& zoomObjects)
	{
		for (unsigned int i = 0; i < m_Paths.size(); i++)
			m_Paths.at(i)->draw(zoomObjects, 1.0f, path::MODE_LINE_POINT);

		// Mark per-frame ROIs as a green rectangles
		for (int currROI = 0; currROI < m_ROIRegions.size(); currROI++)
		{
			cv::Rect r = m_ROIRegions[currROI];
			utils::zoomRectangle(zoomObjects, r, cv::Scalar(0, 255, 0, 255));
		}
	}
}