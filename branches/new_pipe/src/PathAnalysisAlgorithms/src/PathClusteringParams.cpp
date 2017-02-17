#include "motionanalysis/algorithms/PathClusteringParams.h"

using namespace motion_analysis;
using namespace std;

motion_analysis::MergingThresholds::MergingThresholds()
	: time(10)
	, x(20.0f)
	, y(20.0f)
{
}

BOOST_CLASS_EXPORT_IMPLEMENT(motion_analysis::PathClusteringParams);

/// <summary>
/// See declaration for all the details.
/// </summary>
motion_analysis::PathClusteringParams::PathClusteringParams()
{
    addParam<int>( "minimumPathLength", minimumPathLength, 10, 2, 20 );
	addParam<float>( "alpha", alpha, 0.5, 0.0, 1.0, 100 );

    addParam<float>( "pathThresholds.total", pathThresholds.total, 100, 0, 100, 1000 );
    addParam<float>( "pathThresholds.positional", pathThresholds.positional, 3, 0, 100, 1000 );
    addParam<float>( "pathThresholds.tangent", pathThresholds.tangent, 100, 0, 100, 1000 );

    addParam<float>( "clusterThresholds.total", clusterThresholds.total, 100, 0, 100, 100 );
    addParam<float>( "clusterThresholds.positional", clusterThresholds.positional, 3, 0, 100, 1000 );
    addParam<float>( "clusterThresholds.tangent", clusterThresholds.tangent, 100, 0, 100, 1000 );

    addParam<float>( "weightX", weightX, 20, 1, 300 );
    addParam<float>( "weightY", weightY, 40, 1, 300 );
    addParam<float>( "weightTau", weightTau, 0.5f, 1, 100 );

	//addParam<float>("pathsPercentageMovesPerFrame", pathsPercentageMovesPerFrame, 0.1f, 0.0f, 1.0f, 100);
	//addParam<int>("minPathsMovesPerFrame", minPathsMovesPerFrame, 10, 1, 1000);

	//pathsPercentageMovesPerFrame = 0.05f;
	addParam<float>( "pathsPercentageMovesPerFrame", pathsPercentageMovesPerFrame, 0.5f, 0, 1, 100);

	//minPathsMovesPerFrame = 10;
	addParam("minPathsMovesPerFrame", minPathsMovesPerFrame, 10, 0, 100);
	//addParam<int>( "minPathsMovesPerFrame", minPathsMovesPerFrame, 10, 1, 500);

	//enableMerging = 10;
	addParam("enableMerging", enableMerging, 0, 0, 10);
	//addParam<int>( "enableMerging", enableMerging, 1, 0, 10);

	//createNewCluster = true;
	addParam("createNewCluster", createNewCluster, true, false, true);
	//addParam<bool>( "createNewCluster", createNewCluster, true, false, true);

	//tryMovingOnlyWhenCurrentIsToFar = true;
	addParam("tryMovingOnlyWhenCurrentIsToFar", tryMovingOnlyWhenCurrentIsToFar, true, false, true);
	//addParam<bool>( "tryMovingOnlyWhenCurrentIsToFar", tryMovingOnlyWhenCurrentIsToFar, true, false, true);

	//enableMovingPaths = 0;
	addParam<int>("enableMovingPaths", enableMovingPaths, 0, 0, 10);
	//enableClusterRemoving = 1;
	addParam<int>("enableClusterRemoving", enableClusterRemoving, 0, 0, 10);
	//enableMergingGaps = 1;
	addParam<int>("enableMergingGaps", enableMergingGaps, 1, 0, 10);

	addParam<int>("mergingThresholds.time", mergingThresholds.time, 25, 0, 1000);
	addParam<float>("mergingThresholds.x", mergingThresholds.x, 50, 0, 1000);
	addParam<float>("mergingThresholds.y", mergingThresholds.y, 50, 0, 1000);

	addParam<int>("mergingOldestClusterTime", mergingOldestClusterTime, 50, 0, 1000);

	//enableMultitrackingTarget = false;
	//enableClusterSplitting = false;

	//addParam("domPointsMin", domPointsMin, 56, 1, 100);
	//addParam("domPointsMax", domPointsMax, 64, 1, 100);
	//addParam("domPointsNeigh", domPointsNeigh, 48, 1, 100);
	//addParam("domPointsMaxAngle", domPointsMaxAngle, 160, 1, 180);

	enableHistory = false;
}
