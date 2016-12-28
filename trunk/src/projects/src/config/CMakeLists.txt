BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
		Action.h
		Aquisition.h
		Camera.h
		Compression.h
		Directory.h
		Glossary.h
		Milestone.h
		Monitor.h
		Network.h
		PartitionAnalyzer.h
		PathAnalysis.h
		PathDetection.h
		Process.h
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include/config/")
#------------------------------------------------------------------------------

SET_SOURCE_FILES(
		Action.cpp
		Aquisition.cpp
		Camera.cpp
		Compression.cpp
		Directory.cpp
		Glossary.cpp
		Milestone.cpp
		Monitor.cpp
		Network.cpp
		PartitionAnalyzer.cpp
		PathAnalysis.cpp
		PathDetection.cpp
		Process.cpp
		Diagnostic.cpp
)



END_PROJECT()