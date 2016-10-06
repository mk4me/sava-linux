#include "Detector.h"
#include "path.h"

#include <sequence/CompressedVideo.h>
#include <sequence/PathStream.h>

#include <config/PathDetection.h>

#include <opencv2/xfeatures2d.hpp>

#include <boost/timer/timer.hpp>

namespace clustering
{
	Detector::Detector(bool visualize) 
		: m_Visualize(visualize)
	{
	}

	Detector::~Detector()
	{
	}

	void Detector::registerParameters(ProgramOptions& programOptions)
	{
		registerGeneralParameters(programOptions);
		registerDetectorParameters(programOptions);
	}

	bool Detector::loadParameters(const ProgramOptions& options)
	{
		config::PathDetection config;
		if (!loadGeneralParameters(options, config))
			return false;

		if (!loadDetectorParameters(options, config))
			return false;

		return true;
	}

	void Detector::registerGeneralParameters(ProgramOptions& programOptions)
	{
		programOptions.add<int>("maxCostThresh", "  maximum cost allowed for matching\n   SIFT descriptors");
		programOptions.add<int>("maxPathLength", "  maximum path length");
		programOptions.add<int>("maxMissedFramesInPath", "  maximum number of consecutive missed\n   frames in path");

		programOptions.add<int>("descCostModifier", "  weight for components for computing\n   the final cost matrix value");
		programOptions.add<int>("distanceModifier", "  weight for components for computing\n   the final cost matrix value");
		programOptions.add<int>("angleModifier", "  weight for components for computing\n   the final cost matrix value");

		programOptions.add<int>("inPlaceMode", "  processing one frame over and over");
		programOptions.add<int>("useROIForDetection", "  if = 1, the keypoints will be\n   detected only in per-frame and/or user-drawn\n   regions of image/frame");

		programOptions.add<int>("userDefinedROI.x", "  ROI dimensions");
		programOptions.add<int>("userDefinedROI.y", "  ROI dimensions");
		programOptions.add<int>("userDefinedROI.width", "  ROI dimensions");
		programOptions.add<int>("userDefinedROI.height", "  ROI dimensions");
	}

	bool Detector::loadGeneralParameters(const ProgramOptions& options, const config::PathDetection& config)
	{
		if (!options.get<int>("maxCostThresh", m_MaxCostThresh))
			m_MaxCostThresh = config.getMaxCostThresh();
		if (!options.get<int>("maxPathLength", m_MaxPathLength))
			m_MaxPathLength = config.getMaxPathLength();
		if (!options.get<int>("maxMissedFramesInPath", m_MaxMissedFramesInPath))
			m_MaxMissedFramesInPath = config.getMaxMissedFramesInPath();
		if (!options.get<float>("currMaxDistFromPredictedNextWorldPoint", m_CurrMaxDistFromPredictedNextWorldPoint))
			m_CurrMaxDistFromPredictedNextWorldPoint = config.getCurrMaxDistFromPredictedNextWorldPoint();
		if (!options.get<int>("descCostModifier", m_DescCostModifier))
			m_DescCostModifier = config.getDescCostModifier();
		if (!options.get<int>("distanceModifier", m_DistanceModifier))
			m_DistanceModifier = config.getDistanceModifier();
		if (!options.get<int>("angleModifier", m_AngleModifier))
			m_AngleModifier = config.getAngleModifier();
		
		m_InPlaceMode = getParameter<int>(options, "inPlaceMode", 0);
		m_UseROIForDetection = getParameter<int>(options, "useROIForDetection", 1);

		m_UserDefinedROI.x = getParameter<int>(options, "userDefinedROI.x", -1);
		m_UserDefinedROI.y = getParameter<int>(options, "userDefinedROI.y", -1);
		m_UserDefinedROI.width = getParameter<int>(options, "userDefinedROI.width", -1);
		m_UserDefinedROI.height = getParameter<int>(options, "userDefinedROI.height", -1);

		return true;
	}

