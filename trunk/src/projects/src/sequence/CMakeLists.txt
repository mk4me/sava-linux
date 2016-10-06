BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	Action.h
	Image.h
	PathStream.h
	Video.h
	Cluster.h   
	IStreamedVideo.h
	RoiVideo.h
	VideoUtils.h
	CompressedVideo.h
	IVideo.h
	StreamedVideo.h

)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include/sequence/")
#------------------------------------------------------------------------------

SET_SOURCE_FILES(
	Action.cpp           
	Cluster.cpp          
	CompressedVideo.cpp  
	Image.cpp           
	IStreamedVideo.cpp  
	IVideo.cpp          
	PathStream.cpp     
	RoiVideo.cpp       
	StreamedVideo.cpp
	Video.cpp
	VideoUtils.cpp
)

#SET_MOC_FILES(
#	CvZoom.h
#)


END_PROJECT()