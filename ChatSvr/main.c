#include "Context.h"
#include "Server.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
#include "IPCProcedures.h"

#define C2S_COMMAND(__NAME__, __COMMAND__)                                                          \
Void SERVER_PROC_ ## __NAME__(                                                                      \
    ServerRef Server,                                                                               \
    Void *ServerContext,                                                                            \
    SocketRef Socket,                                                                               \
    SocketConnectionRef Connection,                                                                 \
    Void *ConnectionContext,                                                                        \
    Void *Packet                                                                                    \
) {                                                                                                 \
    ServerContextRef Context = (ServerContextRef)ServerContext;                                     \
    ClientContextRef Client = (ClientContextRef)ConnectionContext;                                  \
    PROC_ ## __NAME__(Server, Context, Socket, Connection, Client, (C2S_DATA_ ## __NAME__*)Packet); \
}
#include "ClientCommands.h"

#define IPC_COMMAND_CALLBACK(__NAMESPACE__, __NAME__)               \
Void SERVER_IPC_ ## __NAMESPACE__ ## _PROC_ ## __NAME__(            \
    IPCSocketRef Socket,                                            \
    IPCSocketConnectionRef Connection,                              \
    IPCPacketRef Packet                                             \
) {                                                                 \
    ServerRef Server = (ServerRef)Socket->Userdata;                 \
    ServerContextRef Context = (ServerContextRef)Server->Userdata;  \
    SocketConnectionRef ClientConnection = NULL;                    \
    ClientContextRef Client = NULL;                                 \
                                                                    \
    if (Packet->TargetConnectionID > 0) {                           \
        ClientConnection = SocketGetConnection(                     \
            Context->ClientSocket,                                  \
            Packet->TargetConnectionID                              \
        );                                                          \
        if (!ClientConnection) return;                              \
        Client = (ClientContextRef)ClientConnection->Userdata;      \
    }                                                               \
                                                                    \
    IPC_ ## __NAMESPACE__ ## _PROC_ ## __NAME__(                    \
        Server,                                                     \
        Context,                                                    \
        Socket,                                                     \
        Connection,                                                 \
        ClientConnection,                                           \
        Client,                                                     \
        (IPC_ ## __NAMESPACE__ ## _DATA_ ## __NAME__ *)Packet       \
    );                                                              \
}

#define IPC_W2C_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(W2C, __NAME__)
#include "IPCCommands.h"

#define IPC_D2C_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(D2C, __NAME__)
#include "IPCCommands.h"

Void ServerOnUpdate(
    ServerRef Server,
    Void *ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
}

Int32 main(Int32 ArgumentCount, CString* Arguments) {
    CString ConfigFileName = "ChatSvr.ini";
    if (ArgumentCount > 1) {
        ConfigFileName = Arguments[1];
    }

    Info("Load config file: %s", ConfigFileName);

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, ConfigFileName);
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    DiagnosticSetupLogFile(Arguments[0], Config.ChatSvr.LogLevel, NULL, NULL);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;

    IPCNodeID NodeID = kIPCNodeIDNull;
    NodeID.Group = Config.ChatSvr.GroupIndex;
    NodeID.Index = 0;
    NodeID.Type = IPC_TYPE_CHAT;

    ServerRef Server = ServerCreate(
        Allocator,
        NodeID,
        Config.MasterSvr.Host,
        Config.MasterSvr.Port,
        Config.MasterSvr.Timeout,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        Config.MasterSvr.LogPackets,
        &ServerOnUpdate,
        &ServerContext
    );
    ServerContext.Server = Server;
    ServerContext.IPCSocket = Server->IPCSocket;

    ServerContext.ClientSocket = ServerCreateSocket(
        Server,
        SOCKET_FLAGS_LISTENER | SOCKET_FLAGS_ENCRYPTED,
        NULL,
        Config.ChatSvr.Port,
        sizeof(struct _ClientContext),
        Config.NetLib.ProtocolIdentifier,
        Config.NetLib.ProtocolVersion,
        Config.NetLib.ProtocolExtension,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        Config.ChatSvr.MaxConnectionCount,
        Config.NetLib.PacketBufferBacklogSize,
        Config.NetLib.LogPackets,
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect
    );

#define C2S_COMMAND(__NAME__, __COMMAND__) \
    Trace("RegisterCommand(%d, C2S_%s)", C2S_ ## __NAME__, #__NAME__); \
    ServerSocketRegisterPacketCallback(Server, ServerContext.ClientSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__);
#include "ClientCommands.h"

#define IPC_W2C_COMMAND(__NAME__) \
    Trace("RegisterCommand(%d, IPC_W2C_%s)", IPC_W2C_ ## __NAME__, #__NAME__); \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_W2C_ ## __NAME__, &SERVER_IPC_W2C_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_D2C_COMMAND(__NAME__) \
    Trace("RegisterCommand(%d, IPC_D2C_%s)", IPC_D2C_ ## __NAME__, #__NAME__); \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_D2C_ ## __NAME__, &SERVER_IPC_D2C_PROC_ ## __NAME__);
#include "IPCCommands.h"

    ServerRun(Server);
    
    return EXIT_SUCCESS;
}
