#pragma once
#ifndef cvgui_Widget_h__
#define cvgui_Widget_h__

#include <memory>
#include <list>

namespace cvgui
{
	class Widget
	{
	public:
		virtual ~Widget();

		void addWidget(const std::shared_ptr<Widget>& widget);

	private:
		std::list<std::shared_ptr<Widget>> widgets;
	};
}

#endif // cvgui_Widget_h__