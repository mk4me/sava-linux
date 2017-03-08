#pragma once
#ifndef StreamAnalyzer_h__
#define StreamAnalyzer_h__


#include <boost/timer/timer.hpp>
#include <map>
#include <memory>
#include <utils/ProgramOptions.h>
#include <sequence/PathStream.h>
#include <PathAnalysisAlgorithms/PathAnalyzer.h>

namespace motion_analysis
{
    class PathClustering;
    class PathEstimator;
}


namespace clustering
{
	class IVisualizer;
	class ConfigurationWindow;

	/// <summary>
	/// klasa dziedzicz¹ca po klasie PathAnalyzer, udostêpniaj¹ca algorytm klasteryzacji zawarty w bibliotece PathAnalysisAlgorithms.
	/// </summary>
	/// <seealso cref="PathAnalyzer" />
	class StreamAnalyzer
	{
	public:
		StreamAnalyzer();
		virtual ~StreamAnalyzer();

		//static void registerParameters(ProgramOptions& programOptions);
		virtual bool loadParameters();//(const ProgramOptions& options);

		virtual std::vector<sequence::Cluster> processFrame(const std::map<sequence::PathStream::Id, sequence::PathStream::Path>&);

		//virtual void show();

        std::vector<sequence::Cluster> save();
        std::vector<sequence::Cluster> saveEstimated();

	private:
		std::shared_ptr<motion_analysis::PathClustering> m_PathClustering;
		std::shared_ptr<motion_analysis::PathEstimator> m_PathEstimator;

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


        long m_MinProcessTime;
        bool m_RoiEstimation;
		//std::set<long long> m_Paths;
	};
}
#endif // DefaultPathAnalyzer_h__
