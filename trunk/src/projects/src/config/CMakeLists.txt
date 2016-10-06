BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	Action.h
	Aquisition.h
	Compression.h
	PathAnalysis.h
	PathDetection.h
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include/config/")
#------------------------------------------------------------------------------

SET_SOURCE_FILES(
	Action.cpp 
	Compression.cpp
	PathDetection.cpp
	Aquisition.cpp
	PathAnalysis.cpp
)



END_PROJECT()