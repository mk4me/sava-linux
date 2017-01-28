#include "cvgui/Widget.h"

namespace cvgui
{
	Widget::~Widget()
	{

	}

	void Widget::addWidget(const std::shared_ptr<Widget>& widget)
	{
		widgets.push_back(widget);
	}

}