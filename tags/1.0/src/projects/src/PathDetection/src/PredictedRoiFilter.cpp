#include "PredictedRoiFilter.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace clustering
{
	PredictedRoiFilter::PredictedRoiFilter() : windowSize(5), threshold(127), time(0)
	{

	}

	PredictedRoiFilter::~PredictedRoiFilter()
	{

	}

	void PredictedRoiFilter::createGui()
	{
		cv::createTrackbar("Pred ROI window size", "", &windowSize, 9);
		cv::createTrackbar("Pred ROI threshold", "", &threshold, 255);
	}

	void VariancePredictedRoiFilter::update(const cv::Mat& frame)
	{
		TimeMesurer t(time);
		cv::cvtColor(frame, currentFrame, CV_BGR2GRAY);
	}

	bool VariancePredictedRoiFilter::check(const cv::Point& point)
	{
		TimeMesurer t(time);
		boost::timer::cpu_timer timer;
		int margin = (windowSize - 1) / 2;
		cv::Rect r(point.x - margin, point.y - margin, windowSize, windowSize);
		if (r.x < 0) { r.width += r.x; r.x = 0; }
		if (r.y < 0) { r.height += r.y; r.y = 0; }
		if (r.x + r.width > currentFrame.cols) r.width = currentFrame.cols - r.x;
		if (r.y + r.height > currentFrame.rows) r.height = currentFrame.rows - r.y;

		cv::Mat roi(currentFrame, r);
		cv::Scalar m = cv::mean(roi);
		cv::Mat diff;
		cv::absdiff(roi, m, diff);
		cv::Mat output;
		cv::multiply(diff, diff, output, 1, CV_16U);
		cv::Scalar val = cv::mean(output);
		time += timer.elapsed().wall;
		return val[0] < threshold;
	}

	GradientMagnitudePredictedRoiFilter::GradientMagnitudePredictedRoiFilter() : useSobel(true)
	{

	}

	void GradientMagnitudePredictedRoiFilter::update(const cv::Mat& frame)
	{
		TimeMesurer t(time);
		cv::Mat srcGray;
		cvtColor(frame, srcGray, CV_BGR2GRAY);

		cv::Mat gradX, gradY;
		cv::Mat absGradX, absGradY;

		if (useSobel)
		{
			cv::Sobel(srcGray, gradX, CV_16S, 1, 0, windowSize);
			cv::Sobel(srcGray, gradY, CV_16S, 0, 1, windowSize);
		}
		else
		{
			cv::Scharr(srcGray, gradX, CV_16S, 1, 0);
			cv::Scharr(srcGray, gradY, CV_16S, 0, 1);
		}

		cv::convertScaleAbs(gradX, absGradX);
		cv::convertScaleAbs(gradY, absGradY);

		cv::addWeighted(absGradX, 0.5, absGradY, 0.5, 0, gradient);

		//cv::imshow("gradient", gradient);
		//cv::imwrite("F:\\demonstrator_data\\gradient\\gradient.png", gradient);
	}

	bool GradientMagnitudePredictedRoiFilter::check(const cv::Point& point)
	{
		TimeMesurer t(time);
		if (point.x < 0 || point.y < 0)
			return false;
		return gradient.at<unsigned char>(point) < threshold;
	}

	void GradientMagnitudePredictedRoiFilter::createGui()
	{
		PredictedRoiFilter::createGui();
		cv::createButton("Use Sobel", &GradientMagnitudePredictedRoiFilter::useSobelChange, this, CV_CHECKBOX, useSobel);
	}

	void GradientMagnitudePredictedRoiFilter::useSobelChange(int state, void* data)
	{
		static_cast<GradientMagnitudePredictedRoiFilter*>(data)->useSobel = state != 0;
	}
}