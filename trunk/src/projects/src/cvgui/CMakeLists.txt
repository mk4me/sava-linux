BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	Button.h
	Trackbar.h
	Widget.h
	Window.h
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include/cvgui/")
#------------------------------------------------------------------------------

SET_SOURCE_FILES(
	Button.cpp
	Trackbar.cpp
	Widget.cpp
	Window.cpp
)



END_PROJECT()