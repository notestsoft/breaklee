#include "Context.h"
#include "ClientProtocol.h"
#include "ClientProcedures.h"

#define S2C_COMMAND(__NAME__, __COMMAND__)                                                  \
Void SERVER_PROC_ ## __NAME__ ## _LOGIN(                                                    \
    ServerRef Server,                                                                       \
    Void *ServerContext,                                                                    \
    SocketRef Socket,                                                                       \
    SocketConnectionRef Connection,                                                         \
    Void *ConnectionContext,                                                                \
    Void *Packet                                                                            \
) {                                                                                         \
    ServerContextRef Context = (ServerContextRef)ServerContext; \
    PROC_ ## __NAME__ ## _LOGIN(Server, Context, Socket, Connection, (S2C_DATA_ ## __NAME__ ## _LOGIN*)Packet);\
}
#include "LoginCommands.h"

#define S2C_COMMAND(__NAME__, __COMMAND__)                                                  \
Void SERVER_PROC_ ## __NAME__ ## _WORLD(                                                    \
    ServerRef Server,                                                                       \
    Void *ServerContext,                                                                    \
    SocketRef Socket,                                                                       \
    SocketConnectionRef Connection,                                                         \
    Void *ConnectionContext,                                                                \
    Void *Packet                                                                            \
) {                                                                                         \
    ServerContextRef Context = (ServerContextRef)ServerContext; \
    PROC_ ## __NAME__ ## _WORLD(Server, Context, Socket, Connection, (S2C_DATA_ ## __NAME__ ## _WORLD*)Packet);\
}
#include "WorldCommands.h"

Void ServerOnConnectLogin(
    ServerRef Server,
    Void* ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void* ConnectionContext
) {
    C2S_DATA_CONNECT_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, CONNECT_LOGIN);
    SocketSend(Socket, Connection, Request);
}

Void ServerOnUpdate(
    ServerRef Server,
    Void* ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    if (Context->WorldSocket == NULL && !Context->IsLoggingIn) {
        Context->IsLoggingIn = true;
        // SocketConnect(Context->LoginSocket, "127.0.0.1", 38101, 10000);
    }
}

Int32 main(Int32 ArgumentCount, CString* Arguments) {
    DiagnosticSetup(
        stdout,
        LOG_LEVEL_TRACE,
        NULL,
        NULL
    );

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    IPCNodeID NodeID = kIPCNodeIDNull;
    struct _ServerContext ServerContext = { 0 };
    CStringCopySafe(ServerContext.Username, MAX_PATH, "test");
    CStringCopySafe(ServerContext.Password, MAX_PATH, "test");
    ServerRef Server = ServerCreate(
        Allocator,
        NodeID,
        NULL,
        12345,
        0,
        0x1FFFF,
        0x1FFFF,
        &ServerOnUpdate,
        &ServerContext
    );

    ServerContext.LoginSocket = ServerCreateSocket(
        Server,
        SOCKET_FLAGS_CLIENT | SOCKET_FLAGS_ENCRYPTED,
        "127.0.0.1",
        38101,
        sizeof(struct _ClientContext),
        0xB7D9,
        0x0009,
        0x1111,
        0x1FFFF,
        0x7FFFF,
        1,
        1,
        ServerOnConnectLogin,
        NULL
    );

#define S2C_COMMAND(__NAME__, __COMMAND__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.LoginSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__ ## _LOGIN);
#include "LoginCommands.h"
    /*
#define S2C_COMMAND(__NAME__, __COMMAND__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.WorldSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__ ## _WORLD);
#include "WorldCommands.h"
    */
    ServerRun(Server);

    EncryptionUnloadLibrary();
    DiagnosticTeardown();

    return EXIT_SUCCESS;
}
