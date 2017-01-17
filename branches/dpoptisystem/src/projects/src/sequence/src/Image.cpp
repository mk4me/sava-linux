#include "Image.h"
#include <opencv2/imgcodecs.hpp>

namespace sequence
{
	const int Image::DEFAULT_COMPRESSION = 30;

	Image::Image()
	{
	}

	Image::Image(const std::vector<uchar>& data) : m_Data(data)
	{
	}

	Image::Image(const cv::Mat& matrix, const int compression)
	{
		cv::imencode(".jpg", matrix, m_Data, { cv::IMWRITE_JPEG_QUALITY, 100-compression });
	}

	cv::Mat Image::getMatrix() const
	{
		return cv::imdecode(m_Data, cv::IMREAD_COLOR);
	}
}