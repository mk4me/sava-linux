BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/gbh")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../../config/src")

SET_PRIVATE_HEADERS(
        ../../../Common/src/dputils.h
        ../../../Common/src/dplog.h
)

SET_SOURCE_FILES(
        getGMM.cc
        ../../../Common/src/gbh/descFeature.cpp
        ../../../Common/src/gbh/integralVideo.cpp
        ../../../Common/src/gbh/stDescriptor.cc
        ../../../../config/src/Directory.cpp
        ../../../Common/src/dputils.cpp
        ../../../Common/src/dplog.cpp
)

END_PROJECT()
