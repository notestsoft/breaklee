#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(L2M, GET_WORLD_LIST) {
    IPC_M2L_DATA_GET_WORLD_LIST* Response = IPCPacketBufferInit(Connection->PacketBuffer, M2L, GET_WORLD_LIST);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    Response->GroupCount = 1;

    IPC_M2L_DATA_SERVER_GROUP* ServerGroup = IPCPacketBufferAppendStruct(Connection->PacketBuffer, IPC_M2L_DATA_SERVER_GROUP);
    ServerGroup->GroupIndex = Server->IPCSocket->NodeID.Group;

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(Socket->NodeTable);
    while (Iterator.Key) {
        IPCNodeID Node = *(IPCNodeID*)Iterator.Key;
        Iterator = DictionaryKeyIteratorNext(Iterator);

        if (Node.Type != IPC_TYPE_WORLD) continue;
        if (Node.Group != Server->IPCSocket->NodeID.Group) continue;

        WorldInfoRef WorldInfo = (WorldInfoRef)DictionaryLookup(Context->WorldInfoTable, &Node);
        if (!WorldInfo) continue;

        IPC_M2L_DATA_SERVER_GROUP_NODE* ServerGroupNode = IPCPacketBufferAppendStruct(Connection->PacketBuffer, IPC_M2L_DATA_SERVER_GROUP_NODE);
        ServerGroupNode->NodeIndex = Node.Index;
        ServerGroupNode->PlayerCount = WorldInfo->PlayerCount;
        ServerGroupNode->MaxPlayerCount = WorldInfo->MaxPlayerCount;
        CStringCopySafe(ServerGroupNode->Host, 64 + 1, WorldInfo->Host);
        ServerGroupNode->Port = WorldInfo->Port;
        ServerGroupNode->Type = WorldInfo->Type;

        ServerGroup->NodeCount += 1;
    }

    IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2M, GET_WORLD_LIST) {
    IPC_M2W_DATA_GET_WORLD_LIST* Response = IPCPacketBufferInit(Connection->PacketBuffer, M2W, GET_WORLD_LIST);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(Socket->NodeTable);
    while (Iterator.Key) {
        IPCNodeID Node = *(IPCNodeID*)Iterator.Key;
        Iterator = DictionaryKeyIteratorNext(Iterator);

        if (Node.Type != IPC_TYPE_WORLD) continue;
        if (Node.Group != Context->Config.MasterSvr.GroupIndex) continue;

        WorldInfoRef WorldInfo = (WorldInfoRef)DictionaryLookup(Context->WorldInfoTable, &Node);
        if (!WorldInfo) continue;

        IPC_M2W_DATA_SERVER_GROUP_NODE* ServerGroupNode = IPCPacketBufferAppendStruct(Connection->PacketBuffer, IPC_M2W_DATA_SERVER_GROUP_NODE);
        ServerGroupNode->NodeIndex = Node.Index;
        ServerGroupNode->PlayerCount = WorldInfo->PlayerCount;
        ServerGroupNode->MaxPlayerCount = WorldInfo->MaxPlayerCount;
        CStringCopySafe(ServerGroupNode->Host, 64 + 1, WorldInfo->Host);
        ServerGroupNode->Port = WorldInfo->Port;
        ServerGroupNode->Type = WorldInfo->Type;

        Response->NodeCount += 1;
    }

    IPCSocketUnicast(Socket, Response);
}

