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
    IPC_ ## __NAMESPACE__ ## _PROC_ ## __NAME__(                        \
        Server,                                                         \
        Context,                                                        \
        Socket,                                                         \
        Connection,                                                     \
        NodeContext,                                                    \
        (IPC_ ## __NAMESPACE__ ## _DATA_ ## __NAME__ *)Packet           \
    );                                                                  \
}

#define IPC_W2D_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(W2D, __NAME__)
#include "IPCCommands.h"

Void ServerOnUpdate(
    ServerRef Server,
    Void* ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
}

Int32 main(Int32 argc, CString* argv) {
    DiagnosticSetupLogFile("MasterDBAgent", LOG_LEVEL_TRACE, NULL, NULL);

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "MasterDBAgent.ini");
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;
    ServerContext.Database = NULL;

    IPCNodeID NodeID = kIPCNodeIDNull;
    NodeID.Group = Config.MasterDBAgent.GroupIndex;
    NodeID.Type = IPC_TYPE_MASTERDB;

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

    Int64 DatabaseResultBufferSize = 0;

#define ACCOUNT_DATA_PROTOCOL(__TYPE__, __NAME__) \
    DatabaseResultBufferSize = MAX(DatabaseResultBufferSize, sizeof(__TYPE__));
    
#define CHARACTER_DATA_PROTOCOL(__TYPE__, __NAME__) \
    DatabaseResultBufferSize = MAX(DatabaseResultBufferSize, sizeof(__TYPE__));
    
#include "RuntimeLib/CharacterDataDefinition.h"

    ServerContext.Database = DatabaseConnect(
        Config.Database.Host,
        Config.Database.Username,
        Config.Database.Password,
        Config.Database.Database,
        Config.Database.Port,
        DatabaseResultBufferSize,
        Config.Database.AutoReconnect
    );
    if (!ServerContext.Database) Fatal("Database connection failed");

    MasterDBMigrate(ServerContext.Database);

#define IPC_W2D_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_W2D_ ## __NAME__, &SERVER_IPC_W2D_PROC_ ## __NAME__);
#include "IPCCommands.h"

    MasterDBPrepareStatements(ServerContext.Database);

    ServerRun(Server);
    ServerDestroy(Server);
    DatabaseDisconnect(ServerContext.Database);
    DiagnosticTeardown();

    return EXIT_SUCCESS;
}