	void Detector::registerDetectorParameters(ProgramOptions& programOptions)
	{
		programOptions.add<int>("detectorID", "  detector algorithms\n   0 - SIFT\n   1 - SURF");
		programOptions.add<int>("descriptorID", "  descriptor algorithms\n   0 - SIFT\n   1 - SURF");

		programOptions.add<int>("SIFTnfeatures", "  def: 0");
		programOptions.add<int>("SIFTnOctaveLayers", "  def: 3");
		programOptions.add<double>("SIFTThreshold", "  def: 0.4");
		programOptions.add<double>("SIFTEdgeThreshold", "  def: 10.0");
		programOptions.add<double>("SIFTSigma", "  def 1.6");

		programOptions.add<int>("SURFnOctaves", "  def: 3");
		programOptions.add<int>("SURFnOctaveLayers", "  def: 4");
		programOptions.add<int>("SURFThreshold", "  def: 400");
	}

	bool Detector::loadDetectorParameters(const ProgramOptions& options, const config::PathDetection& config)
	{
		int detectorID;
		if (!options.get<int>("detectorID", detectorID))
			detectorID = config.getDetectorId();
		m_DescriptorID = detectorID;
		
		// SIFT
		int SIFTnfreatures;
		int SIFTnOctaveLayers;
		double SIFTThreshold;
		double SIFTEdgeThreshold;
		double SIFTSigma;

		if (!options.get("SIFTnfeatures", SIFTnfreatures))
			SIFTnfreatures = config.getSIFTnfeatures();
		if (!options.get("SIFTnOctaveLayers", SIFTnOctaveLayers))
			SIFTnOctaveLayers = config.getSIFTnOctaveLayers();
		if (!options.get("SIFTThreshold", SIFTThreshold))
			SIFTThreshold = config.getSIFTContrastThreshold();
		if (!options.get("SIFTEdgeThreshold", SIFTEdgeThreshold))
			SIFTEdgeThreshold = config.getSIFTEdgeThreshold();
		if (!options.get("SIFTSigma", SIFTSigma))
			SIFTSigma = config.getSIFTSigma();

		// SURF
		int SURFnOctaves;
		int SURFnOctaveLayers;
		int SURFThreshold;

		if (!options.get("SURFnOctaves", SURFnOctaves))
			SURFnOctaves = config.getSURFnOctaves();
		if (!options.get("SURFnOctaveLayers", SURFnOctaveLayers))
			SURFnOctaveLayers = config.getSURFnOctaveLayers();
		if (!options.get("SURFThreshold", SURFThreshold))
			SURFThreshold = config.getSURFHessianThreshold();
		
		if (detectorID == 0)
			m_Detector = cv::xfeatures2d::SIFT::create(SIFTnfreatures, SIFTnOctaveLayers, SIFTThreshold, SIFTEdgeThreshold, SIFTSigma);
		else if (detectorID == 1)
			m_Detector = cv::xfeatures2d::SURF::create(SURFThreshold, SURFnOctaves, SURFnOctaveLayers, true, false);

		if (m_DescriptorID == 0)
			m_Descriptor = cv::xfeatures2d::SIFT::create(SIFTnfreatures, SIFTnOctaveLayers, SIFTThreshold, SIFTEdgeThreshold, SIFTSigma);
		else if (m_DescriptorID == 1)
			m_Descriptor = cv::xfeatures2d::SURF::create(SURFThreshold, SURFnOctaves, SURFnOctaveLayers, true, false);

		if (detectorID != 0 && detectorID != 1)
		{
			std::cout << std::endl << "At least one detector has to be set!! Press any key to terminate..." << std::endl;
			return false;
		}

		if (m_DescriptorID != 0 && m_DescriptorID != 1)
		{
			std::cout << std::endl << "At least one descriptor has to be set!! Press any key to terminate..." << std::endl;
			return false;
		}

		return true;
	}

	void Detector::process(const std::string& inputSequence, const std::string& outputPaths)
	{
		std::cout << "Processing file " << inputSequence << std::endl;
		boost::timer::cpu_timer timer;

		m_Paths.clear();
		path::resetId();

		sequence::CompressedVideo seq(inputSequence);
		sequence::PathStream pathStream;

		for (size_t frameId = 0; frameId < seq.getNumFrames(); ++frameId)
		{
			m_VideoFrame = seq.getFrameImage(frameId);
			m_ROIRegions = seq.getFrameCrumbles(frameId);

			for (auto& p : m_Paths)
				p->_pushedPoint = 0;

			for (const cv::Rect& roi : m_ROIRegions)
			{
				initPerROIStructures(roi);
				createCostMatrix();
				match();
				appendPointsToPaths();
				cleanPerROIStructures();
			}

			savePaths(pathStream, frameId);
			if (m_Visualize)
				visualize();
		}

		pathStream.save(outputPaths);

		timer.stop();
		auto elapsed = timer.elapsed().wall / 1000000;
		std::cout << "done in " << elapsed << " ms" << std::endl;
	}

