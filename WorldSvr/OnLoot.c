#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

CLIENT_PROCEDURE_BINDING(LOOT_INVENTORY_ITEM) {
    if (!Character) goto error;

    RTLootResult Result = RTCharacterLootItem(
        Runtime,
        Character,
        Packet->Entity,
        Packet->UniqueKey,
        Packet->InventorySlotIndex
    );

    S2C_DATA_LOOT_INVENTORY_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, LOOT_INVENTORY_ITEM);
    Response->Result = Result.Result;
    Response->ItemID = Result.ItemID.Serial;
    Response->ItemOptions = Result.ItemOptions;
    Response->InventorySlotIndex = Result.InventorySlotIndex;
    SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_LOOT_INVENTORY_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, LOOT_INVENTORY_ITEM);
        Response->Result = S2C_DATA_LOOT_RESULT_OWNERSHIP_ERROR;
        SocketSend(Socket, Connection, Response);
    }
}

CLIENT_PROCEDURE_BINDING(LOOT_CURRENCY_ITEM) {
    if (!Character) goto error;

    if (Packet->Length != sizeof(C2S_DATA_LOOT_CURRENCY_ITEM) + sizeof(C2S_DATA_LOOT_CURRENCY_ITEM_INDEX) * Packet->Count) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (!World) goto error;

    S2C_DATA_LOOT_CURRENCY_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, LOOT_CURRENCY_ITEM);
    Response->Result = S2C_DATA_LOOT_RESULT_SUCCESS;

    for (Int Index = 0; Index < Packet->Count; Index++) {
        C2S_DATA_LOOT_CURRENCY_ITEM_INDEX* ItemIndex = &Packet->Data[Index];
        RTWorldItemRef Item = RTWorldGetItem(Runtime, World, ItemIndex->Entity, ItemIndex->UniqueKey);
        if (!Item) continue; // NOTE: Is it fine to just continue here?

        // TODO: Check character distance to item?
        
        if (Item->Item.ID != RUNTIME_ITEM_ID_CURRENCY) continue;

        Character->Data.Info.Alz += Item->ItemOptions;
        Character->SyncMask.Info = true;

        Response->Count += 1;

        RTWorldDespawnItem(Runtime, World, Item);
    }

    Response->Currency = Character->Data.Info.Alz;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(LOOT_GROUP_ITEM) {
    if (!Character) goto error;

    Int32 PacketLength = sizeof(C2S_DATA_LOOT_GROUP_ITEM) + sizeof(C2S_DATA_LOOT_GROUP_ITEM_INDEX) * Packet->ItemCount;
    if (Packet->Length != PacketLength) goto error;

    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);

    S2C_DATA_LOOT_GROUP_ITEM* Response = PacketBufferInit(PacketBuffer, S2C, LOOT_GROUP_ITEM);
    Response->Result = 1;
    Response->ItemCount = 0;

    for (Int Index = 0; Index < Packet->ItemCount; Index += 1) {
        RTLootResult Result = RTCharacterLootItem(
            Runtime,
            Character,
            Packet->Items[Index].Entity,
            Packet->Items[Index].UniqueKey,
            Packet->Items[Index].InventorySlotIndex
        );

        if (Result.Result != S2C_DATA_LOOT_GROUP_ITEM_RESULT_SUCCESS) {
            if (Index > 0) goto error;

            Response->Result = Result.Result;
            break;
        }
        
        S2C_DATA_LOOT_GROUP_ITEM_INDEX* ResponseItem = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_LOOT_GROUP_ITEM_INDEX);
        ResponseItem->ItemID = Result.ItemID.Serial;
        ResponseItem->ItemOptions = Result.ItemOptions;
        ResponseItem->InventorySlotIndex = Result.InventorySlotIndex;
        Response->ItemCount += 1;
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_LOOT_GROUP_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, LOOT_GROUP_ITEM);
        Response->Result = S2C_DATA_LOOT_RESULT_OWNERSHIP_ERROR;
        SocketSend(Socket, Connection, Response);
    }
}
