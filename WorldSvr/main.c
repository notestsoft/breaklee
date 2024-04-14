#include "Context.h"
#include "Server.h"
#include "ServerDataLoader.h"
#include "ClientSocket.h"
#include "MasterSocket.h"
#include "ClientProcedures.h"
#include "IPCProcs.h"
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

Void ServerOnIPCPacketReceived(
    ServerRef Server,
    Void* ServerContext,
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    Void* ConnectionContext,
    IPCPacketRef Packet
) {

}

// TODO: This is not a good solution considering the connection id is being reused
Index PacketGetConnectionID(
    Void* Packet
) {
    return *(Index*)((UInt8*)Packet + 8);
}

#define IPC_MASTER_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__)                                               \
Void SERVER_ ## __NAME__(                                                                                       \
    ServerRef Server,                                                                                           \
    Void *ServerContext,                                                                                        \
    SocketRef Socket,                                                                                           \
    SocketConnectionRef Connection,                                                                             \
    Void *ConnectionContext,                                                                                    \
    Void *Packet                                                                                                \
) {                                                                                                             \
    ServerContextRef Context = (ServerContextRef)ServerContext;                                                 \
    MasterContextRef Master = (MasterContextRef)ConnectionContext;                                              \
    Index ConnectionID = PacketGetConnectionID(Packet);                                                         \
    SocketConnectionRef ClientConnection = SocketGetConnection(Context->ClientSocket, ConnectionID);            \
    ClientContextRef Client = NULL;                                                                             \
    RTCharacterRef Character = NULL;                                                                            \
                                                                                                                \
    if (ClientConnection) {                                                                                     \
        Client = (ClientContextRef)ClientConnection->Userdata;                                                  \
    }                                                                                                           \
                                                                                                                \
    if (Client && Client->CharacterIndex > 0) {                                                                 \
        Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, Client->CharacterIndex);  \
    }                                                                                                           \
                                                                                                                \
    __NAME__(                                                                                                   \
        Server,                                                                                                 \
        Context,                                                                                                \
        Socket,                                                                                                 \
        Connection,                                                                                             \
        Master,                                                                                                 \
        ClientConnection,                                                                                       \
        Client,                                                                                                 \
        Context->Runtime,                                                                                       \
        Character,                                                                                              \
        (__PROTOCOL__*)Packet                                                                                   \
    );                                                                                                          \
}
#include "IPCProcDefinition.h"

Void ServerOnUpdate(
    ServerRef Server,
    Void *ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    RTRuntimeUpdate(Context->Runtime);

    Bool IsMasterConnected = Context->MasterSocket->Flags & SOCKET_FLAGS_CONNECTED;
    if (Context->UserListBroadcastTimestamp < Context->UserListUpdateTimestamp && IsMasterConnected) {
        Context->UserListBroadcastTimestamp = ServerGetTimestamp(Server);
        Context->UserListUpdateTimestamp = Context->UserListBroadcastTimestamp;
        BroadcastUserList(Context);
    }

    ServerSyncDB(Server, Context, false);
}

Int32 main(Int32 argc, CString* argv) {
    DiagnosticCreateLogFile("WorldSvr");

    if (!EncryptionLoadLibrary()) FatalError("Error loading zlib library...\n");

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "WorldSvr.ini");
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;
    ServerContext.RuntimeData = AllocatorAllocate(Allocator, sizeof(struct _RuntimeData));
    if (!ServerContext.RuntimeData) FatalError("Memory allocation failed");
    ServerContext.Runtime = RTRuntimeCreate(Allocator, &ServerRuntimeOnEvent, &ServerContext);
    ServerContext.Runtime->Config.ExpMultiplier = Config.WorldSvr.ExpMultiplier;
    ServerContext.Runtime->Config.SkillExpMultiplier = Config.WorldSvr.SkillExpMultiplier;
    ServerContext.UserListBroadcastTimestamp = 0;
    ServerContext.UserListUpdateTimestamp = 0;

    IPCNodeID NodeID = kIPCNodeIDNull;
    NodeID.Group = 1;
    NodeID.Index = 1;
    NodeID.Type = IPC_TYPE_WORLD;

    ServerRef Server = ServerCreate(
        Allocator,
        NodeID,
        Config.MasterSvr.Host,
        Config.MasterSvr.Port,
        Config.MasterSvr.Timeout,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        &ServerOnUpdate,
        &ServerOnIPCPacketReceived,
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
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect
    );
    
#define C2S_COMMAND(__NAME__, __COMMAND__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.ClientSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__);
#include "ClientCommands.h"
    
    ServerContext.MasterSocket = ServerCreateSocket(
        Server,
        SOCKET_FLAGS_IPC,
        Config.MasterSvr.Host,
        Config.MasterSvr.Port,
        sizeof(struct _MasterContext),
        Config.NetLib.ProtocolIdentifier,
        Config.NetLib.ProtocolVersion,
        Config.NetLib.ProtocolExtension,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        1,
        &MasterSocketOnConnect,
        &MasterSocketOnDisconnect
    );
    
#define IPC_MASTER_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.MasterSocket, __COMMAND__, &SERVER_ ## __NAME__);
#include "IPCProcDefinition.h"

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
