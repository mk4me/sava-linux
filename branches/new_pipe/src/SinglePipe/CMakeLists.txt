BEGIN_PROJECT("executable")


SET_PRIVATE_HEADERS(
    AquisitionPipe.h
	AnalysisPipe.h
)


SET_SOURCE_FILES(
	AquisitionPipe.cpp
	AnalysisPipe.cpp
	main.cpp
)

END_PROJECT()