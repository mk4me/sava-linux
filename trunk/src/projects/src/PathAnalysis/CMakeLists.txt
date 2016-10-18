BEGIN_PROJECT("executable")


SET_PRIVATE_HEADERS(
	Partition/PartitionAnalyzer.h
	Partition/PartitionCluster.h
	Partition/PartitionDefinitions.h
	Partition/PartitionPath.h
	Partition/PartitionFrame.h
	Partition/PartitionMath.h
	Partition/PartitionPathGroup.h
	Partition/PartitionSequence.h
	Partition/PartitionSequenceGroup.h
	ConfigurationWindow.h
	DefaultPathAnalyzer.h
	IVisualizer.h
	PathAnalysis.h
	StatisticPathDistanceFactory.h
	NullVisualizer.h
	PathAnalysisVisualizer.h
	PathAnalyzer.h

)


SET_SOURCE_FILES(
	Partition/PartitionAnalyzer.cpp
	Partition/PartitionCluster.cpp
	Partition/PartitionDefinitions.cpp
	Partition/PartitionFrame.cpp
	Partition/PartitionMath.cpp
	Partition/PartitionPath.cpp
	Partition/PartitionPathGroup.cpp
	Partition/PartitionSequence.cpp
	Partition/PartitionSequenceGroup.cpp
	ConfigurationWindow.cpp
	DefaultPathAnalyzer.cpp
	Main.cpp
	PathAnalysis.cpp
	PathAnalysisVisualizer.cpp
	PathAnalyzer.cpp
	StatisticPathDistanceFactory.cpp
)

END_PROJECT()