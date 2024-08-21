#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(BUY_SKILLBOOK) {
    if (!Character) goto error;

    RTTrainerDataRef Trainer = RTRuntimeGetTrainerByWorldNpcID(
        Runtime,
        Character->Data.Info.WorldIndex,
        Packet->NpcID
    );
    if (!Trainer) goto error;

    // TODO: Check NPC distance to character

    RTTrainerSkillDataRef Skill = NULL;
    for (Int32 Index = 0; Index < Trainer->SkillCount; Index++) {
        if (Trainer->Skills[Index].SlotID == Packet->SlotID && Trainer->Skills[Index].ID == Packet->SkillID) {
            Skill = &Trainer->Skills[Index];
        }
    }
    if (!Skill) goto error;

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Skill->SkillBookID);
    assert(ItemData);

    if (Character->Data.Info.Alz< ItemData->BuyPrice) goto error;
    
    struct _RTItemSlot ItemSlot = { 0 };
    ItemSlot.SlotIndex = Packet->InventorySlotIndex;
    ItemSlot.Item.ID = Skill->SkillBookID;
    ItemSlot.ItemOptions = Skill->Level;

    Bool Success = RTInventorySetSlot(
        Runtime,
        &Character->Data.InventoryInfo,
        &ItemSlot
    );

    if (Success) {
        Character->Data.Info.Alz-= ItemData->BuyPrice;
    }

    S2C_DATA_BUY_SKILLBOOK* Response = PacketBufferInit(Connection->PacketBuffer, S2C, BUY_SKILLBOOK);
    Response->ItemID = Skill->SkillBookID;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