	void Detector::show()
	{
		std::lock_guard<std::mutex> lock(m_VisualizeMutex);
		utils::zoomShow("video", m_VisualizationFrame, m_ZoomObjects);
		cv::waitKey(1);
	}

	void Detector::initPerROIStructures(const cv::Rect& roi)
	{
		m_TmpKeypoints.clear();

		cv::Mat crumble(m_VideoFrame, roi);

		//cv::namedWindow("crumble", CV_WINDOW_AUTOSIZE);
		//cv::imshow("crumble", m_VideoFrame);
		//cv::waitKey(1000);

		// Detect keypoints in current ROI
		m_Detector->detect(crumble, m_TmpKeypoints);
		m_Descriptor->compute(crumble, m_TmpKeypoints, m_TmpDescs);

		for (int i = 0; i < m_TmpKeypoints.size(); i++)
		{
			// Keypoints from each ROI have their coordinates relative to the ROI's upper-left corner
			// so we need to unify all Keypoints so they have their coordinates relative to the frame's upper-left corner
			m_TmpKeypoints[i].pt = cv::Point2f(m_TmpKeypoints[i].pt.x + roi.x, m_TmpKeypoints[i].pt.y + roi.y);
		}

		// Compute world points from keypoints
		m_TmpWorldPoints.resize(m_TmpKeypoints.size());
		for (int i = 0; i < m_TmpKeypoints.size(); i++)
			m_TmpWorldPoints.at(i) = m_TmpKeypoints.at(i).pt;

		m_TmpPaths.clear();

		const float tolRad = 10.0;
		cv::Rect tolRoi = cv::Rect(roi.x - tolRad, roi.y - tolRad, roi.width + tolRad * 2, roi.height + tolRad * 2);

		for (auto it = m_Paths.begin(); it != m_Paths.end(); ++it)
		{
			cv::Point2f tmpPt = (*it)->getPredictedNextPoint();
			if (tolRoi.contains(tmpPt))
				m_TmpPaths.push_back(*it);
		}
	}

	void Detector::createCostMatrix()
	{
		m_CostMatrix = (float**)calloc(m_TmpDescs.rows, sizeof(float*));

		for (int i = 0; i < m_TmpDescs.rows; i++)
		{
			m_CostMatrix[i] = (float*)calloc(m_TmpPaths.size(), sizeof(float));

			for (unsigned int j = 0; j < m_TmpPaths.size(); j++)
			{
				if (!m_TmpPaths.at(j)->validateWithPredictedNextWorldPoint(m_TmpWorldPoints.at(i), m_CurrMaxDistFromPredictedNextWorldPoint))
				{
					m_CostMatrix[i][j] = 999999999999.9f;
					continue;
				}

				float sum = 0;
				cv::KeyPoint matchingPoint = m_TmpPaths.at(j)->getMatchingPoint();
				const float* matchingDescriptor = m_TmpPaths.at(j)->getMatchingDescriptor();
				cv::Mat pointDesc = cv::Mat(m_TmpDescs, cv::Range(i, i + 1), cv::Range(0, m_TmpDescs.cols));
				std::vector<float> matchingDescriptorData(matchingDescriptor, matchingDescriptor + m_TmpDescs.cols);
				//cv::InputArray ia(matchingDescriptor, m_TmpDescs.cols);
				sum = cv::norm(pointDesc, matchingDescriptorData, cv::NORM_L2)*m_DescCostModifier;

				// apply descriptor value modifier
				//sum = sqrtf(sum)*m_DescCostModifier;

				// in case of SURF descriptor, the desc. distance should be multiplied by 100 to be similar to SIFT
				if (m_DescriptorID == 1)
					sum *= 100;

				// apply distance modifier
				sum += path::L2Dist(m_TmpPaths.at(j)->getPredictedNextWorldPoint(), m_TmpWorldPoints.at(i)) * m_DistanceModifier;

				// apply angle modifier
				float angleDiff = abs(matchingPoint.angle - m_TmpKeypoints.at(i).angle);

				if (angleDiff > 180)
					angleDiff = 360 - angleDiff;

				sum += angleDiff*m_AngleModifier;

				// Since we will be filtering the matrix's elements with some threshold
				// we need to ensure that the range of those elements is not too wide
				// That is why we "normalize" the values by dividing them by the sum
				// of all the modifiers
				m_CostMatrix[i][j] = sum / (m_DescCostModifier + m_DistanceModifier + m_AngleModifier);
			}
		}
	}

