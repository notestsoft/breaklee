#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_WAREHOUSE) {
    if(!Character) goto error;

    S2C_DATA_GET_WAREHOUSE* Response = PacketInit(S2C_DATA_GET_WAREHOUSE);
    Response->Command = S2C_GET_WAREHOUSE;
    Response->Count = 1;
    Response->Currency = 1234567;
    //Response->Slots 

    S2C_DATA_GET_WAREHOUSE_SLOT_INDEX* Slot = PacketAppendStruct(S2C_DATA_GET_WAREHOUSE_SLOT_INDEX);
    Slot->SlotIndex = 15;
    Slot->ItemID.ID = 3145;
    Slot->ItemID.IsAccountBinding = 0;
    Slot->ItemID.IsCharacterBinding = 0;
    Slot->ItemID.DivineLevel = 0;
    Slot->ItemID.ExtremeLevel = 0;
    Slot->ItemID.IsTradableBinding = 0;
    Slot->ItemID.PartyQuestItemID = 0;
    Slot->ItemID.PartyQuestItemStackable = 0;
    Slot->ItemID.Serial = 0;
    Slot->ItemID.UpgradeLevel = 0;
    Slot->ItemOptions = 0;

    return SocketSend(Socket, Connection, Response);
error:
	return SocketDisconnect(Socket, Connection);
}
