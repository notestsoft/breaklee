#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ADD_CHARACTER_STATS) {
	if (!Character) goto error;

    if (Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR] != Packet->Stat[RUNTIME_CHARACTER_STAT_STR] ||
        Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] != Packet->Stat[RUNTIME_CHARACTER_STAT_DEX] ||
        Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT] != Packet->Stat[RUNTIME_CHARACTER_STAT_INT]) {
        goto error;
    }

    Bool Success = RTCharacterAddStats(Runtime, Character, (Int32*)Packet->StatDelta);
    
    S2C_DATA_ADD_CHARACTER_STATS* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ADD_CHARACTER_STATS);
    Response->Result = Success ? 0 : 1;
    Response->Stat[RUNTIME_CHARACTER_STAT_STR] = Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR];
    Response->Stat[RUNTIME_CHARACTER_STAT_DEX] = Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX];
    Response->Stat[RUNTIME_CHARACTER_STAT_INT] = Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT];
	SocketSend(Socket, Connection, Response);
    return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(REMOVE_CHARACTER_STATS) {
	if (!Character) goto error;

    Int32 PacketTailLength = (
        Packet->StatFlag[RUNTIME_CHARACTER_STAT_STR] ? sizeof(UInt16) : 0 +
        Packet->StatFlag[RUNTIME_CHARACTER_STAT_DEX] ? sizeof(UInt16) : 0 +
        Packet->StatFlag[RUNTIME_CHARACTER_STAT_INT] ? sizeof(UInt16) : 0
    );

    if (Packet->Length != sizeof(C2S_DATA_REMOVE_CHARACTER_STATS) + PacketTailLength) goto error;

    if (Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR] != Packet->Stat[RUNTIME_CHARACTER_STAT_STR] ||
        Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] != Packet->Stat[RUNTIME_CHARACTER_STAT_DEX] ||
        Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT] != Packet->Stat[RUNTIME_CHARACTER_STAT_INT]) {
        goto error;
    }

    Int32 StatIndices[] = {
        RUNTIME_CHARACTER_STAT_STR,
        RUNTIME_CHARACTER_STAT_DEX,
        RUNTIME_CHARACTER_STAT_INT,
    };
    Int32 StatTypes[] = {
        RUNTIME_ITEM_POTION_STAT_TYPE_STR,
        RUNTIME_ITEM_POTION_STAT_TYPE_DEX,
        RUNTIME_ITEM_POTION_STAT_TYPE_INT,
    };
    Int32 SlotIndex = 0;
    for (Int Index = 0; Index < 3; Index += 1) {
        if (Packet->StatFlag[Index]) {
            Int32 StatIndex = StatIndices[Index];
            Int32 InventorySlotIndex = Packet->InventorySlots[SlotIndex];
            SlotIndex += 1;

            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
            if (!ItemSlot) goto error;

            RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
            if (!ItemData) goto error;

            if (ItemData->ItemType != RUNTIME_ITEM_TYPE_POTION) goto error;
            if (ItemData->Potion.PotionType != RUNTIME_ITEM_SUBTYPE_POTION_STAT) goto error;
            if (ItemData->Potion.StatType != StatTypes[Index]) goto error;
            if (ItemData->MaxStackSize < 1) goto error;
            // NOTE: This check will cause issues if you fake items with unallowed sizes..
            // if (ItemData->MaxStackSize < Packet->StatDelta[Index]) goto error;
            if (ItemSlot->ItemOptions < Packet->StatDelta[Index]) goto error;

            ItemSlot->ItemOptions -= Packet->StatDelta[Index];
            if (ItemSlot->ItemOptions < 1) {
                RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
            }

            Bool Success = RTCharacterRemoveStat(Runtime, Character, StatIndex, Packet->StatDelta[Index]);
            // TODO: We should roll back on errors!
        }
    }

    S2C_DATA_REMOVE_CHARACTER_STATS* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REMOVE_CHARACTER_STATS);
    Response->Result = 0;
    Response->Stat[RUNTIME_CHARACTER_STAT_STR] = Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR];
    Response->Stat[RUNTIME_CHARACTER_STAT_DEX] = Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX];
    Response->Stat[RUNTIME_CHARACTER_STAT_INT] = Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT];
	SocketSend(Socket, Connection, Response);
    return;

error:
	SocketDisconnect(Socket, Connection);
}
