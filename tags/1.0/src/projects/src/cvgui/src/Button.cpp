#include "cvgui/Button.h"

namespace cvgui
{

	Button::Button(const std::string& name) : m_Name(name)
	{
		cv::createButton(m_Name, &Button::onChange, this, CV_PUSH_BUTTON);
	}

	boost::signals2::connection Button::doOnClick(const OnClickSlotType& slot)
	{
		return m_OnClick.connect(slot);
		//throw std::exception("exception");
	}

	void Button::onChange(int state, void* data)
	{
		Button* button = static_cast<Button*>(data);
		button->m_OnClick();
	}

}