BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")

SET_PRIVATE_HEADERS(
Recognizer.h
)


SET_SOURCE_FILES(
Recognizer.cpp
main.cpp
../../../Common/src/svm.cpp
)

END_PROJECT()
