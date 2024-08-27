#include "Context.h"
#include "Server.h"
#include "ServerDataLoader.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "NotificationProcedures.h"

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
    RTCharacterRef Character = NULL;                                \
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
    if (Client && Client->CharacterIndex > 0) {                     \
        Character = RTWorldManagerGetCharacterByIndex(              \
            Context->Runtime->WorldManager,                         \
            Client->CharacterIndex                                  \
        );                                                          \
    }                                                               \
                                                                    \
    IPC_ ## __NAMESPACE__ ## _PROC_ ## __NAME__(                    \
        Server,                                                     \
        Context,                                                    \
        Socket,                                                     \
        Connection,                                                 \
        ClientConnection,                                           \
        Client,                                                     \
        Context->Runtime,                                           \
        Character,                                                  \
        (IPC_ ## __NAMESPACE__ ## _DATA_ ## __NAME__ *)Packet       \
    );                                                              \
}

#define IPC_A2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(A2W, __NAME__)
#include "IPCCommands.h"

#define IPC_C2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(C2W, __NAME__)
#include "IPCCommands.h"

#define IPC_D2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(D2W, __NAME__)
#include "IPCCommands.h"

#define IPC_L2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(L2W, __NAME__)
#include "IPCCommands.h"

#define IPC_M2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(M2W, __NAME__)
#include "IPCCommands.h"

#define IPC_W2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(W2W, __NAME__)
#include "IPCCommands.h"

#define IPC_P2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(P2W, __NAME__)
#include "IPCCommands.h"

#define NOTIFICATION_PROTOCOL(__NAME__, __COMMAND__, __BODY__)      \
Void SERVER_NOTIFICATION_PROC_ ## __NAME__(                         \
    RTRuntimeRef Runtime,                                           \
    RTCharacterRef Character,                                       \
    RTNotificationRef Notification,                                 \
    Void* UserData                                                  \
) {                                                                 \
    ServerRef Server = (ServerRef)UserData;                         \
    ServerContextRef Context = (ServerContextRef)Server->Userdata;  \
    ClientContextRef Client = ServerGetClientByIndex(               \
        Context,                                                    \
        Character->CharacterIndex,                                  \
        Character->Name                                             \
    );                                                              \
    if (!Client) return;                                            \
    SocketConnectionRef ClientConnection = Client->Connection;      \
    if (!ClientConnection) return;                                  \
                                                                    \
    NOTIFICATION_PROC_ ## __NAME__(                                 \
        Server,                                                     \
        Context,                                                    \
        Context->ClientSocket,                                      \
        ClientConnection,                                           \
        Client,                                                     \
        Context->Runtime,                                           \
        Character,                                                  \
        (NOTIFICATION_DATA_ ## __NAME__ *)Notification              \
    );                                                              \
}
#include "RuntimeLib/NotificationProtocolDefinition.h"

Void ServerOnUpdate(
    ServerRef Server,
    Void *ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    RTRuntimeUpdate(Context->Runtime);
    ServerSyncDB(Server, Context, false);

    Timestamp CurrentTimestamp = GetTimestampMs();
    if (Context->UserListBroadcastTimestamp < CurrentTimestamp) {
        Context->UserListBroadcastTimestamp = CurrentTimestamp + Context->Config.WorldSvr.UserListBroadcastInterval;
        BroadcastUserList(Server, Context);
    }
}

