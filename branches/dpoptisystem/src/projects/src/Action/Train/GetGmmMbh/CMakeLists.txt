BEGIN_PROJECT("executable")


include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/mbh")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../../config/src")

SET_PRIVATE_HEADERS(
        ../../../Common/src/dputils.h
        ../../../Common/src/dplog.h
)

SET_SOURCE_FILES(
        getClusters_fv_hmdb.cc
        ../../../Common/src/mbh/descFeature.cpp
        ../../../Common/src/mbh/integralVideo.cpp
        ../../../Common/src/mbh/stDescriptor.cc
        ../../../../config/src/Directory.cpp
        ../../../Common/src/dputils.cpp
        ../../../Common/src/dplog.cpp
)

END_PROJECT()
