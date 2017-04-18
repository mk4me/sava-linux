#include "VideoUtils.h"

namespace sequence
{
	cv::Mat VideoUtils::getImageRoi(cv::Mat &frame, const cv::Rect &roi)
	{		
		cv::Rect intersection = cv::Rect(0, 0, frame.cols, frame.rows) & roi;

		if (intersection.area() == roi.area())
			return frame(roi);

		cv::Mat newFrame = cv::Mat::zeros(roi.height, roi.width, CV_8UC3);

		cv::Rect newRoi(roi.x > 0 ? 0 : -roi.x, roi.y > 0 ? 0 : -roi.y, intersection.width, intersection.height);
		frame(intersection).copyTo(newFrame(newRoi));
		return newFrame;
	}
}