Int32 main(Int32 ArgumentCount, CString* Arguments) {
    CString ConfigFileName = "WorldSvr.ini";
    if (ArgumentCount > 1) {
        ConfigFileName = Arguments[1];
    }

    Info("Load config file: %s", ConfigFileName);

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, ConfigFileName);
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    DiagnosticSetupLogFile(Arguments[0], Config.WorldSvr.LogLevel, NULL, NULL);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;
    ServerContext.Runtime = RTRuntimeCreate(Allocator, Config.WorldSvr.MaxPartyCount, &ServerContext);
    ServerContext.Runtime->Environment.IsPKEnabled = Config.Environment.IsPKEnabled;
    ServerContext.Runtime->Environment.IsPremiumEnabled = Config.Environment.IsPremiumEnabled;
    ServerContext.Runtime->Environment.IsWarEnabled = Config.Environment.IsWarEnabled;
    ServerContext.Runtime->Environment.IsRestrictedEnabled = Config.Environment.IsRestrictedEnabled;
    ServerContext.Runtime->Environment.IsEventEnabled = Config.Environment.IsEventEnabled;
    ServerContext.Runtime->Environment.IsNoviceEnabled = Config.Environment.IsNoviceEnabled;
    ServerContext.Runtime->Environment.IsOnly2FAEnabled = Config.Environment.IsOnly2FAEnabled;
    ServerContext.Runtime->Environment.IsRaidBossEnabled = Config.Environment.IsRaidBossEnabled;
    ServerContext.Runtime->Config.IsSkillRankUpLimitEnabled = Config.Environment.IsSkillRankUpLimitEnabled;
    ServerContext.Runtime->Config.WorldItemDespawnInterval = Config.WorldSvr.WorldItemDespawnInterval;
    ServerContext.Runtime->Config.NewbieSupportTimeout = Config.WorldSvr.NewbieSupportTimeout;

    IPCNodeID NodeID = kIPCNodeIDNull;
    NodeID.Group = Config.WorldSvr.GroupIndex;
    NodeID.Index = Config.WorldSvr.NodeIndex;
    NodeID.Type = IPC_TYPE_WORLD;

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
        Config.WorldSvr.Port,
        sizeof(struct _ClientContext),
        Config.NetLib.ProtocolIdentifier,
        Config.NetLib.ProtocolVersion,
        Config.NetLib.ProtocolExtension,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        Config.WorldSvr.MaxConnectionCount,
        Config.NetLib.LogPackets,
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect
    );
    
#define C2S_COMMAND(__NAME__, __COMMAND__) \
    Trace("RegisterCommand(%d, C2S_%s)", C2S_ ## __NAME__, #__NAME__); \
    ServerSocketRegisterPacketCallback(Server, ServerContext.ClientSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__);
#include "ClientCommands.h"

#define IPC_A2W_COMMAND(__NAME__) \
    Trace("RegisterCommand(%d, IPC_A2W_%s)", IPC_A2W_ ## __NAME__, #__NAME__); \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_A2W_ ## __NAME__, &SERVER_IPC_A2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_C2W_COMMAND(__NAME__) \
    Trace("RegisterCommand(%d, IPC_C2W_%s)", IPC_C2W_ ## __NAME__, #__NAME__); \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_C2W_ ## __NAME__, &SERVER_IPC_C2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_D2W_COMMAND(__NAME__) \
    Trace("RegisterCommand(%d, IPC_D2W_%s)", IPC_D2W_ ## __NAME__, #__NAME__); \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_D2W_ ## __NAME__, &SERVER_IPC_D2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_L2W_COMMAND(__NAME__) \
    Trace("RegisterCommand(%d, IPC_L2W_%s)", IPC_L2W_ ## __NAME__, #__NAME__); \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_L2W_ ## __NAME__, &SERVER_IPC_L2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_M2W_COMMAND(__NAME__) \
    Trace("RegisterCommand(%d, IPC_M2W_%s)", IPC_M2W_ ## __NAME__, #__NAME__); \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_M2W_ ## __NAME__, &SERVER_IPC_M2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_W2W_COMMAND(__NAME__) \
    Trace("RegisterCommand(%d, IPC_W2W_%s)", IPC_W2W_ ## __NAME__, #__NAME__); \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_W2W_ ## __NAME__, &SERVER_IPC_W2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_P2W_COMMAND(__NAME__) \
    Trace("RegisterCommand(%d, IPC_P2W_%s)", IPC_P2W_ ## __NAME__, #__NAME__); \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_P2W_ ## __NAME__, &SERVER_IPC_P2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define NOTIFICATION_PROTOCOL(__NAME__, __COMMAND__, __BODY__) \
    Trace("RegisterNotification(%d, RUNTIME_NOTIFICATION_%s)", __COMMAND__, #__NAME__); \
    RTNotificationManagerRegisterCallback(ServerContext.Runtime->NotificationManager, __COMMAND__, SERVER_NOTIFICATION_PROC_ ## __NAME__, Server);
#include "RuntimeLib/NotificationProtocolDefinition.h"
    
    RTRuntimeLoadData(ServerContext.Runtime, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath);
    ServerLoadRuntimeData(Config, &ServerContext);
    ServerLoadScriptData(Config, &ServerContext);
    ServerRun(Server);
    RTRuntimeDestroy(ServerContext.Runtime);
    DiagnosticTeardown();
    
    return EXIT_SUCCESS;
}
