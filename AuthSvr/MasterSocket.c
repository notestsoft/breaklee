#include "MasterSocket.h"
#include "Server.h"
#include "IPCProcs.h"

Void MasterSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    MasterContextRef Master = (MasterContextRef)ConnectionContext;
    Master->ServerID = 0;
    Master->Worlds = ArrayCreateEmpty(AllocatorGetSystemDefault(), sizeof(struct _MasterContextWorld), 0);
}

Void MasterSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    MasterContextRef Master = (MasterContextRef)ConnectionContext;
    ArrayDestroy(Master->Worlds);
}
