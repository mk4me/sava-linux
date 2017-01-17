#include "ConfigurationWindow.h"

#include <cvgui/Trackbar.h>

namespace clustering
{

	ConfigurationWindow::ConfigurationWindow(const std::string& name, const motion_analysis::ParamSet& parameters)
		: Window(name)
	{
		cv::Mat emptyImage = cv::Mat::zeros(1, 1000, CV_8UC3);
		show(emptyImage);

		for (auto w : parameters.wrappers)
		{
			auto param = w.second;

			auto handler = std::make_shared<ParamHandler>(param);
			handlers.push_back(handler);
			auto trackbar = std::make_shared<cvgui::Trackbar>(param->getName(), getName(), param->getInUnits(), param->getPrecision());
			trackbar->doOnPositionChange(boost::bind(&ParamHandler::trackbarChanged, handler, _1));
			addWidget(trackbar);
		}
	}

	void ConfigurationWindow::ParamHandler::trackbarChanged(int position)
	{
		m_Param->setInUnits(position);
		std::cout << m_Param->getName() << ": " << m_Param->getStringValue() << std::endl;
	}

	ConfigurationWindow::ParamHandler::ParamHandler(const std::shared_ptr<motion_analysis::ParamWrapperBase>& param) 
		: m_Param(param)
	{
		// empty
	}

}