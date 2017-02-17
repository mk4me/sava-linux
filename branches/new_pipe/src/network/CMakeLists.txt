BEGIN_PROJECT("static")


SET_PUBLIC_HEADERS(
AbstractCommand.h
Acceptor.h
ConnectionManager.h
Connector.h
KillCommand.h
NetworkMessage.h
ProcessManagerLib.h
ProcessServer.h
RaportErrorCommand.h
RaportFinishedCommand.h
Session.h
SessionHub.h
SessionUser.h
StartCommand.h
StopCommand.h

)


include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include/network/")


SET_SOURCE_FILES(
AbstractCommand.cpp
Acceptor.cpp
ConnectionManager.cpp
Connector.cpp
KillCommand.cpp
ProcessManagerLib.cpp
ProcessServer.cpp
RaportErrorCommand.cpp
RaportFinishedCommand.cpp
Session.cpp
SessionHub.cpp
StartCommand.cpp
StopCommand.cpp
)

SET_MOC_FILES(
AbstractCommand.h
Acceptor.h
ConnectionManager.h
Connector.h
KillCommand.h
NetworkMessage.h
ProcessManagerLib.h
ProcessServer.h
RaportErrorCommand.h
RaportFinishedCommand.h
Session.h
SessionHub.h
SessionUser.h
StartCommand.h
StopCommand.h

)

END_PROJECT()
