BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/mbh")

SET_SOURCE_FILES(
        getData2c_FV_hmdb.cc
        ../../../Common/src/mbh/descFeature.cpp
        ../../../Common/src/mbh/integralVideo.cpp
        ../../../Common/src/mbh/stDescriptor.cc
)

END_PROJECT()
