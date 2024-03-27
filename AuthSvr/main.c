#include "AuthDB.h"
#include "Context.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
#include "MasterSocket.h"
#include "IPCProcs.h"
#include "Notification.h"

#define C2S_COMMAND(__NAME__, __COMMAND__)                  \
Void SERVER_PROC_ ## __NAME__(                              \
    ServerRef Server,                                       \
    Void *ServerContext,                                    \
    SocketRef Socket,                                       \
    SocketConnectionRef Connection,                         \
    Void *ConnectionContext,                                \
    Void *Packet                                            \
) {                                                         \
    PROC_ ## __NAME__(                                      \
        Server,                                             \
        (ServerContextRef)ServerContext,                    \
        Socket,                                             \
        Connection,                                         \
        (ClientContextRef)ConnectionContext,                \
        (C2S_DATA_ ## __NAME__*)Packet                      \
    );                                                      \
}
#include "ClientCommands.h"

#define IPC_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__) \
Void SERVER_ ## __NAME__(                                  \
    ServerRef Server,                                      \
    Void *ServerContext,                                   \
    SocketRef Socket,                                      \
    SocketConnectionRef Connection,                        \
    Void *ConnectionContext,                               \
    Void *Packet                                           \
) {                                                        \
    __NAME__(                                              \
        Server,                                            \
        (ServerContextRef)ServerContext,                   \
        Socket,                                            \
        Connection,                                        \
        (MasterContextRef)ConnectionContext,               \
        (__PROTOCOL__*)Packet                              \
    );                                                     \
}
#include "IPCProcDefinition.h"

Void ServerOnUpdate(
    ServerRef Server,
    Void *ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client->Flags & CLIENT_FLAGS_CHECK_DISCONNECT_TIMER) {
            if (Client->DisconnectTimestamp < ServerGetTimestamp(Server)) {
                Client->Flags &= ~CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
                SocketDisconnect(Context->ClientSocket, Connection);
                continue;
            }
        }
    }

    if (Context->WorldListBroadcastTimestamp < Context->WorldListUpdateTimestamp) {
        Context->WorldListBroadcastTimestamp = ServerGetTimestamp(Server);
        Context->WorldListUpdateTimestamp = Context->WorldListBroadcastTimestamp;
        BroadcastWorldList(Server, Context);
    }
}

Int32 main(Int32 argc, CString* argv) {
    DiagnosticCreateLogFile("AuthSvr");

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "AuthSvr.ini");
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Allocator = Allocator;
    ServerContext.Config = Config;
    ServerContext.ClientSocket = NULL;
    ServerContext.MasterSocket = NULL;
    ServerContext.Database = NULL;
    ServerContext.WorldListBroadcastTimestamp = 0;
    ServerContext.WorldListUpdateTimestamp = 0;
    ServerRef Server = ServerCreate(Allocator, &ServerOnUpdate, &ServerContext);
    
    ServerContext.ClientSocket = ServerCreateSocket(
        Server,
        SOCKET_FLAGS_LISTENER | SOCKET_FLAGS_ENCRYPTED,
        NULL,
        Config.Auth.Port,
        sizeof(struct _ClientContext),
        Config.NetLib.ProtocolIdentifier,
        Config.NetLib.ProtocolVersion,
        Config.NetLib.ProtocolExtension,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        Config.Auth.MaxConnectionCount,
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect
    );

#define C2S_COMMAND(__NAME__, __COMMAND__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.ClientSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__);
#include "ClientCommands.h"

    ServerContext.MasterSocket = ServerCreateSocket(
        Server,
        SOCKET_FLAGS_LISTENER | SOCKET_FLAGS_IPC,
        NULL,
        Config.MasterSvr.Port,
        sizeof(struct _MasterContext),
        Config.NetLib.ProtocolIdentifier,
        Config.NetLib.ProtocolVersion,
        Config.NetLib.ProtocolExtension,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        Config.MasterSvr.MaxServerCount,
        &MasterSocketOnConnect,
        &MasterSocketOnDisconnect
    );
    
#define IPC_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.MasterSocket, __COMMAND__, &SERVER_ ## __NAME__);
#include "IPCProcDefinition.h"

    ServerContext.Database = DatabaseConnect(
        Config.AuthDB.Host,
        Config.AuthDB.Username,
        Config.AuthDB.Password,
        Config.AuthDB.Database,
        Config.AuthDB.Port
    );
    if (!ServerContext.Database) FatalError("Database connection failed");

    AuthDBPrepareStatements(ServerContext.Database);
    ServerRun(Server);
    ServerDestroy(Server);
    DatabaseDisconnect(ServerContext.Database);
    DiagnosticTeardown();

    return EXIT_SUCCESS;
}
