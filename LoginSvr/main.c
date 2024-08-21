#include "Context.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Server.h"

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

#define IPC_M2L_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(M2L, __NAME__)
#include "IPCCommands.h"

#define IPC_W2L_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(W2L, __NAME__)
#include "IPCCommands.h"

Void ServerOnUpdate(
    ServerRef Server,
    Void *ServerContext
) {
    Timestamp CurrentTimestamp = GetTimestampMs();

    ServerContextRef Context = (ServerContextRef)ServerContext;
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client->Flags & CLIENT_FLAGS_CHECK_DISCONNECT_TIMER) {
            if (Client->DisconnectTimestamp < CurrentTimestamp) {
                Client->Flags &= ~CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
                SocketDisconnect(Context->ClientSocket, Connection);
                continue;
            }
        }
    }

    if (Context->WorldListBroadcastTimestamp < CurrentTimestamp) {
        Context->WorldListBroadcastTimestamp = CurrentTimestamp + Context->Config.Login.WorldListBroadcastInterval;
        IPC_L2M_DATA_GET_WORLD_LIST* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, L2M, GET_WORLD_LIST);
        Request->Header.Source = Server->IPCSocket->NodeID;
        Request->Header.Target.Group = Server->IPCSocket->NodeID.Group;
        Request->Header.Target.Type = IPC_TYPE_MASTER;
        IPCSocketUnicast(Server->IPCSocket, Request);
    }
}

Void ContextAddCaptchaFile(
    CString FileName,
    FileRef File,
    Void* UserData
) {
    ServerContextRef Context = (ServerContextRef)UserData;
    CaptchaInfoRef Captcha = (CaptchaInfoRef)ArrayAppendUninitializedElement(Context->CaptchaInfoList);

    CStringCopySafe(Captcha->Name, MAX_PATH, FileName);

    CString ExtensionOffset = strstr(FileName, ".jpg");
    if (ExtensionOffset != NULL) {
        Int32 CaptchaLength = (Int32)(ExtensionOffset - FileName);
        Captcha->Name[CaptchaLength] = '\0';
    }

    if (!FileRead(File, (UInt8**)&Captcha->Data, &Captcha->DataLength)) {
        Fatal("Couldn't load captcha file content '%s'", FileName);
    }
}

Int32 main(Int32 argc, CString* argv) {
    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "LoginSvr.ini");
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    DiagnosticSetupLogFile(argv[0], Config.Login.LogLevel, NULL, NULL);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Allocator = Allocator;
    ServerContext.Config = Config;
    ServerContext.ClientSocket = NULL;
    ServerContext.Database = NULL;
    ServerContext.WorldListBroadcastTimestamp = 0;
    ServerContext.WorldServerTable = IndexDictionaryCreate(Allocator, 256);
    ServerContext.CaptchaInfoList = ArrayCreateEmpty(Allocator, sizeof(struct _CaptchaInfo), 8);

    if (Config.Login.CaptchaVerificationEnabled) {
        FilesProcess(
            Config.Login.CaptchaDataPath,
            "*.jpg",
            ContextAddCaptchaFile,
            &ServerContext
        );
    }

    IPCNodeID NodeID = kIPCNodeIDNull;
    NodeID.Group = Config.Login.GroupIndex;
    NodeID.Index = 0;
    NodeID.Type = IPC_TYPE_LOGIN;

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

    ServerContext.ClientSocket = ServerCreateSocket(
        Server,
        (Config.NetLib.UseEncryption) ? SOCKET_FLAGS_LISTENER | SOCKET_FLAGS_ENCRYPTED : SOCKET_FLAGS_LISTENER,
        NULL,
        Config.Login.Port,
        sizeof(struct _ClientContext),
        Config.NetLib.ProtocolIdentifier,
        Config.NetLib.ProtocolVersion,
        Config.NetLib.ProtocolExtension,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        Config.Login.MaxConnectionCount,
        Config.NetLib.LogPackets,
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect
    );

#define C2S_COMMAND(__NAME__, __COMMAND__) \
    ServerSocketRegisterPacketCallback(Server, ServerContext.ClientSocket, __COMMAND__, &SERVER_PROC_ ## __NAME__);
#include "ClientCommands.h"

#define IPC_M2L_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_M2L_ ## __NAME__, &SERVER_IPC_M2L_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_W2L_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_W2L_ ## __NAME__, &SERVER_IPC_W2L_PROC_ ## __NAME__);
#include "IPCCommands.h"

    ServerContext.Database = DatabaseConnect(
        Allocator,
        Config.Database.Driver,
        Config.Database.Host,
        Config.Database.Database,
        Config.Database.Username,
        Config.Database.Password,
        Config.Database.Port,
        0,
        Config.Database.AutoReconnect
    );
    if (!ServerContext.Database) Fatal("Database connection failed");

    ServerLoadMigrationData(Config, &ServerContext);

    ServerRun(Server);
    ServerDestroy(Server);
    DatabaseDisconnect(ServerContext.Database);

    for (Index Index = 0; Index < ArrayGetElementCount(ServerContext.CaptchaInfoList); Index += 1) {
        CaptchaInfoRef Captcha = (CaptchaInfoRef)ArrayGetElementAtIndex(ServerContext.CaptchaInfoList, Index);
        free(Captcha->Data);
    }

    ArrayDestroy(ServerContext.CaptchaInfoList);

    DiagnosticTeardown();

    return EXIT_SUCCESS;
}
