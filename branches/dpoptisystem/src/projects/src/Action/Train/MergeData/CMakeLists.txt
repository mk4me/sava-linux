BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
#include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/mbh")

SET_PRIVATE_HEADERS(
        ../../../Common/src/dputils.h
        ../../../Common/src/dplog.h
)

SET_SOURCE_FILES(
        merge.cpp
        #../../../Common/src/mbh/descFeature.cpp
        #../../../Common/src/mbh/integralVideo.cpp
        #../../../Common/src/mbh/stDescriptor.cc
        ../../../Common/src/dputils.cpp
        ../../../Common/src/dplog.cpp
)

END_PROJECT()
