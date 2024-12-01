#include "Context.h"
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

#define IPC_N2M_COMMAND(__NAME__) IPC_COMMAND_CALLBACK(N2M, __NAME__)
#include "IPCCommands.h"

Void ServerOnUpdate(
    ServerRef Server,
    Void* ServerContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
}

Int32 main(Int32 argc, CString* argv) {
    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "MasterSvr.ini");
    ServerConfig Config = ServerConfigLoad(ConfigFilePath);

    DiagnosticSetupLogFile(argv[0], Config.MasterSvr.LogLevel, NULL, NULL);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    struct _ServerContext ServerContext = { 0 };
    ServerContext.Config = Config;
    ServerContext.WorldInfoTable = IPCNodeIDDictionaryCreate(Allocator, Config.MasterSvr.MaxWorldCount);
    ServerContext.ClientInfoTable = IndexDictionaryCreate(Allocator, 4096);

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
        Config.NetLib.LogPackets,
        &ServerOnUpdate,
        &ServerContext
    );

#define IPC_L2M_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_L2M_ ## __NAME__, &SERVER_IPC_L2M_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_W2M_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_W2M_ ## __NAME__, &SERVER_IPC_W2M_PROC_ ## __NAME__);
#include "IPCCommands.h"

#define IPC_N2M_COMMAND(__NAME__) \
    IPCSocketRegisterCommandCallback(Server->IPCSocket, IPC_N2M_ ## __NAME__, &SERVER_IPC_N2M_PROC_ ## __NAME__);
#include "IPCCommands.h"

    ServerRun(Server);
    ServerDestroy(Server);
    DictionaryDestroy(ServerContext.WorldInfoTable);
    DictionaryDestroy(ServerContext.ClientInfoTable);

    return EXIT_SUCCESS;
}
