#include "RandomColor.h"

namespace utils
{
	std::vector<cv::Scalar> RandomColor::ms_Palette;
	std::mt19937 RandomColor::ms_ColorPickingEngine;

	RandomColor::RandomColor()
	{
		if (ms_Palette.empty())
			for (int r = 31; r < 256; r += 32)			
				for (int g = 31; g < 256; g += 32)				
					for (int b = 31; b < 256; b += 32)					
						ms_Palette.push_back(cv::Scalar(b, g, r));

		m_Color = nextColor();
	}

	cv::Scalar RandomColor::nextColor()
	{
		std::uniform_int_distribution<size_t> distribution(0, ms_Palette.size() - 1);
		return ms_Palette[distribution(ms_ColorPickingEngine)];
	}


}