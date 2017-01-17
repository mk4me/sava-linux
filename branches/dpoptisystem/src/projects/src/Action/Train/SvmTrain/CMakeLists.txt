BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
#include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/mbh")


SET_SOURCE_FILES(
	svmOvr.cpp     
	../../../Common/src/svm.cpp
)

END_PROJECT()
