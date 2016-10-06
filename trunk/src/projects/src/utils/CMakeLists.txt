BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	SMilestoneStructures.h
	CvSerialization.h
	IAppModule.h
	ProgramOptions.h
	Singleton.h
	CvZoom.h
	WebUtils.h
	FileLock.h
	CFPSCounter.h
	timer.h
	matrix.h
	StatisticsProvider.h
	Spinlock.h
	Glossary.h
	CvZoomObjects.h
	gui.h
	Stat.h
	Hungarian.h
	CAxisRawReader.h
	Filesystem.h
	SRawMJFPFrame.h
	Console.h
	munkres.h
	pathFinder.h
	PipeProcessUtils.h
	BlockingQueue.h
	WindowsUtilities.h
	ImageConverter.h
	MemTools.h
	CCameraEnumerator.h
	PipeProcess.h
	Application.h
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include/utils/")
#------------------------------------------------------------------------------

SET_SOURCE_FILES(
	Application.cpp
	CAxisRawReader.cpp
	StatisticsProvider.cpp
	CCameraEnumerator.cpp
	CvZoomObjects.cpp
	Filesystem.cpp
	WebUtils.cpp
	Glossary.cpp
	munkres.cpp
	pathFinder.cpp
	FileLock.cpp
	gui.cpp
	CvZoom.cpp
	PipeProcessUtils.cpp
	PipeProcess.cpp
	Hungarian.cpp
)

SET_MOC_FILES(
	CvZoom.h
	PipeProcess.h
)


END_PROJECT()