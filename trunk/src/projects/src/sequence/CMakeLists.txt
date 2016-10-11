BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	Action.h
	Cluster.h
	CompressedVideo.h
	GpuVideo.h
	Image.h
	IStreamedVideo.h
	IVideo.h
	MetaVideo.h
	PathStream.h
	RoiVideo.h
	StreamedVideo.h
	Video.h
	VideoPack.h
	VideoUtils.h
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include/sequence/")
#------------------------------------------------------------------------------

SET_SOURCE_FILES(
	Action.cpp
    Image.cpp
    PathStream.cpp
    VideoPack.cpp
	Cluster.cpp
    IStreamedVideo.cpp
	RoiVideo.cpp
	VideoUtils.cpp
	CompressedVideo.cpp
	IVideo.cpp
    StreamedVideo.cpp
	#GpuVideo.cpp # sava-linux.rev
    MetaVideo.cpp
    Video.cpp
)

#SET_MOC_FILES(
#	CvZoom.h
#)


END_PROJECT()