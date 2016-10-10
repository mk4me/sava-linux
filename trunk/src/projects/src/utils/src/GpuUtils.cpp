#include "GpuUtils.h"

namespace utils
{
	void GpuUtils::download(const cv::cuda::GpuMat& dMat, std::vector<cv::Point2f>& vec)
	{
		vec.resize(dMat.cols);
		if (!vec.empty())
		{
			cv::Mat mat(1, dMat.cols, CV_32FC2, (void*)&vec[0]);
			dMat.download(mat);
		}
	}

	void GpuUtils::download(const cv::cuda::GpuMat& dMat, std::vector<uchar>& vec)
	{
		vec.resize(dMat.cols);
		if (!vec.empty())
		{
			cv::Mat mat(1, dMat.cols, CV_8UC1, (void*)&vec[0]);
			dMat.download(mat);
		}
	}

	void GpuUtils::upload(cv::cuda::GpuMat& dMat, std::vector<cv::Point2f>& vec)
	{
		cv::Mat mat(1, (int)vec.size(), CV_32FC2, (void*)&vec[0]);
		dMat.upload(mat);
	}

	void GpuUtils::forceGpuInitialization()
	{
		// force CUDA initialization
		{
			cv::cuda::GpuMat dummyGpuMat;
			cv::Mat dummyMat = cv::Mat::zeros(64, 64, CV_8UC1);
			dummyGpuMat.upload(dummyMat);
		}
	}

}