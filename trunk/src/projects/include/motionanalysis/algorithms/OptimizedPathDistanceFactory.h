#pragma once
#include "motionanalysis/algorithms/PathDistanceFactory.h"

namespace motion_analysis
{
	class OptimizedPathDistanceFactory : public PathDistanceFactory
	{
	protected:
		virtual std::shared_ptr<PathDistance> createImpl(
			const motion_analysis::PathCluster& cluster,
			const motion_analysis::Path& path,
			const motion_analysis::PathClusteringParams& params) override;

		virtual std::shared_ptr<PathDistance> createImpl(
			const motion_analysis::PathCluster& cluster1,
			const motion_analysis::PathCluster& cluster2,
			const motion_analysis::PathClusteringParams& params) override;

	};
}
