#include "Context.h"
#include "MasterDB.h"
#include "IPCProcedures.h"

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

#define IPC_L2M_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(L2M, __NAME__)
#include "IPCCommands.h"

#define IPC_W2M_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(W2M, __NAME__)
#include "IPCCommands.h"

Void ServerOnUpdate(
    ServerRef Server,
    Void* ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
}

Int32 main(Int32 argc, CString* argv) {
    DiagnosticCreateLogFile("MasterSvr");

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "MasterSvr.ini");
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;
    ServerContext.Database = NULL;
    ServerContext.WorldInfoTable = IndexDictionaryCreate(Allocator, Config.MasterSvr.MaxWorldCount);

    IPCNodeID NodeID = kIPCNodeIDNull;
    NodeID.Group = Config.MasterSvr.GroupIndex;
    NodeID.Type = IPC_TYPE_MASTER;

    ServerRef Server = ServerCreate(
        Allocator,
        NodeID,
        NULL,
        Config.MasterSvr.Port,
        0,
        Config.NetLib.ReadBufferSize,
        Config.NetLib.WriteBufferSize,
        &ServerOnUpdate,
        &ServerContext
    );

    ServerContext.Database = DatabaseConnect(
        Config.MasterDB.Host,
        Config.MasterDB.Username,
        Config.MasterDB.Password,
        Config.MasterDB.Database,
        Config.MasterDB.Port,
        Config.MasterDB.AutoReconnect
    );
    if (!ServerContext.Database) FatalError("Database connection failed");

#define IPC_L2M_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_L2M_ ## __NAME__, &SERVER_IPC_L2M_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_W2M_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_W2M_ ## __NAME__, &SERVER_IPC_W2M_PROC_ ## __NAME__);
#include "IPCCommands.h"

    MasterDBPrepareStatements(ServerContext.Database);

    ServerRun(Server);
    ServerDestroy(Server);
    DatabaseDisconnect(ServerContext.Database);
    DictionaryDestroy(ServerContext.WorldInfoTable);
    DiagnosticTeardown();

    return EXIT_SUCCESS;
}
