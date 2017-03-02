BEGIN_PROJECT("static")


SET_PUBLIC_HEADERS(
	DefaultPathDetector.h
	DefaultSequenceDetector.h
	IPathDetector.h
	ISequenceDetector.h
	OpticalFlowPathDetector.h
	OpticalFlowSequenceDetector.h
	path.h
	PredictedRoiFilter.h
	PredictedRoiPathDetector.h
)


SET_SOURCE_FILES(
	DefaultPathDetector.cpp
	DefaultSequenceDetector.cpp
	OpticalFlowPathDetector.cpp
	OpticalFlowSequenceDetector.cpp
	path.cpp
	PredictedRoiFilter.cpp
	PredictedRoiPathDetector.cpp
)


END_PROJECT()
