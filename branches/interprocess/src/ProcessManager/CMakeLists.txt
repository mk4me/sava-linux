BEGIN_PROJECT("executable")


SET_PRIVATE_HEADERS(
Logger.h
ProcessManager.h
ProcessTab.h
)


SET_SOURCE_FILES(
main.cpp
Logger.cpp
ProcessManager.cpp
ProcessTab.cpp
)

SET_UI_FILES(
Logger.ui
ProcessManager.ui
ProcessTab.ui
)

SET_MOC_FILES(
Logger.h
ProcessManager.h
ProcessTab.h
)

#SET_RC_FILES ()
END_PROJECT()
