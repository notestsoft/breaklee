#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

// TODO: Add support for warehouse inventory!
// 
// NOTE: Currently we use this shared memory because we don't do parallel processing...
static struct _RTCharacterEquipmentInfo kEquipmentInfoBackup;
static struct _RTCharacterInventoryInfo kInventoryInfoBackup;

Bool MoveInventoryItem(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 SourceStorageType,
    UInt32 SourceSlotIndex,
    UInt32 DestinationStorageType,
    UInt32 DestinationSlotIndex
) {
    // TODO: Check if this is causing an issue when the client is not initialized!
    memcpy(&kEquipmentInfoBackup, &Character->EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
    memcpy(&kInventoryInfoBackup, &Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));

    struct _RTItemSlot TempSlot = { 0 };
    if (SourceStorageType == STORAGE_TYPE_INVENTORY) {
        if (!RTInventoryRemoveSlot(Runtime, &Character->InventoryInfo, SourceSlotIndex, &TempSlot)) goto error;
    }
    else if (SourceStorageType == STORAGE_TYPE_EQUIPMENT) {
        if (!RTEquipmentRemoveSlot(Runtime, &Character->EquipmentInfo, SourceSlotIndex, &TempSlot)) goto error;
    }
    else {
        goto error;
    }

    TempSlot.SlotIndex = DestinationSlotIndex;
    if (DestinationStorageType == STORAGE_TYPE_INVENTORY) {
        if (!RTInventorySetSlot(Runtime, &Character->InventoryInfo, &TempSlot)) goto error;
    }
    else if (DestinationStorageType == STORAGE_TYPE_EQUIPMENT) {
        if (!RTEquipmentSetSlot(Runtime, &Character->EquipmentInfo, &TempSlot)) goto error;
    }
    else {
        goto error;
    }

    if (SourceStorageType == STORAGE_TYPE_INVENTORY || DestinationStorageType == STORAGE_TYPE_INVENTORY) {
        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
    }

    if (SourceStorageType == STORAGE_TYPE_EQUIPMENT || DestinationStorageType == STORAGE_TYPE_EQUIPMENT) {
        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_EQUIPMENT;
    }

    Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

    return true;

error:
    memcpy(&Character->EquipmentInfo, &kEquipmentInfoBackup, sizeof(struct _RTCharacterEquipmentInfo));
    memcpy(&Character->InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));
    return false;
}

