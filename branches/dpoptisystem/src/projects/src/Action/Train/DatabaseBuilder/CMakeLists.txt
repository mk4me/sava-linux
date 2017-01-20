BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/gbh")

SET_PRIVATE_HEADERS(
	HMDBdatabase.h
	SystemDatabase.h
		../../../Common/src/dputils.h
		../../../Common/src/dplog.h
)

SET_SOURCE_FILES(
	HMDBdatabase.cpp
    SystemDatabase.cpp
		../../../Common/src/dputils.cpp
		../../../Common/src/dplog.cpp
	main.cpp
)

END_PROJECT()
