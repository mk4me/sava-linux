#include "cvgui/Trackbar.h"

#include <opencv2/highgui.hpp>

namespace cvgui
{
	Trackbar::Trackbar(const std::string& name, const std::string& winName, int value, int maxPos) : m_Name(name)
		, m_WinName(winName)
		, m_Value(value)
	{
		cv::createTrackbar(m_Name, m_WinName, &m_Value, maxPos, &Trackbar::onPositionChange, this);
	}

	void Trackbar::setPos(int pos)
	{
		cv::setTrackbarPos(m_Name, m_WinName, pos);
	}

	void Trackbar::setMax(int maxPos)
	{
		cv::setTrackbarMax(m_Name, m_WinName, maxPos);
	}

	boost::signals2::connection Trackbar::doOnPositionChange(const OnPositionChangeSlotType& slot)
	{
		return m_OnPositionChange.connect(slot);
	}

	void Trackbar::onPositionChange(int pos, void* data)
	{
		Trackbar* trackbar = static_cast<Trackbar*>(data);
		trackbar->m_OnPositionChange(pos);
	}

}