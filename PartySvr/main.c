#include "Context.h"
#include "Server.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
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

Void ServerOnUpdate(
    ServerRef Server,
    Void *ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    // RTPartyManagerUpdate(Context->PartyManager);
}

Int32 main(Int32 argc, CString* argv) {
    DiagnosticCreateLogFile("PartySvr");

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "PartySvr.ini");
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;
    // ServerContext.PartyManager = RTPartyManagerCreate(Allocator, Config.PartySvr.MaxPartyCount);

    IPCNodeID NodeID = kIPCNodeIDNull;
    NodeID.Group = 1;
    NodeID.Index = 1;
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
        &ServerOnIPCPacketReceived,
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
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect
    );
    
#define C2S_COMMAND(__NAME__, __COMMAND__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.ClientSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__);
#include "ClientCommands.h"

    ServerRun(Server);

    // RTPartyManagerDestroy(ServerContext.PartyManager);
    DiagnosticTeardown();
    
    return EXIT_SUCCESS;
}
