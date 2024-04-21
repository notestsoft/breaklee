#include "Context.h"
#include "Server.h"
#include "ServerDataLoader.h"
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

// TODO: This is not a good solution considering the connection id is being reused
Index PacketGetConnectionID(
    Void* Packet
) {
    return *(Index*)((UInt8*)Packet + 8);
}

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

#define IPC_L2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(L2W, __NAME__)
#include "IPCCommands.h"

#define IPC_M2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(M2W, __NAME__)
#include "IPCCommands.h"

#define IPC_W2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(W2W, __NAME__)
#include "IPCCommands.h"

#define IPC_P2W_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(P2W, __NAME__)
#include "IPCCommands.h"

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

    DiagnosticCreateLogFile("WorldSvr");
    LogMessageFormat(LOG_LEVEL_INFO, "ConfigFile: %s", ConfigFileName);

    if (!EncryptionLoadLibrary()) FatalError("Error loading zlib library...\n");

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, ConfigFileName);
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;
    ServerContext.RuntimeData = AllocatorAllocate(Allocator, sizeof(struct _RuntimeData));
    if (!ServerContext.RuntimeData) FatalError("Memory allocation failed");
    ServerContext.Runtime = RTRuntimeCreate(Allocator, &ServerRuntimeOnEvent, &ServerContext);
    ServerContext.Runtime->Config.ExpMultiplier = Config.WorldSvr.ExpMultiplier;
    ServerContext.Runtime->Config.SkillExpMultiplier = Config.WorldSvr.SkillExpMultiplier;

    IPCNodeID NodeID = kIPCNodeIDNull;
    NodeID.Group = Config.WorldSvr.GroupIndex;
    NodeID.Index = Config.WorldSvr.NodeIndex;
    NodeID.Type = IPC_TYPE_WORLD;

    LogMessageFormat(LOG_LEVEL_INFO, "Node(%d, %d, %d)", NodeID.Group, NodeID.Index, NodeID.Type);

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
    ServerSocketRegisterPacketCallback(Server, ServerContext.ClientSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__);
#include "ClientCommands.h"

#define IPC_A2W_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_A2W_ ## __NAME__, &SERVER_IPC_A2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_C2W_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_C2W_ ## __NAME__, &SERVER_IPC_C2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_L2W_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_L2W_ ## __NAME__, &SERVER_IPC_L2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_M2W_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_M2W_ ## __NAME__, &SERVER_IPC_M2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_W2W_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_W2W_ ## __NAME__, &SERVER_IPC_W2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_P2W_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_P2W_ ## __NAME__, &SERVER_IPC_P2W_PROC_ ## __NAME__);
#include "IPCCommands.h"

    RTRuntimeLoadData(ServerContext.Runtime, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath);
    ServerLoadRuntimeData(Config, &ServerContext);

    ServerRun(Server);

    for (Index WorldIndex = 0; WorldIndex < ServerContext.Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!RTWorldDataExists(ServerContext.Runtime->WorldManager, WorldIndex)) continue;

        RTWorldDataRef World = RTWorldDataGet(ServerContext.Runtime->WorldManager, WorldIndex);
        ArrayDestroy(World->MobTable);
    }

    for (Int32 Index = 0; Index < ServerContext.Runtime->MissionDungeonPatternPartDataCount; Index += 1) {
        ArrayDestroy(ServerContext.Runtime->MissionDungeonPatternPartData[Index].MobTable);
    }

    RTRuntimeDestroy(ServerContext.Runtime);
    AllocatorDeallocate(Allocator, ServerContext.RuntimeData);
    EncryptionUnloadLibrary();
    DiagnosticTeardown();
    
    return EXIT_SUCCESS;
}
