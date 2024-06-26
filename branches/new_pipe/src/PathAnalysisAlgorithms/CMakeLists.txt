BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
		./Partition/PartitionAnalyzer.h
		./Partition/PartitionCluster.h
		./Partition/PartitionDefinitions.h
		./Partition/PartitionPath.h
		./Partition/PartitionFrame.h
		./Partition/PartitionMath.h
		./Partition/PartitionPathGroup.h
		./Partition/PartitionSequence.h
		./Partition/PartitionSequenceGroup.h
		DefaultPathAnalyzer.h
		PathAnalyzer.h
		StatisticPathDistanceFactory.h
		algorithms/auxmath.h
		algorithms/aux_serialization.h
		algorithms/ClusterBase.h
		algorithms/FastPathClustering.h
		algorithms/IClustering.h
		algorithms/OptimizedPathDistance.h
		algorithms/OptimizedPathDistanceFactory.h
		algorithms/ParamSet.h
		algorithms/ParamWrapper.h
		algorithms/ParamWrapperBase.h
		algorithms/Path.h
		algorithms/PathAssignments.h
		algorithms/PathCluster.h
		algorithms/PathClustering.h
		algorithms/PathClusteringParams.h
		algorithms/PathDistance.h
		algorithms/PathDistanceFactory.h
		algorithms/PathEstimator.h
		algorithms/Point.h
		algorithms/PointCollection.h
		algorithms/PointCollectionBase.h
		algorithms/PointCollectionUtils.h
		StreamAnalyzer.h
)

SET_SOURCE_FILES(
	FastPathClustering.cpp
	OptimizedPathDistance.cpp
	OptimizedPathDistanceFactory.cpp
	Path.cpp
	PathAssignments.cpp
	PathCluster.cpp
	PathClustering.cpp
	PathClusteringParams.cpp
	PathDistance.cpp
	PathDistanceFactory.cpp

		PathEstimator.cpp
	PointCollection.cpp
		./Partition/PartitionAnalyzer.cpp
		./Partition/PartitionCluster.cpp
		./Partition/PartitionDefinitions.cpp
		./Partition/PartitionFrame.cpp
		./Partition/PartitionMath.cpp
		./Partition/PartitionPath.cpp
		./Partition/PartitionPathGroup.cpp
		./Partition/PartitionSequence.cpp
		./Partition/PartitionSequenceGroup.cpp
		DefaultPathAnalyzer.cpp
		PathAnalyzer.cpp
		StatisticPathDistanceFactory.cpp
		StreamAnalyzer.cpp
)

END_PROJECT()