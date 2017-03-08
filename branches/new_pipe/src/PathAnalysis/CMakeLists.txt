BEGIN_PROJECT("executable")


SET_PRIVATE_HEADERS(

	ConfigurationWindow.h
	IVisualizer.h
	PathAnalysis.h
	NullVisualizer.h
	PathAnalysisVisualizer.h

)


SET_SOURCE_FILES(
	ConfigurationWindow.cpp
	Main.cpp
	PathAnalysis.cpp
	PathAnalysisVisualizer.cpp
)

SET_MOC_FILES(
		PathAnalysis.h
)


END_PROJECT()