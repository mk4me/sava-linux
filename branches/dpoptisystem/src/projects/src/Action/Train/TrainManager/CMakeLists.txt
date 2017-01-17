BEGIN_PROJECT("executable")

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/")
#include_directories("${CMAKE_CURRENT_LIST_DIR}/../../Common/src/mbh")

SET_PRIVATE_HEADERS(
ReadOnlyCheckBox.h
TrainManager.h    
)

SET_SOURCE_FILES(
main.cpp
ReadOnlyCheckBox.cpp
TrainManager.cpp
)

SET_UI_FILES(
TrainManager.ui
)

SET_MOC_FILES(
        ReadOnlyCheckBox.h
        TrainManager.h
)
END_PROJECT()
