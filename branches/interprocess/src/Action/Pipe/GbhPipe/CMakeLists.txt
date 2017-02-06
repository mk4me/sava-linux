BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/gbh")

SET_PRIVATE_HEADERS(
FisherVector.h
)


SET_SOURCE_FILES(
FisherVector.cpp
main.cpp

../../../Common/src/Descriptor.cpp
../../../Common/src/gbh/descFeature.cpp
../../../Common/src/gbh/integralVideo.cpp
../../../Common/src/gbh/stDescriptor.cc
)

END_PROJECT()