	void Detector::match()
	{
		// initialize the hungarian_problem using the cost matrix
		hungarian_init(&m_HungarianProblem, m_CostMatrix, m_TmpDescs.rows, static_cast<int>(m_TmpPaths.size()), HUNGARIAN_MODE_MINIMIZE_COST);

		// solve the assignment problem
		hungarian_solve(&m_HungarianProblem);
	}

	void Detector::appendPointsToPaths()
	{
		// The number of old paths
		size_t oldPathsNum = m_TmpPaths.size();

		// If matched, append points to appropriate paths
		// otherwise create new paths from them
		for (unsigned int i = 0; i < m_TmpKeypoints.size(); i++)
		{
			int matchedPt = 0;
			size_t currPath = 0;

			while (currPath < oldPathsNum)
			{
				if (m_HungarianProblem.assignment[i][currPath] == 1)
				{
					if (m_CostMatrix[i][currPath] < m_MaxCostThresh && m_TmpPaths.at(currPath)->validateWithPredictedNextWorldPoint(m_TmpWorldPoints.at(i), m_CurrMaxDistFromPredictedNextWorldPoint))
					{
						matchedPt = 1;
						m_TmpPaths.at(currPath)->pushPoint(m_TmpKeypoints.at(i), (float*)&(m_TmpDescs.data[m_TmpDescs.step*i]), m_TmpWorldPoints.at(i));
					}

					break;
				}

				currPath++;
			}

			if (!matchedPt)
			{
				path* newPath = new path(m_TmpKeypoints.at(i), m_MaxPathLength, m_MaxMissedFramesInPath, (float*)&(m_TmpDescs.data[m_TmpDescs.step*i]), m_TmpDescs.cols, m_TmpWorldPoints.at(i));
				m_Paths.push_back(newPath);
			}
		}
	}

	void Detector::cleanPerROIStructures()
	{
		hungarian_free(&m_HungarianProblem);

		for (int i = 0; i < m_TmpDescs.rows; i++)
			free(m_CostMatrix[i]);
		free(m_CostMatrix);

		m_TmpDescs.release();
	}

	void Detector::savePaths(sequence::PathStream& pathStream, size_t frameId)
	{
		pathStream.addFrame();

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

	void Detector::visualize()
	{
		utils::ZoomObjectCollection zoomObjects;

		//cvtColor(m_VideoFrame, m_VideoFrameGray, CV_BGR2GRAY);
		//cvtColor(m_VideoFrameGray, m_VideoFrameCompare, CV_GRAY2BGR);

		// Show paths "normally" if it's not inplace mode
		if (m_InPlaceMode == 0)
		{
			for (unsigned int i = 0; i < m_Paths.size(); i++)
				m_Paths.at(i)->draw(zoomObjects, 1.0f, path::MODE_LINE_POINT);
		}

		if (m_UseROIForDetection && m_UserDefinedROI.x > 0)
		{
			// Mark user-defined ROI as a red rectangle
			utils::zoomRectangle(zoomObjects, m_UserDefinedROI, cv::Scalar(0, 0, 255, 255));
		}

		if (m_UseROIForDetection)
		{
			// Mark per-frame ROIs as a green rectangles
			for (int currROI = 0; currROI < m_ROIRegions.size(); currROI++)
			{
				cv::Rect r = m_ROIRegions[currROI];
				utils::zoomRectangle(zoomObjects, r, cv::Scalar(0, 255, 0, 255));
			}
		}

		std::lock_guard<std::mutex> lock(m_VisualizeMutex);
		m_VideoFrame.copyTo(m_VisualizationFrame);
		m_ZoomObjects = zoomObjects;
	}
}