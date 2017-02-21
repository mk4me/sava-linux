BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	Action.h
	Cluster.h
	CompressedVideo.h
	ComplexAction.h
	GpuVideo.h
	Image.h
	IStreamedVideo.h
	BackgroundSeparation.h
	IVideo.h
	MetaVideo.h
	PathStream.h
	RoiVideo.h
	StreamedVideo.h
	#VideoAccesor.h
	Video.h
	VideoPack.h
	VideoUtils.h
	DefaultPacker.h
	GpuPacker.h
	IPacker.h
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include/sequence/")
#------------------------------------------------------------------------------

SET_SOURCE_FILES(
	Action.cpp
    Image.cpp
    PathStream.cpp
    VideoPack.cpp
	Cluster.cpp
    BackgroundSeparation.cpp
	ComplexAction.cpp
    IStreamedVideo.cpp
	RoiVideo.cpp
	#VideoAccesor.cpp
	VideoUtils.cpp
	CompressedVideo.cpp
	IVideo.cpp
    StreamedVideo.cpp
	GpuVideo.cpp
    MetaVideo.cpp
    Video.cpp
	DefaultPacker.cpp
	GpuPacker.cpp
)

#SET_MOC_FILES(
#	CvZoom.h
#)


END_PROJECT()
