#pragma once
#ifndef DefaultPathAnalyzer_h__
#define DefaultPathAnalyzer_h__

#include "PathAnalyzer.h"

#include <boost/timer/timer.hpp>

namespace motion_analysis
{
	class PathClustering;
}

namespace clustering
{
	class IVisualizer;
	class ConfigurationWindow;

	class DefaultPathAnalyzer : public PathAnalyzer
	{
	public:
		DefaultPathAnalyzer();
		~DefaultPathAnalyzer();

		//static void registerParameters(ProgramOptions& programOptions);
		virtual bool loadParameters(const ProgramOptions& options) override;

		virtual void process() override;

		virtual void show() override;

	private:
		std::shared_ptr<motion_analysis::PathClustering> m_PathClustering;

		long long m_StartTime;
		unsigned long long m_LastMaxPathId;

		int m_MaxClusterWidth;
		int m_MaxClusterHeight;

		int m_MinPathsInCluster;

		std::shared_ptr<IVisualizer> m_Visualizer;

		std::shared_ptr<ConfigurationWindow> m_ConfigurationWindow;
		std::shared_ptr<PathAnalyzer> m_PathAnalyzer;

		std::string m_InputSequence;
		std::string m_InputPaths;
		std::string m_OutputClusterPattern;
		std::string m_OutputClusterExtension;

		void save();
	};
}
#endif // DefaultPathAnalyzer_h__
