#pragma once
#ifndef cvgui_Window_h__
#define cvgui_Window_h__

#include "cvgui/Widget.h"

#include <opencv2/highgui.hpp>

#include <string>

namespace cvgui
{
	class Window : public Widget
	{
	public:
		Window(const std::string& name)
			: m_Name(name)
		{
			cv::namedWindow(m_Name);
		}

		~Window()
		{
			cv::destroyWindow(m_Name);
		}

		void setProperty(int id, double value)
		{
			cv::setWindowProperty(m_Name, id, value);
		}

		void show(cv::Mat& image) { cv::imshow(m_Name, image); }

		const std::string& getName() const { return m_Name; }

	private:
		const std::string m_Name;
	};
}

#endif // cvgui_Window_h__
