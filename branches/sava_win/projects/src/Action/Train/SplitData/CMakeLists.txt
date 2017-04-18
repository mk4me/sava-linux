BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")


SET_PRIVATE_HEADERS(
matOperations.h
)

SET_SOURCE_FILES(
data2Files_scale_hmdb51.cc
matOperations.cc
#parseBinary2txt.cc        
)

END_PROJECT()
