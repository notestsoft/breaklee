#include "Context.h"
#include "MasterDB.h"
#include "IPCProcedures.h"

Void ServerOnIPCPacketReceived(
    ServerRef Server,
    Void* ServerContext,
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    Void* ConnectionContext,
    IPCPacketRef Packet
) {
    assert(Packet->SubCommand < IPC_COMMAND_COUNT);

    ServerContextRef Context = (ServerContextRef)ServerContext;
    IPCProcedureCallback* Callback = (IPCProcedureCallback*)MemoryPoolFetch(Context->IPCCallbackPool, Packet->SubCommand);
    IPCNodeContextRef NodeContext = (IPCNodeContextRef)ConnectionContext;

    if (Callback) (*Callback)(
        Server,
        Context,
        Socket,
        Connection,
        NodeContext,
        Packet
    );
}

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
    ServerContext.IPCCallbackPool = MemoryPoolCreate(Allocator, sizeof(IPCProcedureCallback), IPC_COMMAND_COUNT);
    ServerContext.WorldInfoTable = IndexDictionaryCreate(Allocator, Config.MasterSvr.MaxWorldCount);

#define IPC_L2M_COMMAND(__NAME__) \
    { \
        IPCProcedureCallback* Callback = (IPCProcedureCallback*)MemoryPoolReserve(ServerContext.IPCCallbackPool, IPC_L2M_ ## __NAME__); \
        *Callback = &IPC_L2M_PROC_ ## __NAME__; \
    }
#include "IPCCommands.h"

#define IPC_W2M_COMMAND(__NAME__) \
    { \
        IPCProcedureCallback* Callback = (IPCProcedureCallback*)MemoryPoolReserve(ServerContext.IPCCallbackPool, IPC_W2M_ ## __NAME__); \
        *Callback = &IPC_W2M_PROC_ ## __NAME__; \
    }
#include "IPCCommands.h"

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
        &ServerOnIPCPacketReceived,
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

    MasterDBPrepareStatements(ServerContext.Database);

    ServerRun(Server);
    ServerDestroy(Server);
    DatabaseDisconnect(ServerContext.Database);
    MemoryPoolDestroy(ServerContext.IPCCallbackPool);
    DictionaryDestroy(ServerContext.WorldInfoTable);
    DiagnosticTeardown();

    return EXIT_SUCCESS;
}
