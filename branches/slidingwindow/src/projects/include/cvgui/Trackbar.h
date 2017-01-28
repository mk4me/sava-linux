#pragma once
#ifndef cvgui_Trackbar_h__
#define cvgui_Trackbar_h__

#include "cvgui/Widget.h"

#include <boost/signals2.hpp>

namespace cvgui
{
	class Trackbar : public Widget
	{
		typedef boost::signals2::signal<void(int)> OnPositionChange;
		
	public:
		typedef OnPositionChange::slot_type OnPositionChangeSlotType;

		Trackbar(const std::string& name, const std::string& winName, int value, int maxPos);

		void setPos(int pos);
		void setMax(int maxPos);

		boost::signals2::connection doOnPositionChange(const OnPositionChangeSlotType& slot);

	private:
		int m_Value;
		std::string m_Name;
		std::string m_WinName;

		OnPositionChange m_OnPositionChange;

		static void onPositionChange(int pos, void* data);
	};
}

#endif // cvgui_Trackbar_h__
