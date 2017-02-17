#pragma once
#ifndef GpuUtils_h__
#define GpuUtils_h__

#include <opencv2/cudaarithm.hpp>

namespace utils
{
	/// <summary>
	/// klasa pomocnicza zapewniaj¹ca funkcje do pracy z OpenCV na GPU.
	/// </summary>
	class GpuUtils
	{
	public:
		static void download(const cv::cuda::GpuMat& dMat, std::vector<uchar>& vec);
		static void download(const cv::cuda::GpuMat& dMat, std::vector<cv::Point2f>& vec);
		static void upload(cv::cuda::GpuMat& dMat, std::vector<cv::Point2f>& vec);

		static void forceGpuInitialization();
	};
}

#endif // GpuUtils_h__