#pragma once
#ifndef IPathDetector_h__
#define IPathDetector_h__

#include <sequence/PathStream.h>

#include <utils/CvZoom.h>

#include <opencv2/core.hpp>

namespace clustering
{
	class IPathDetector
	{
	public:
		virtual ~IPathDetector() { }

		virtual void initialize() = 0;
		virtual void processFrame(const cv::Mat& frame) = 0;
		virtual void visualize(utils::ZoomObjectCollection& zoomObjects) = 0;

		virtual size_t getPathsCount() const = 0;

		void setPathStream(sequence::PathStream& pathStream) { m_PathStream = &pathStream; m_DefaultPathStream.clear(); }
		sequence::PathStream& getPathStream() { return *m_PathStream; }

		void setCrumbles(const std::vector<cv::Rect>& crumbles) { m_Crumbles = crumbles; }
		void setCrumbles(std::vector<cv::Rect>&& crumbles) { m_Crumbles = crumbles; }
		std::vector<cv::Rect>& getCrumbles() { return m_Crumbles; }

		void setCameraMask(const cv::Mat& mask) { m_CameraMask = mask; }
		cv::Mat getCameraMask() const { return m_CameraMask; }

	protected:
		IPathDetector() { m_PathStream = &m_DefaultPathStream; }

	private:
		sequence::PathStream m_DefaultPathStream;
		sequence::PathStream* m_PathStream;

		std::vector<cv::Rect> m_Crumbles;

		cv::Mat m_CameraMask;
	};
}

#endif // IPathDetector_h__