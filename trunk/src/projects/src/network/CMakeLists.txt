BEGIN_PROJECT("static")


SET_PUBLIC_HEADERS(
AbstractCommand.h
Acceptor.h
BroadcastReceiver.h
BroadcastSender.h
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


SET_SOURCE_FILES(
AbstractCommand.cpp
Acceptor.cpp
BroadcastReceiver.cpp
BroadcastSender.cpp
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

END_PROJECT()
