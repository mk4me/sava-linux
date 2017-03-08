#include "PathAnalysisAlgorithms/StatisticPathDistanceFactory.h"

#include <PathAnalysisAlgorithms/algorithms/PathDistance.h>

namespace clustering
{
	StatisticPathDistanceFactory::Stats StatisticPathDistanceFactory::ms_PathStats;
	StatisticPathDistanceFactory::Stats StatisticPathDistanceFactory::ms_ClusterStats;

	std::shared_ptr<motion_analysis::PathDistance> StatisticPathDistanceFactory::createImpl(const motion_analysis::PathCluster& cluster, const motion_analysis::Path& path, const motion_analysis::PathClusteringParams& params)
	{
		auto distance = m_Factory->createImpl(cluster, path, params);
		if (distance->checkConditions(params.pathThresholds))
			ms_PathStats.update(distance);
		return distance;
	}

	std::shared_ptr<motion_analysis::PathDistance> StatisticPathDistanceFactory::createImpl(const motion_analysis::PathCluster& cluster1, const motion_analysis::PathCluster& cluster2, const motion_analysis::PathClusteringParams& params)
	{
		auto distance = m_Factory->createImpl(cluster1, cluster2, params);
		if (distance->checkConditions(params.clusterThresholds))
			ms_ClusterStats.update(distance);
		return distance;
	}

	void StatisticPathDistanceFactory::printAndClearStats()
	{
		std::cout << "Distances cluster to path" << std::endl;
		ms_PathStats.printAndClear();
		std::cout << "Distances cluster to cluster" << std::endl;
		ms_ClusterStats.printAndClear();
	}

	void StatisticPathDistanceFactory::Stats::update(const std::shared_ptr<motion_analysis::PathDistance>& distance)
	{
		m_TotalStat.add(distance->total);
		m_TangentialComponentStat.add(distance->tangentialComponent);
		m_SquaredDispertionXStat.add(distance->squaredDispertion.x);
		m_SquaredDispertionYStat.add(distance->squaredDispertion.y);
	}

	void StatisticPathDistanceFactory::Stats::printAndClear()
	{
		printAndClear(m_TotalStat);
		printAndClear(m_TangentialComponentStat);
		printAndClear(m_SquaredDispertionXStat);
		printAndClear(m_SquaredDispertionYStat);
		std::cout << std::endl;
	}
}