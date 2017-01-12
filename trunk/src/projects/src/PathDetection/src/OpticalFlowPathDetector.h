#pragma once
#ifndef OpticalFlowPathDetector_h__
#define OpticalFlowPathDetector_h__

#include "IPathDetector.h"

#include <utils/RandomColor.h>
#include <config/PathDetection.h>

#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>


namespace clustering
{
	/// <summary>
	/// klasy implementuj¹ce wykrywanie cie¿ek poprzez wykrycie punktów charakterystycznych detektorem GoodFeaturesToTrack oraz dopasowaniem metod¹ SparsePyrLKOpticalFlow.
	/// </summary>
	/// <seealso cref="IPathDetector" />
	class OpticalFlowPathDetector : public IPathDetector
	{
	public:
		OpticalFlowPathDetector() : m_Time(0), m_DetectionInterval(15) { }

		virtual void initialize() override;
		virtual void processFrame(const cv::Mat& frame) override;
		virtual void visualize(utils::ZoomObjectCollection& zoomObjects) override;

		void setDetectionInterval(int val) { m_DetectionInterval = val; }
		int getDetectionInterval() const { return m_DetectionInterval; }

		virtual size_t getPathsCount() const override { return m_Paths.size(); }
	private:
		config::PathDetection& getConfig();

		cv::Mat m_VideoFrame;

		class Path
		{
		public:
			enum
			{
				MAX_DISTANCE = 300,
			};
			Path(size_t beginTime);

			long getId() const { return m_Id; }
			void addPoint(const cv::Point2f& point);
			float length() const { return m_Length; }

			size_t size() const { return m_Points.size(); }
			std::vector<cv::Point2f>::const_reference at(size_t pos) const { return m_Points.at(pos); }

			size_t getBeginTime() const { return m_BeginTime; }
			size_t getEndTime() const { return m_BeginTime + size(); }
			std::vector<cv::Point2f>::const_reference getPoint(size_t t) const { return m_Points.at(t - m_BeginTime); }

			bool isEnded(size_t lastPoints);
			bool isValid();

		private:
			static long m_NextId;

			long m_Id;

			size_t m_BeginTime;
			std::vector<cv::Point2f> m_Points;
			float m_Length;

			static float dist(const cv::Point2f& a, const cv::Point2f& b)
			{
				return abs(a.x - b.x) + abs(a.y - b.y);
			}
		};

		std::vector<std::shared_ptr<Path>> m_Paths;

		cv::cuda::GpuMat m_LastGpuFrameGray;
		std::vector<cv::Rect> m_LastCrumbles;

		cv::Ptr<cv::cuda::CornersDetector> m_Detector;
		cv::Ptr<cv::cuda::SparsePyrLKOpticalFlow> m_OptFlow;
		size_t m_Time;

		std::map<long, utils::RandomColor> m_ColorMap;

		int m_DetectionInterval;
	};
}

#endif // OpticalFlowPathDetector_h__