BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/gbh")

SET_PRIVATE_HEADERS(
	HMDBdatabase.h
	SystemDatabase.h
)

SET_SOURCE_FILES(
	HMDBdatabase.cpp
	main.cpp
	SystemDatabase.cpp
)

END_PROJECT()
