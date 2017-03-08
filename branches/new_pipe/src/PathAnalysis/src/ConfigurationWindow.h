#pragma once
#ifndef ConfigurationWindow_h__
#define ConfigurationWindow_h__

#include <PathAnalysisAlgorithms/algorithms/ParamSet.h>
#include <cvgui/Window.h>

namespace clustering
{

	/// <summary>
	/// klasa tworz¹ca okno do zmiany parametrów klasteryzacji podczas dzia³ania aplikacji. U¿ywana by³a na etapie tworzenia modu³u.
	/// </summary>
	/// <seealso cref="cvgui::Window" />
	class ConfigurationWindow : public cvgui::Window
	{
	public:
		ConfigurationWindow(const std::string& name, const motion_analysis::ParamSet& parameters);

	private:
		class ParamHandler
		{
		public:
			ParamHandler(const std::shared_ptr<motion_analysis::ParamWrapperBase>& param);

			void trackbarChanged(int position);

		private:
			std::shared_ptr<motion_analysis::ParamWrapperBase> m_Param;
		};

		std::vector<std::shared_ptr<ParamHandler>> handlers;
	};
}

#endif // ConfigurationWindow_h__