CLIENT_PROCEDURE_BINDING(PUSH_EQUIPMENT_ITEM) {
    if (!Character) goto error;

    RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->LeftSlotIndex);
    if (!ItemSlot) goto error;

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
    if (!ItemData) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (!World) goto error;

    /*
    struct _RTItemSlot LeftSlot = { 0 };
    struct _RTItemSlot RightSlot = { 0 };

    Int32 NextSlotIndex = RTCharacterFindNextEquipmentSlotIndex(Runtime, Character, 0, Packet->LeftToRightItemType);
    if (NextSlotIndex >= 0) {
        RTEquipmentRemoveSlot(Runtime, &Character->EquipmentInfo, NextSlotIndex, &LeftSlot);

        NextSlotIndex = RTCharacterFindNextEquipmentSlotIndex(Runtime, Character, NextSlotIndex + 1, Packet->LeftToRightItemType);
    }

    while (NextSlotIndex >= 0) {
        if (!RTEquipmentRemoveSlot(Runtime, &Character->EquipmentInfo, NextSlotIndex, &LeftSlot)) break;

        S2C_DATA_NFY_UNEQUIP_ITEM* Notification = PacketInit(S2C_DATA_NFY_UNEQUIP_ITEM);
        Notification->Command = S2C_NFY_UNEQUIP_ITEM;
        Notification->CharacterIndex = Character->CharacterIndex;
        Notification->EquipmentSlotIndex = LeftSlot.SlotIndex;
        BroadcastToWorld(
            Server,
            World,
            Character->ID,
            Character->Movement.PositionCurrent.X,
            Character->Movement.PositionCurrent.Y,
            Notification
        );

        NextSlotIndex = RTCharacterFindNextEquipmentSlotIndex(Runtime, Character, NextSlotIndex + 1, Packet->LeftToRightItemType);
        if (NextSlotIndex < 0) break;

        if (!RTEquipmentRemoveSlot(Runtime, &Character->EquipmentInfo, NextSlotIndex, &RightSlot)) {
            LeftSlot.SlotIndex = NextSlotIndex;

            if (!RTEquipmentSetSlot(Runtime, &Character->EquipmentInfo, &LeftSlot)) {
                // TODO: Make a backup of equipment info and rollback
                FatalError("Internal inconsistency!");
            }

            S2C_DATA_NFY_EQUIP_ITEM* Notification = PacketInit(S2C_DATA_NFY_EQUIP_ITEM);
            Notification->Command = S2C_NFY_EQUIP_ITEM;
            Notification->CharacterIndex = Character->CharacterIndex;
            Notification->Item = LeftSlot.Item;
            Notification->ItemOptions = LeftSlot.ItemOptions;
            Notification->EquipmentSlotIndex = LeftSlot.SlotIndex;
            BroadcastToWorld(
                Server,
                World,
                Character->ID,
                Character->Movement.PositionCurrent.X,
                Character->Movement.PositionCurrent.Y,
                Notification
            );

            break;
        }

        S2C_DATA_NFY_UNEQUIP_ITEM* Notification = PacketInit(S2C_DATA_NFY_UNEQUIP_ITEM);
        Notification->Command = S2C_NFY_UNEQUIP_ITEM;
        Notification->CharacterIndex = Character->CharacterIndex;
        Notification->EquipmentSlotIndex = RightSlot.SlotIndex;
        BroadcastToWorld(
            Server,
            World,
            Character->ID,
            Character->Movement.PositionCurrent.X,
            Character->Movement.PositionCurrent.Y,
            Notification
        );


    }
    */

    S2C_DATA_PUSH_EQUIPMENT_ITEM* Response = PacketInit(S2C_DATA_PUSH_EQUIPMENT_ITEM);
    Response->Command = S2C_PUSH_EQUIPMENT_ITEM;
    Response->Result = 0;
    // TODO: Add implementation!
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MOVE_INVENTORY_ITEM) {
    if (!Character) goto error;

    S2C_DATA_MOVE_INVENTORY_ITEM* Response = PacketInit(S2C_DATA_MOVE_INVENTORY_ITEM);
    Response->Command = S2C_MOVE_INVENTORY_ITEM;
    Response->Result = MoveInventoryItem(
        Runtime,
        Character,
        Packet->Source.StorageType,
        Packet->Source.Index,
        Packet->Destination.StorageType,
        Packet->Destination.Index
    );

    if (Response->Result) {
        // TODO: We recalc all character data for simplicity now but please fix this!
        RTCharacterInitializeAttributes(Runtime, Character);
    }

    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SWAP_INVENTORY_ITEM) {
    if (!Character) {
        return SocketDisconnect(Socket, Connection);
    }

    // TODO: Check if this is causing an issue when the client is not initialized!
    memcpy(&kEquipmentInfoBackup, &Character->EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
    memcpy(&kInventoryInfoBackup, &Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));

    S2C_DATA_SWAP_INVENTORY_ITEM* Response = PacketInit(S2C_DATA_SWAP_INVENTORY_ITEM);
    Response->Command = S2C_SWAP_INVENTORY_ITEM;

    struct _RTItemSlot TempSlot = { 0 };

    // Move Source2 to TempSlot
    if (Packet->Source2.StorageType == STORAGE_TYPE_INVENTORY) {
        if (!RTInventoryRemoveSlot(Runtime, &Character->InventoryInfo, Packet->Source2.Index, &TempSlot)) goto error;
    }
    else if (Packet->Source2.StorageType == STORAGE_TYPE_EQUIPMENT) {
        if (!RTEquipmentRemoveSlot(Runtime, &Character->EquipmentInfo, Packet->Source2.Index, &TempSlot)) goto error;
    }
    else {
        goto error;
    }

    // Move Source1 to Destination1
    if (!MoveInventoryItem(
        Runtime,
        Character,
        Packet->Source1.StorageType,
        Packet->Source1.Index,
        Packet->Destination1.StorageType,
        Packet->Destination1.Index
    )) goto error;

    // Move TempSlot to Destination2
    TempSlot.SlotIndex = Packet->Destination2.Index;
    if (Packet->Destination2.StorageType == STORAGE_TYPE_INVENTORY) {
        if (!RTInventorySetSlot(Runtime, &Character->InventoryInfo, &TempSlot)) goto error;
    }
    else if (Packet->Destination2.StorageType == STORAGE_TYPE_EQUIPMENT) {
        if (!RTEquipmentSetSlot(Runtime, &Character->EquipmentInfo, &TempSlot)) goto error;
    }
    else {
        goto error;
    }

    Response->Result = 1;

    RTCharacterInitializeAttributes(Runtime, Character);

    return SocketSend(Socket, Connection, Response);

error:
    memcpy(&Character->EquipmentInfo, &kEquipmentInfoBackup, sizeof(struct _RTCharacterEquipmentInfo));
    memcpy(&Character->InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));

    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SORT_INVENTORY) {
    if (!Character) {
        return SocketDisconnect(Socket, Connection);
    }

    S2C_DATA_SORT_INVENTORY* Response = PacketInit(S2C_DATA_SORT_INVENTORY);
    Response->Command = S2C_SORT_INVENTORY;
    Response->Success = 0;
    return SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(MOVE_INVENTORY_ITEM_LIST) {
    if (!Character) {
        return SocketDisconnect(Socket, Connection);
    }

    S2C_DATA_SORT_INVENTORY* Response = PacketInit(S2C_DATA_SORT_INVENTORY);
    Response->Command = S2C_SORT_INVENTORY;
    Response->Success = 0;

    Int32 TailLength = Packet->ItemCount * sizeof(CSC_DATA_ITEM_SLOT_INDEX) * 2;
    Int32 PacketLength = sizeof(C2S_DATA_MOVE_INVENTORY_ITEM_LIST) + TailLength;
    if (Packet->Signature.Length != PacketLength) goto error;

    CSC_DATA_ITEM_SLOT_INDEX* Source = (CSC_DATA_ITEM_SLOT_INDEX*)(&Packet->Data[0]);
    CSC_DATA_ITEM_SLOT_INDEX* Destination = (CSC_DATA_ITEM_SLOT_INDEX*)(&Packet->Data[0] + Packet->ItemCount * sizeof(CSC_DATA_ITEM_SLOT_INDEX));

    for (Int32 Index = 0; Index < Packet->ItemCount; Index += 1) {
        if (Source[Index].StorageType != STORAGE_TYPE_INVENTORY) goto error;
        if (Destination[Index].StorageType != STORAGE_TYPE_INVENTORY) goto error;

        Int32 SourceIndex = Source[Index].Index;
        RTCharacterInventoryInfoRef SourceInventory = &Character->InventoryInfo;
        if (SourceIndex >= RUNTIME_INVENTORY_TOTAL_SIZE) {
            SourceIndex -= RUNTIME_INVENTORY_TOTAL_SIZE;
            SourceInventory = &Character->TemporaryInventoryInfo;
            if (SourceIndex >= RUNTIME_INVENTORY_TOTAL_SIZE) {
                goto error;
            }
        }

        Bool Success = RTInventoryMoveSlot(
            Runtime,
            SourceInventory,
            &Character->InventoryInfo,
            SourceIndex,
            Destination[Index].Index
        );

        if (!Success) goto error;
    }

    Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
    Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

    Response->Success = 1;
    return SocketSend(Socket, Connection, Response);

error:
    Response->Success = 0;
    return SocketSend(Socket, Connection, Response);
}
