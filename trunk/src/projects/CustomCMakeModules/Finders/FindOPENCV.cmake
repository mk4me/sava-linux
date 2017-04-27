# przygotowanie do szukania
FIND_INIT(OPENCV opencv)

# szukanie
# funkcja wykrywaj¹ce wersjê OPEN CV
function(OPENCV_FIND_VERSION path)    
	# inicjalizacja
	set(OPENCV_VERSION "Unknown" CACHE STRING "Unknown version")
	# próba odczytania wersji z pliku
	if (EXISTS ${path})
		file(READ "${path}" _opencv_Version_contents)
		string(REGEX REPLACE ".*#define CV_VERSION_REVISION[ \t]+([0-9]+).*"
            "\\1" _version_subminor ${_opencv_Version_contents})
        string(REGEX REPLACE ".*#define CV_VERSION_MAJOR[ \t]+([0-9]+).*"
            "\\1" _version_major ${_opencv_Version_contents})
        string(REGEX REPLACE ".*#define CV_VERSION_MINOR[ \t]+([0-9]+).*"
            "\\1" _version_minor ${_opencv_Version_contents})
		if (OPENCV_VERSION STREQUAL "Unknown")
			set(OPENCV_VERSION "${_version_major}${_version_minor}${_version_subminor}" CACHE STRING "The version of OSG_VERSION${suffix} which was detected" FORCE)
		endif()
    endif()
	# czy siê uda³o?
	if (OPENCV_VERSION STREQUAL "Unknown")
		FIND_NOTIFY(OPENCV_VERSION "Unknown version of OPENCV_VERSION. File ${path} could not be read. This may result in further errors.")
	endif()
endfunction(OPENCV_FIND_VERSION)

# wykrycie wersji osg
OPENCV_FIND_VERSION("${OPENCV_INCLUDE_DIR}/opencv2/core/version.hpp")

FIND_SHARED(OPENCV_CALIB3D "<lib,?>opencv_calib3d<${OPENCV_VERSION},?>" "<lib,?>opencv_calib3d<${OPENCV_VERSION},?>")
#FIND_SHARED(OPENCV_CONTRIB "<lib,?>opencv_contrib<${OPENCV_VERSION},?>" "<lib,?>opencv_contrib<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_CORE "<lib,?>opencv_core<${OPENCV_VERSION},?>" "<lib,?>opencv_core<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_FEATURES2D "<lib,?>opencv_features2d<${OPENCV_VERSION},?>" "<lib,?>opencv_features2d<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_XFEATURES2D "<lib,?>opencv_xfeatures2d<${OPENCV_VERSION},?>" "<lib,?>opencv_xfeatures2d<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_FLANN "<lib,?>opencv_flann<${OPENCV_VERSION},?>" "<lib,?>opencv_flann<${OPENCV_VERSION},?>")
#FIND_SHARED(OPENCV_GPU "<lib,?>opencv_gpu<${OPENCV_VERSION},?>" "<lib,?>opencv_gpu<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_HIGHGUI "<lib,?>opencv_highgui<${OPENCV_VERSION},?>" "<lib,?>opencv_highgui<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_IMGCODECS "<lib,?>opencv_imgcodecs<${OPENCV_VERSION},?>" "<lib,?>opencv_imgcodecs<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_VIDEOIO "<lib,?>opencv_videoio<${OPENCV_VERSION},?>" "<lib,?>opencv_videoio<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_IMGPROC "<lib,?>opencv_imgproc<${OPENCV_VERSION},?>" "<lib,?>opencv_imgproc<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_CUDAIMGPROC "<lib,?>opencv_cudaimgproc<${OPENCV_VERSION},?>" "<lib,?>opencv_cudaimgproc<${OPENCV_VERSION},?>")
#FIND_SHARED(OPENCV_LEGACY "<lib,?>opencv_legacy<${OPENCV_VERSION},?>" "<lib,?>opencv_legacy<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_ML "<lib,?>opencv_ml<${OPENCV_VERSION},?>" "<lib,?>opencv_ml<${OPENCV_VERSION},?>")
#FIND_SHARED(OPENCV_NONFREE "<lib,?>opencv_nonfree<${OPENCV_VERSION},?>" "<lib,?>opencv_nonfree<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_OBJDETECT "<lib,?>opencv_objdetect<${OPENCV_VERSION},?>" "<lib,?>opencv_objdetect<${OPENCV_VERSION},?>")
# FIND_SHARED(OPENCV_OCL "<lib,?>opencv_ocl<${OPENCV_VERSION},?>" "<lib,?>opencv_ocl<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_OPTFLOW "<lib,?>opencv_optflow<${OPENCV_VERSION},?>" "<lib,?>opencv_optflow<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_CUDAOPTFLOW "<lib,?>opencv_cudaoptflow<${OPENCV_VERSION},?>" "<lib,?>opencv_cudaoptflow<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_CUDAARITHM "<lib,?>opencv_cudaarithm<${OPENCV_VERSION},?>" "<lib,?>opencv_cudaarithm<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_CUDABGSEGM "<lib,?>opencv_cudabgsegm<${OPENCV_VERSION},?>" "<lib,?>opencv_cudabgsegm<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_CUDALEGACY "<lib,?>opencv_cudalegacy<${OPENCV_VERSION},?>" "<lib,?>opencv_cudalegacy<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_CUDAWARPING "<lib,?>opencv_cudawarping<${OPENCV_VERSION},?>" "<lib,?>opencv_cudawarping<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_CUDAFILTERS "<lib,?>opencv_cudafilters<${OPENCV_VERSION},?>" "<lib,?>opencv_cudafilters<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_PHOTO "<lib,?>opencv_photo<${OPENCV_VERSION},?>" "<lib,?>opencv_photo<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_STITCHING "<lib,?>opencv_stitching<${OPENCV_VERSION},?>" "<lib,?>opencv_stitching<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_SUPERRES "<lib,?>opencv_superres<${OPENCV_VERSION},?>" "<lib,?>opencv_superres<${OPENCV_VERSION},?>")
#FIND_SHARED(OPENCV_TS "<lib,?>opencv_ts<${OPENCV_VERSION},?>" "<lib,?>opencv_ts<${OPENCV_VERSION},?>")
FIND_SHARED(OPENCV_VIDEO "<lib,?>opencv_video<${OPENCV_VERSION},?>" "<lib,?>opencv_video<${OPENCV_VERSION},?>")



if (WIN32)
	#opencv_ffmpeg nie ma liba, dlatego trzeba jedynie znalezc i skopiowac w odpowiednie miejsce dllki
	FIND_DLL(OPENCV_FFMPEG "opencv_ffmpeg${OPENCV_VERSION}<_${SOLUTION_PROCESSOR_PLATFORM},?>" "opencv_ffmpeg${OPENCV_VERSION}<_${SOLUTION_PROCESSOR_PLATFORM},?>")
	FIND_PREREQUISITES(OPENCV "QT;FFMPEG;TBB")
elseif (UNIX)
	FIND_DEPENDENCIES(OPENCV "QT;FFMPEG;TBB")
endif()

# koniec
FIND_FINISH(OPENCV)
