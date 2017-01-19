BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS(
		dplog.h
		dputils.h
)

SET_SOURCE_FILES(
	Main.cpp
		dplog.cpp
		dputils.cpp
)

END_PROJECT()
