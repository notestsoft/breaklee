#include "Context.h"
#include "Server.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
#include "IPCProcedures.h"
#include "Notification.h"

#define C2S_COMMAND(__NAME__, __COMMAND__)                                                                                       \
Void SERVER_PROC_ ## __NAME__(                                                                                                   \
    ServerRef Server,                                                                                                            \
    Void *ServerContext,                                                                                                         \
    SocketRef Socket,                                                                                                            \
    SocketConnectionRef Connection,                                                                                              \
    Void *ConnectionContext,                                                                                                     \
    Void *Packet                                                                                                                 \
) {                                                                                                                              \
    ServerContextRef Context = (ServerContextRef)ServerContext;                                                                  \
    ClientContextRef Client = (ClientContextRef)ConnectionContext;                                                               \
    RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, Client->CharacterIndex);        \
    PROC_ ## __NAME__(Server, Context, Socket, Connection, Client, Context->Runtime, Character, (C2S_DATA_ ## __NAME__*)Packet); \
}
#include "ClientCommands.h"

#define IPC_COMMAND_CALLBACK(__NAMESPACE__, __NAME__)                       \
Void SERVER_IPC_ ## __NAMESPACE__ ## _PROC_ ## __NAME__(                    \
    IPCSocketRef Socket,                                                    \
    IPCSocketConnectionRef Connection,                                      \
    IPCPacketRef Packet                                                     \
) {                                                                         \
    ServerRef Server = (ServerRef)Socket->Userdata;                         \
    ServerContextRef Context = (ServerContextRef)Server->Userdata;          \
    IPCNodeContextRef NodeContext = (IPCNodeContextRef)Connection->Userdata;\
                                                                            \
    IPC_ ## __NAMESPACE__ ## _PROC_ ## __NAME__(                            \
        Server,                                                             \
        Context,                                                            \
        Socket,                                                             \
        Connection,                                                         \
        NodeContext,                                                        \
        (IPC_ ## __NAMESPACE__ ## _DATA_ ## __NAME__ *)Packet               \
    );                                                                      \
}

#define IPC_W2P_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(W2P, __NAME__)
#include "IPCCommands.h"

Void ServerOnUpdate(
    ServerRef Server,
    Void *ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    
}

Int32 main(Int32 argc, CString* argv) {
    DiagnosticSetupLogFile("PartySvr", LOG_LEVEL_TRACE, NULL, NULL);

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "PartySvr.ini");
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;
    ServerContext.PartyManager = RTPartyManagerCreate(Allocator, Config.PartySvr.MaxPartyCount);

    IPCNodeID NodeID = kIPCNodeIDNull;
    NodeID.Group = Config.PartySvr.GroupIndex;
    NodeID.Type = IPC_TYPE_PARTY;

    ServerRef Server = ServerCreate(
        Allocator,
        NodeID,
        Config.MasterSvr.Host,
        Config.MasterSvr.Port,
        Config.MasterSvr.Timeout,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        &ServerOnUpdate,
        &ServerContext
    );

    ServerContext.ClientSocket = ServerCreateSocket(
        Server,
        SOCKET_FLAGS_LISTENER | SOCKET_FLAGS_ENCRYPTED,
        NULL,
        Config.PartySvr.Port,
        sizeof(struct _ClientContext),
        Config.NetLib.ProtocolIdentifier,
        Config.NetLib.ProtocolVersion,
        Config.NetLib.ProtocolExtension,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        Config.PartySvr.MaxConnectionCount,
        Config.NetLib.LogPackets,
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect
    );
    
#define C2S_COMMAND(__NAME__, __COMMAND__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.ClientSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__);
#include "ClientCommands.h"

#define IPC_W2P_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_W2P_ ## __NAME__, &SERVER_IPC_W2P_PROC_ ## __NAME__);
#include "IPCCommands.h"

    ServerRun(Server);

    RTPartyManagerDestroy(ServerContext.PartyManager);
    DiagnosticTeardown();
    
    return EXIT_SUCCESS;
}
