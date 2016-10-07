#pragma once
#ifndef RandomColor_h__
#define RandomColor_h__

#include <opencv2/core.hpp>

#include <vector>
#include <random>

namespace utils
{
	class RandomColor
	{
	public:
		RandomColor();

		operator cv::Scalar() const { return m_Color; }
	private:
		static std::vector<cv::Scalar> ms_Palette;
		static std::mt19937 ms_ColorPickingEngine;
		static cv::Scalar nextColor();

		cv::Scalar m_Color;
	};
}

#endif // RandomColor_h__