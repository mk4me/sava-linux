BEGIN_PROJECT("executable")


SET_PRIVATE_HEADERS(
	DefaultPathDetector.h
	DefaultSequenceDetector.h
	IPathDetector.h
	ISequenceDetector.h
	OpticalFlowPathDetector.h
	OpticalFlowSequenceDetector.h
	path.h
	PathDetection.h
	PredictedRoiFilter.h
	PredictedRoiPathDetector.h
)


SET_SOURCE_FILES(
	DefaultPathDetector.cpp
	DefaultSequenceDetector.cpp
	Main.cpp	
	OpticalFlowPathDetector.cpp
	OpticalFlowSequenceDetector.cpp
	path.cpp
	PathDetection.cpp
	PredictedRoiFilter.cpp
	PredictedRoiPathDetector.cpp
)


SET_MOC_FILES(
		PathDetection.h
)

END_PROJECT()