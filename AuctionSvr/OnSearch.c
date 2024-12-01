#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

CLIENT_PROCEDURE_BINDING(SEARCH) {
    if (Client->AccountID < 1) goto error;

    IPC_A2D_DATA_SEARCH* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, A2D, SEARCH);
    Request->Header.SourceConnectionID = Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.AuctionSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB; 
    Request->CategoryIndex2 = Packet->CategoryIndex2;
    Request->CategoryIndex3 = Packet->CategoryIndex3;
    Request->CategoryIndex4 = Packet->CategoryIndex4;
    Request->CategoryIndex5 = Packet->CategoryIndex5;
    Request->SortOrder = Packet->SortOrder;
    IPCSocketUnicast(Context->IPCSocket, Request);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2A, SEARCH) {
    if (!ClientConnection) return;

    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);

    S2C_DATA_SEARCH* Response = PacketBufferInit(PacketBuffer, S2C, SEARCH);
    Response->Unknown1 = 0;
    Response->Unknown2 = 1;
    Response->Unknown3 = 0;
    Response->ResultCount = Packet->ResultCount;

    for (Int Index = 0; Index < Packet->ResultCount; Index += 1) {
        S2C_DATA_SEARCH_RESULT_SLOT* ResponseSlot = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_SEARCH_RESULT_SLOT);
        ResponseSlot->SlotIndex = Index;
        ResponseSlot->ItemID = Packet->Results[Index].ItemID;
        ResponseSlot->ItemOptions = Packet->Results[Index].ItemOptions;
        ResponseSlot->ItemOptionExtended = Packet->Results[Index].ItemOptionExtended;
        ResponseSlot->StackSize = Packet->Results[Index].StackSize;
        ResponseSlot->PriceType = Packet->Results[Index].PriceType;
        ResponseSlot->Price = Packet->Results[Index].Price;
        ResponseSlot->AccountID = Packet->Results[Index].AccountID;
        ResponseSlot->NameLength = strlen(Packet->Results[Index].CharacterName);
        PacketBufferAppendCString(PacketBuffer, Packet->Results[Index].CharacterName);
    }

    SocketSend(Context->ClientSocket, ClientConnection, Response);
}