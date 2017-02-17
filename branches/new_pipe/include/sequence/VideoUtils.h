#pragma once
#ifndef VideoUtils_h__
#define VideoUtils_h__

#include <opencv2/core.hpp>

namespace sequence
{
	/// <summary>
	/// klasa zapewniaj¹ca pomocnicze funkcje.
	/// </summary>
	class VideoUtils
	{
	public:
		static cv::Mat getImageRoi(cv::Mat &frame, const cv::Rect &roi);
	};
}

#endif // VideoUtils_h__