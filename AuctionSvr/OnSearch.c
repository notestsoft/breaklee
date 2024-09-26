#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SEARCH) {
    if (Client->AccountID < 1) goto error;

    S2C_DATA_SEARCH* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SEARCH);
    Response->Unknown1 = 0;
    Response->Unknown2 = 1;
    Response->Unknown3 = 0;
    Response->ResultCount = 10;

    for (Int32 Index = 0; Index < Response->ResultCount; Index += 1) {
        S2C_DATA_SEARCH_RESULT_SLOT* ResponseSlot = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_SEARCH_RESULT_SLOT);
        ResponseSlot->SlotIndex = Index;
        ResponseSlot->ItemID = 33556068;
        ResponseSlot->ItemOptions = 0;
        ResponseSlot->StackSize = 1;
        ResponseSlot->PriceType = 1;
        ResponseSlot->ItemOptionExtended = 1;
        ResponseSlot->Price = 1;
        ResponseSlot->NameLength = 4;
        ResponseSlot->CharacterIndex = Index;
        PacketBufferAppendCString(Connection->PacketBuffer, "test");
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
