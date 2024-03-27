#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(OPEN_GIFTBOX) {
    if (!Character) goto error;

	PacketLogBytes(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Packet
    );
    
    S2C_DATA_OPEN_GIFTBOX* Response = PacketBufferInit(Connection->PacketBuffer, S2C, OPEN_GIFTBOX);
    Response->Count = 1;
    
    S2C_DATA_OPEN_GIFTBOX_ITEM* Item = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_OPEN_GIFTBOX_ITEM);
    RTItem ItemID;
    ItemID.ID = 33556376;
    ItemID.UpgradeLevel = 20;
    ItemID.DivineLevel = 15;
    ItemID.ExtremeLevel = 7;
    ItemID.IsCharacterBinding = 1;
    Item->ItemID = ItemID.Serial;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(OPEN_GIFTBOX_UNKNOWN) {
    if (!Character) goto error;

    PacketLogBytes(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Packet
    );

    S2C_DATA_OPEN_GIFTBOX_UNKNOWN* Response = PacketBufferInit(Connection->PacketBuffer, S2C, OPEN_GIFTBOX_UNKNOWN);
    Response->Count = Packet->Count;
    Response->Unknown[16] = 1;
    Response->Unknown[33] = Packet->Data[0].InventorySlotIndex;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}
