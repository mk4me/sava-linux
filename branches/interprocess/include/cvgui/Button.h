#pragma once
#ifndef cvgui_Button_h__
#define cvgui_Button_h__

#include "cvgui/Widget.h"

#include <opencv2/highgui.hpp>

#include <boost/signals2.hpp>

namespace cvgui
{
	class Button : public Widget
	{
		typedef boost::signals2::signal<void()> OnClick;
	public:
		typedef OnClick::slot_type OnClickSlotType;

		Button(const std::string& name);

		boost::signals2::connection doOnClick(const OnClickSlotType& slot);

	private:
		std::string m_Name;

		OnClick m_OnClick;

		static void onChange(int state, void* data);
	};
}

#endif // cvgui_Button_h__
