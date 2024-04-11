#include "Context.h"
#include "Server.h"
#include "ClientSocket.h"
#include "WorldSocket.h"
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
}

Int32 main(Int32 argc, CString* argv) {
    DiagnosticCreateLogFile("ChatSvr");

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "ChatSvr.ini");
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;

    ServerRef Server = ServerCreate(
        Allocator,
        &ServerOnUpdate,
        &ServerContext
    );
    
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
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect
    );
    
#define C2S_COMMAND(__NAME__, __COMMAND__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.ClientSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__);
#include "ClientCommands.h"
    
    ServerContext.WorldSocket = ServerCreateSocket(
        Server,
        SOCKET_FLAGS_IPC,
        NULL,
        Config.ChatSvr.Port,
        sizeof(struct _WorldContext),
        Config.NetLib.ProtocolIdentifier,
        Config.NetLib.ProtocolVersion,
        Config.NetLib.ProtocolExtension,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        Config.ChatSvr.MaxConnectionCount,
        &WorldSocketOnConnect,
        &WorldSocketOnDisconnect
    );
    /*
#define IPC_MASTER_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.MasterSocket, __COMMAND__, &SERVER_ ## __NAME__);
#include "IPCProcDefinition.h"
    */
    
    ServerRun(Server);

    RTPartyManagerDestroy(ServerContext.PartyManager);
    DiagnosticTeardown();
    
    return EXIT_SUCCESS;
}
