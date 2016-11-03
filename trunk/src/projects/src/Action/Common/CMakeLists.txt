BEGIN_PROJECT("static")


SET_PRIVATE_HEADERS(
biistream.h
biostream.h
bistreamable.h
chiSquareDist.h
Descriptor.h
filesystemUtils.h
formatBinaryStream.h
scaleData.h
svm.h
waitKeySeconds.h

gbh/stDescriptor.h
gbh/integralVideo.h
gbh/sampled3droi.h
gbh/descFeature.h
gbh/fisherVector_Nc.h

mbh/stDescriptor.h
mbh/descFeature.h
mbh/fisherVector_Nc.h
mbh/integralVideo.h
mbh/sampled3droi.h
)


SET_SOURCE_FILES(
Descriptor.cpp
svm.cpp

gbh/descFeature.cpp
gbh/integralVideo.cpp
gbh/stDescriptor.cc

mbh/descFeature.cpp
mbh/integralVideo.cpp
mbh/stDescriptor.cc
)

END_PROJECT()
