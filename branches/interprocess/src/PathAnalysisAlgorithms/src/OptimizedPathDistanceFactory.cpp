#include "motionanalysis/algorithms/OptimizedPathDistanceFactory.h"
#include "motionanalysis/algorithms/OptimizedPathDistance.h"

using namespace motion_analysis;

std::shared_ptr<PathDistance> motion_analysis::OptimizedPathDistanceFactory::createImpl(const motion_analysis::PathCluster& cluster, const motion_analysis::Path& path, const motion_analysis::PathClusteringParams& params)
{
	return std::make_shared<OptimizedPathDistance>(cluster, path, params);
}

std::shared_ptr<PathDistance> motion_analysis::OptimizedPathDistanceFactory::createImpl(const motion_analysis::PathCluster& cluster1, const motion_analysis::PathCluster& cluster2, const motion_analysis::PathClusteringParams& params)
{
	return std::make_shared<OptimizedPathDistance>(cluster1, cluster2, params);
}

