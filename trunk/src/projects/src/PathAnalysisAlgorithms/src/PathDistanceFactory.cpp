#include "motionanalysis/algorithms/PathDistanceFactory.h"
#include "motionanalysis/algorithms/PathDistance.h"

using namespace motion_analysis;

std::shared_ptr<PathDistanceFactory> motion_analysis::PathDistanceFactory::factory = std::make_shared<PathDistanceFactory>();

std::shared_ptr<PathDistance> motion_analysis::PathDistanceFactory::createImpl(const motion_analysis::PathCluster& cluster, const motion_analysis::Path& path, const motion_analysis::PathClusteringParams& params)
{
	return std::make_shared<PathDistance>(cluster, path, params);
}

std::shared_ptr<PathDistance> motion_analysis::PathDistanceFactory::createImpl(const motion_analysis::PathCluster& cluster1, const motion_analysis::PathCluster& cluster2, const motion_analysis::PathClusteringParams& params)
{
	return std::make_shared<PathDistance>(cluster1, cluster2, params);
}
