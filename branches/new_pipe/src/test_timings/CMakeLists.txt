BEGIN_PROJECT("executable")
SET_PRIVATE_HEADERS(
  path_detection.h
  path_analysis.h
  fb_separation.h
)
SET_SOURCE_FILES(
  main.cpp
  path_detection.cpp
  path_analysis.cpp
  fb_separation.cpp
)
END_PROJECT()
