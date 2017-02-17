BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/mbh")

SET_PRIVATE_HEADERS(
FisherVector.h
)


SET_SOURCE_FILES(
FisherVector.cpp
main.cpp

../../../Common/src/Descriptor.cpp
../../../Common/src/mbh/descFeature.cpp
../../../Common/src/mbh/integralVideo.cpp
../../../Common/src/mbh/stDescriptor.cc
)

END_PROJECT()
