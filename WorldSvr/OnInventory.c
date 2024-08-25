#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "Notification.h"
#include "Server.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

// TODO: Add support for warehouse inventory!
// 
// NOTE: Currently we use this shared memory because we don't do parallel processing...
static struct _RTCharacterEquipmentData kEquipmentInfoBackup;
static struct _RTCharacterInventoryInfo kInventoryInfoBackup;
static struct _RTCharacterWarehouseInfo kWarehouseInfoBackup;
static struct _RTCharacterVehicleInventoryInfo kVehicleInventoryInfoBackup;

Bool MoveInventoryItem(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 SourceStorageType,
    UInt32 SourceSlotIndex,
    UInt32 DestinationStorageType,
    UInt32 DestinationSlotIndex
) {
    // TODO: Check if this is causing an issue when the client is not initialized!
    memcpy(&kEquipmentInfoBackup, &Character->Data.EquipmentInfo, sizeof(struct _RTCharacterEquipmentData));
    memcpy(&kInventoryInfoBackup, &Character->Data.InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
    memcpy(&kWarehouseInfoBackup, &Character->Data.WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));
    memcpy(&kVehicleInventoryInfoBackup, &Character->Data.VehicleInventoryInfo, sizeof(struct _RTCharacterVehicleInventoryInfo));

    struct _RTItemSlot TempSlot = { 0 };
    if (SourceStorageType == STORAGE_TYPE_INVENTORY) {
        if (!RTInventoryRemoveSlot(Runtime, &Character->Data.InventoryInfo, SourceSlotIndex, &TempSlot)) goto error;
    }
    else if (SourceStorageType == STORAGE_TYPE_EQUIPMENT) {
        if (!RTEquipmentRemoveSlot(Runtime, Character, &Character->Data.EquipmentInfo, SourceSlotIndex, &TempSlot)) goto error;

        if (TempSlot.SlotIndex == RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT && DestinationStorageType != STORAGE_TYPE_EQUIPMENT) {
            RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, TempSlot.Item.ID);
            Bool IsOneHandedWeapon = (
                ItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_CHAKRAM
            );
            if (ItemData && IsOneHandedWeapon) {
                struct _RTItemSlot TempSlot2 = { 0 };
                if (RTEquipmentRemoveSlot(Runtime, Character, &Character->Data.EquipmentInfo, RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_RIGHT, &TempSlot2)) {
                    TempSlot2.SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT;
                    RTEquipmentSetSlot(Runtime, Character, &Character->Data.EquipmentInfo, &TempSlot2);
                }
            }
        }
    }
    else if (SourceStorageType == STORAGE_TYPE_WAREHOUSE) {
		if (!RTWarehouseRemoveSlot(Runtime, &Character->Data.WarehouseInfo, SourceSlotIndex, &TempSlot)) goto error;
	}
    else if (SourceStorageType == STORAGE_TYPE_VEHICLE_INVENTORY) {
        if (!RTVehicleInventoryRemoveSlot(Runtime, &Character->Data.VehicleInventoryInfo, SourceSlotIndex, &TempSlot)) goto error;
    }
    else {
        goto error;
    }

    TempSlot.SlotIndex = DestinationSlotIndex;
    if (DestinationStorageType == STORAGE_TYPE_INVENTORY) {
        if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &TempSlot)) goto error;
    }
    else if (DestinationStorageType == STORAGE_TYPE_EQUIPMENT) {
        if (!RTEquipmentSetSlot(Runtime, Character, &Character->Data.EquipmentInfo, &TempSlot)) goto error;
    }
    else if (DestinationStorageType == STORAGE_TYPE_WAREHOUSE) {
        if (Character->Data.Info.Alz < 1000 && Character->Data.WarehouseInfo.Info.Currency < 1000) {
            goto error;
        }

        if (!RTWarehouseSetSlot(Runtime, &Character->Data.WarehouseInfo, &TempSlot)) {
            goto error;
        }
        
        if (Character->Data.Info.Alz >= 1000) {
            Character->Data.Info.Alz -= 1000;
            Character->SyncMask.Info = true;
        }
        else {
            Character->Data.WarehouseInfo.Info.Currency -= 1000;
            Character->SyncMask.WarehouseInfo = true;
        }
    }
    else if (DestinationStorageType == STORAGE_TYPE_VEHICLE_INVENTORY) {
        RTItemSlotRef VehicleSlot = RTEquipmentGetSlot(Runtime, &Character->Data.EquipmentInfo, EQUIPMENT_SLOT_INDEX_VEHICLE);
        if (!VehicleSlot) goto error;

        RTItemDataRef VehicleData = RTRuntimeGetItemDataByIndex(Runtime, VehicleSlot->Item.ID);
        if (!VehicleData) goto error;
        if (VehicleData->ItemType != RUNTIME_ITEM_TYPE_VEHICLE_BIKE) goto error;

        if (!RTVehicleInventorySetSlot(Runtime, &Character->Data.VehicleInventoryInfo, &TempSlot)) goto error;
    }
    else {
        goto error;
    }

    if (SourceStorageType == STORAGE_TYPE_INVENTORY || DestinationStorageType == STORAGE_TYPE_INVENTORY) {
        Character->SyncMask.InventoryInfo = true;
    }

    if (SourceStorageType == STORAGE_TYPE_EQUIPMENT || DestinationStorageType == STORAGE_TYPE_EQUIPMENT) {
        Character->SyncMask.EquipmentInfo = true;
    }

    if (SourceStorageType == STORAGE_TYPE_WAREHOUSE || DestinationStorageType == STORAGE_TYPE_WAREHOUSE) {
        Character->SyncMask.WarehouseInfo = true;
    }

    if (SourceStorageType == STORAGE_TYPE_VEHICLE_INVENTORY || DestinationStorageType == STORAGE_TYPE_VEHICLE_INVENTORY) {
        Character->SyncMask.VehicleInventoryInfo = true;
    }

    return true;

error:
    memcpy(&Character->Data.EquipmentInfo, &kEquipmentInfoBackup, sizeof(struct _RTCharacterEquipmentData));
    memcpy(&Character->Data.InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));
    memcpy(&Character->Data.WarehouseInfo, &kWarehouseInfoBackup, sizeof(struct _RTCharacterWarehouseInfo));
    memcpy(&Character->Data.VehicleInventoryInfo, &kVehicleInventoryInfoBackup, sizeof(struct _RTCharacterVehicleInventoryInfo));
    return false;
}

Bool CanSwapInventoryItem(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SourceSlotIndex,
    Int32 TargetSlotIndex
) {
    RTItemSlotRef SourceSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, SourceSlotIndex);
    RTItemSlotRef TargetSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, TargetSlotIndex);
    if (!SourceSlot || !TargetSlot) return false;

    return true;
}

Bool SwapInventoryItem(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SourceSlotIndex,
    Int32 TargetSlotIndex
) {
    RTItemSlotRef SourceSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, SourceSlotIndex);
    RTItemSlotRef TargetSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, TargetSlotIndex);
    if (!SourceSlot || !TargetSlot) return false;

    SourceSlot->SlotIndex = TargetSlotIndex;
    TargetSlot->SlotIndex = SourceSlotIndex;

    return true;
}

// TODO: Add proper error handling and unwind!
CLIENT_PROCEDURE_BINDING(PUSH_EQUIPMENT_ITEM) {
    if (!Character) goto error;

    Info("PushEquipment:");

    RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->LeftSlotIndex);
    if (!ItemSlot) goto error;

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
    if (!ItemData) goto error;

    Int32 ItemType = ItemData->ItemType;
    // if (ItemData->ItemType != Packet->LeftToRightItemType) goto error;

    struct _RTItemSlot LeftSlot = { 0 };
    struct _RTItemSlot RightSlot = { 0 };
    
    Bool HasLeftSlot = false;
    Bool HasRightSlot = false;

    Int32 FirstSlotIndex = RTCharacterFindNextEquipmentSlotIndex(Runtime, Character, 0, ItemType);
    if (FirstSlotIndex < 0) goto error;

    Int32 NextSlotIndex = FirstSlotIndex;
    while (NextSlotIndex >= 0) {
        Int32 RightSlotIndex = NextSlotIndex;
        if (!HasLeftSlot) {
            HasLeftSlot = RTEquipmentRemoveSlot(Runtime, Character, &Character->Data.EquipmentInfo, NextSlotIndex, &LeftSlot);
            if (HasLeftSlot) Info("[E%d] -> [T1]", LeftSlot.SlotIndex);
            RightSlotIndex = RTCharacterFindNextEquipmentSlotIndex(Runtime, Character, NextSlotIndex + 1, ItemType);
        }
        if (!HasLeftSlot) break;

        HasRightSlot = RTEquipmentRemoveSlot(Runtime, Character, &Character->Data.EquipmentInfo, RightSlotIndex, &RightSlot);
        if (HasRightSlot) Info("[E%d] -> [T2]", RightSlot.SlotIndex);
        NextSlotIndex = RTCharacterFindNextEquipmentSlotIndex(Runtime, Character, RightSlotIndex + 1, ItemType);

        if (HasLeftSlot && RightSlotIndex >= 0) {
            LeftSlot.SlotIndex = RightSlotIndex;
            Bool Success = RTEquipmentSetSlot(Runtime, Character, &Character->Data.EquipmentInfo, &LeftSlot);
            assert(Success);
            Info("[T1] -> [E%d]", LeftSlot.SlotIndex);
            memcpy(&LeftSlot, &RightSlot, sizeof(struct _RTItemSlot));
            HasLeftSlot = HasRightSlot;
            HasRightSlot = false;
            Info("[T2] -> [T1]");
        }
    }

    Bool Success = RTInventoryRemoveSlot(Runtime, &Character->Data.InventoryInfo, Packet->LeftSlotIndex, &RightSlot);
    assert(Success);

    Info("[I%d] -> [T2]", RightSlot.SlotIndex);

    if (HasLeftSlot) {
        LeftSlot.SlotIndex = Packet->RightSlotIndex;
        Bool Success = RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &LeftSlot);
        assert(Success);
        Info("[T1] -> [I%d]", LeftSlot.SlotIndex);
    }

    RightSlot.SlotIndex = FirstSlotIndex;
    Success = RTEquipmentSetSlot(Runtime, Character, &Character->Data.EquipmentInfo, &RightSlot);
    assert(Success);
    Info("[T2] -> [E%d]", RightSlot.SlotIndex);

    Character->SyncMask.EquipmentInfo = true;
    Character->SyncMask.InventoryInfo = true;

    RTCharacterInitializeAttributes(Runtime, Character);

    S2C_DATA_PUSH_EQUIPMENT_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PUSH_EQUIPMENT_ITEM);
    Response->Result = 1;
    SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_PUSH_EQUIPMENT_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PUSH_EQUIPMENT_ITEM);
        Response->Result = 0;
        SocketSend(Socket, Connection, Response);
        return;
    }
}

CLIENT_PROCEDURE_BINDING(LOCK_EQUIPMENT) {
    if (!Character) goto error;

    if (!RTCharacterEquipmentSetLocked(Runtime, Character, Packet->EquipmentSlotIndex, Packet->IsLocked)) goto error;

    S2C_DATA_LOCK_EQUIPMENT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, LOCK_EQUIPMENT);
    Response->Success = 1;
    SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_LOCK_EQUIPMENT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, LOCK_EQUIPMENT);
        Response->Success = 0;
        SocketSend(Socket, Connection, Response);
    }
}

CLIENT_PROCEDURE_BINDING(MOVE_INVENTORY_ITEM) {
    if (!Character) goto error;

    S2C_DATA_MOVE_INVENTORY_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, MOVE_INVENTORY_ITEM);
    Response->Result = MoveInventoryItem(
        Runtime,
        Character,
        Packet->Source.StorageType,
        Packet->Source.Index,
        Packet->Destination.StorageType,
        Packet->Destination.Index
    );

    if (Response->Result && (Packet->Source.StorageType == STORAGE_TYPE_EQUIPMENT || Packet->Destination.StorageType == STORAGE_TYPE_EQUIPMENT)) {
        RTCharacterInitializeAttributes(Runtime, Character);
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SWAP_INVENTORY_ITEM) {
    if (!Character) {
        SocketDisconnect(Socket, Connection);
        return;
    }

    // TODO: Check if this is causing an issue when the client is not initialized!
    memcpy(&kEquipmentInfoBackup, &Character->Data.EquipmentInfo, sizeof(struct _RTCharacterEquipmentData));
    memcpy(&kInventoryInfoBackup, &Character->Data.InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));

    S2C_DATA_SWAP_INVENTORY_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SWAP_INVENTORY_ITEM);

    struct _RTItemSlot TempSlot = { 0 };

    // Move Source2 to TempSlot
    if (Packet->Source2.StorageType == STORAGE_TYPE_INVENTORY) {
        if (!RTInventoryRemoveSlot(Runtime, &Character->Data.InventoryInfo, Packet->Source2.Index, &TempSlot)) goto error;
    }
    else if (Packet->Source2.StorageType == STORAGE_TYPE_EQUIPMENT) {
        if (!RTEquipmentRemoveSlot(Runtime, Character, &Character->Data.EquipmentInfo, Packet->Source2.Index, &TempSlot)) goto error;
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
        if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &TempSlot)) goto error;
    }
    else if (Packet->Destination2.StorageType == STORAGE_TYPE_EQUIPMENT) {
        if (!RTEquipmentSetSlot(Runtime, Character, &Character->Data.EquipmentInfo, &TempSlot)) goto error;
    }
    else {
        goto error;
    }

    Response->Result = 1;

    if (Packet->Source1.StorageType == STORAGE_TYPE_EQUIPMENT ||
        Packet->Source2.StorageType == STORAGE_TYPE_EQUIPMENT ||
        Packet->Destination1.StorageType == STORAGE_TYPE_EQUIPMENT ||
        Packet->Destination2.StorageType == STORAGE_TYPE_EQUIPMENT) { 
        RTCharacterInitializeAttributes(Runtime, Character);
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    memcpy(&Character->Data.EquipmentInfo, &kEquipmentInfoBackup, sizeof(struct _RTCharacterEquipmentData));
    memcpy(&Character->Data.InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SPLIT_INVENTORY) {
    if (!Character) goto error;

    Int32 PacketLength = sizeof(C2S_DATA_SPLIT_INVENTORY) + sizeof(UInt16) * Packet->SplitInventorySlotCount;
    if (PacketLength != Packet->Length) goto error;
    if (Packet->StackSize < 1) goto error;

    RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
    if (!ItemSlot) goto error;

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
    if (!ItemData) goto error;

    if (ItemData->MaxStackSize < 1) goto error;

    UInt64 ItemStackSizeMask = RTItemDataGetStackSizeMask(ItemData);
    Int64 ItemStackSize = ItemSlot->ItemOptions & ItemStackSizeMask;
    Int64 TotalStackSize = Packet->StackSize * Packet->SplitInventorySlotCount;
    if (ItemStackSize < TotalStackSize) goto error;

    for (Int32 Index = 0; Index < Packet->SplitInventorySlotCount; Index += 1) {
        if (!RTInventoryIsSlotEmpty(Runtime, &Character->Data.InventoryInfo, Packet->SplitInventorySlotIndex[Index])) goto error;
    }

    struct _RTItemSlot TargetSlot = *ItemSlot;
    TargetSlot.ItemOptions &= ~ItemStackSizeMask;
    TargetSlot.ItemOptions |= Packet->StackSize & ItemStackSizeMask;

    for (Int32 Index = 0; Index < Packet->SplitInventorySlotCount; Index += 1) {
        TargetSlot.SlotIndex = Packet->SplitInventorySlotIndex[Index];
        RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &TargetSlot);
    }

    ItemStackSize -= TotalStackSize;
    if (ItemStackSize <= 0) {
        RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
    }
    else {
        ItemSlot->ItemOptions &= ~ItemStackSizeMask;
        ItemSlot->ItemOptions |= ItemStackSize & ItemStackSizeMask;
    }

    Character->SyncMask.InventoryInfo = true;

    S2C_DATA_SPLIT_INVENTORY* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SPLIT_INVENTORY);
    Response->Result = 0;
    SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_SPLIT_INVENTORY* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SPLIT_INVENTORY);
        Response->Result = 1;
        SocketSend(Socket, Connection, Response);
    }
}

CLIENT_PROCEDURE_BINDING(MERGE_INVENTORY) {
    if (!Character) goto error;

    Int32 PacketLength = sizeof(C2S_DATA_MERGE_INVENTORY) + sizeof(UInt16) * Packet->MergeInventorySlotCount + sizeof(C2S_DATA_MERGE_INVENTORY_SLOT_RESULT) * Packet->ResultInventorySlotCount;
    if (PacketLength != Packet->Length) goto error;
    if (Packet->MergeInventorySlotCount < 1) goto error;

    RTItemSlotRef FirstItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->MergeInventorySlotIndex[0]);
    if (!FirstItemSlot) goto error;

    RTItemDataRef FirstItemData = RTRuntimeGetItemDataByIndex(Runtime, FirstItemSlot->Item.ID);
    if (!FirstItemData) goto error;

    if (FirstItemData->MaxStackSize < 1) goto error;

    UInt64 ItemStackSizeMask = RTItemDataGetStackSizeMask(FirstItemData);
    Int64 TotalStackSize = 0;

    for (Int32 Index = 0; Index < Packet->MergeInventorySlotCount; Index += 1) {
        RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->MergeInventorySlotIndex[Index]);
        if (ItemSlot->Item.Serial != FirstItemSlot->Item.Serial) goto error;
        if (ItemSlot->ItemDuration.Serial != FirstItemSlot->ItemDuration.Serial) goto error;
        if ((ItemSlot->ItemOptions & ~ItemStackSizeMask) != (FirstItemSlot->ItemOptions & ~ItemStackSizeMask)) goto error;

        TotalStackSize += ItemSlot->ItemOptions & ItemStackSizeMask;
    }

    C2S_DATA_MERGE_INVENTORY_SLOT_RESULT* ResultInventorySlotIndex = (C2S_DATA_MERGE_INVENTORY_SLOT_RESULT*)&Packet->MergeInventorySlotIndex[Packet->MergeInventorySlotCount];

    Int64 ResultStackSize = 0;
    for (Int32 Index = 0; Index < Packet->ResultInventorySlotCount; Index += 1) {
        C2S_DATA_MERGE_INVENTORY_SLOT_RESULT ResultInventorySlot = ResultInventorySlotIndex[Index];
        if (ResultInventorySlot.StackSize > ItemStackSizeMask) goto error;

        RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, ResultInventorySlot.SlotIndex);
        if (ItemSlot && ItemSlot->Item.Serial != FirstItemSlot->Item.Serial) goto error;

        ResultStackSize += ResultInventorySlot.StackSize;
    }
    if (ResultStackSize != TotalStackSize) goto error;

    struct _RTItemSlot ItemSlot = *FirstItemSlot;

    for (Int32 Index = 0; Index < Packet->MergeInventorySlotCount; Index += 1) {
        RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->MergeInventorySlotIndex[Index]);
    }

    for (Int32 Index = 0; Index < Packet->ResultInventorySlotCount; Index += 1) {
        C2S_DATA_MERGE_INVENTORY_SLOT_RESULT ResultInventorySlot = ResultInventorySlotIndex[Index];
        ItemSlot.ItemOptions &= ~ItemStackSizeMask;
        ItemSlot.ItemOptions |= ResultInventorySlot.StackSize & ItemStackSizeMask;
        ItemSlot.SlotIndex = ResultInventorySlot.SlotIndex;
        RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &ItemSlot);
    }

    Character->SyncMask.InventoryInfo = true;

    S2C_DATA_MERGE_INVENTORY* Response = PacketBufferInit(Connection->PacketBuffer, S2C, MERGE_INVENTORY);
    Response->Result = 0;
    SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_MERGE_INVENTORY* Response = PacketBufferInit(Connection->PacketBuffer, S2C, MERGE_INVENTORY);
        Response->Result = 1;
        SocketSend(Socket, Connection, Response);
    }
}

CLIENT_PROCEDURE_BINDING(SORT_INVENTORY) {
    if (!Character) {
        SocketDisconnect(Socket, Connection);
        return;
    }

    struct _RTCharacterInventoryInfo TempInventoryMemory = { 0 };
    RTCharacterInventoryInfoRef TempInventory = &TempInventoryMemory;
    memcpy(TempInventory, &Character->Data.InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));

    Bool InventoryOccupancyMask[RUNTIME_INVENTORY_TOTAL_SIZE] = { 0 };
    memset(InventoryOccupancyMask, 0, sizeof(Bool) * RUNTIME_INVENTORY_TOTAL_SIZE);
    for (Int32 Index = 0; Index < Character->Data.InventoryInfo.Info.SlotCount; Index += 1) {
        InventoryOccupancyMask[Character->Data.InventoryInfo.Slots[Index].SlotIndex] = true;
    }

    for (Int32 Index = 0; Index < Packet->Count; Index += 1) {
        InventoryOccupancyMask[Packet->InventorySlots[Index]] = false;
    }

    for (Int32 Index = 0; Index < Packet->Count; Index += 1) {
        Int32 SlotIndex = RTInventoryGetSlotIndex(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlots[Index]);
        if (SlotIndex < 0) goto error;

        RTItemSlotRef Slot = &TempInventory->Slots[SlotIndex];

        Bool Found = false;
        for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_INVENTORY_TOTAL_SIZE; SlotIndex += 1) {
            if (!InventoryOccupancyMask[SlotIndex]) {
                InventoryOccupancyMask[SlotIndex] = true;
                Slot->SlotIndex = SlotIndex;
                Found = true;
                break;
            }
        }
        assert(Found);
    }

    memcpy(&Character->Data.InventoryInfo, TempInventory, sizeof(struct _RTCharacterInventoryInfo));

    Character->SyncMask.InventoryInfo = true;

    S2C_DATA_SORT_INVENTORY* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SORT_INVENTORY);
    Response->Success = 1;
    SocketSend(Socket, Connection, Response);
    return;

error: 
    {
        S2C_DATA_SORT_INVENTORY* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SORT_INVENTORY);
        Response->Success = 0;
        SocketSend(Socket, Connection, Response);
    }
}

CLIENT_PROCEDURE_BINDING(MOVE_INVENTORY_ITEM_LIST) {
    if (!Character) {
        SocketDisconnect(Socket, Connection);
        return;
    }

    S2C_DATA_MOVE_INVENTORY_ITEM_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, MOVE_INVENTORY_ITEM_LIST);
    Response->Count = Packet->ItemCount;

    Int32 TailLength = Packet->ItemCount * sizeof(CSC_DATA_ITEM_SLOT_INDEX) * 2;
    if (Packet->Length != sizeof(C2S_DATA_MOVE_INVENTORY_ITEM_LIST) + TailLength) goto error;

    CSC_DATA_ITEM_SLOT_INDEX* Source = (CSC_DATA_ITEM_SLOT_INDEX*)(&Packet->Data[0]);
    CSC_DATA_ITEM_SLOT_INDEX* Destination = (CSC_DATA_ITEM_SLOT_INDEX*)(&Packet->Data[0] + Packet->ItemCount * sizeof(CSC_DATA_ITEM_SLOT_INDEX));

    for (Int32 Index = 0; Index < Packet->ItemCount; Index += 1) {
        if (Source[Index].StorageType != STORAGE_TYPE_INVENTORY) goto error;
        if (Destination[Index].StorageType != STORAGE_TYPE_INVENTORY) goto error;

        Int32 SourceIndex = Source[Index].Index;
        RTCharacterInventoryInfoRef SourceInventory = &Character->Data.InventoryInfo;
        if (SourceIndex >= RUNTIME_INVENTORY_TOTAL_SIZE) {
            SourceIndex -= RUNTIME_INVENTORY_TOTAL_SIZE;
            SourceInventory = &Character->Data.TemporaryInventoryInfo;
            if (SourceIndex >= RUNTIME_INVENTORY_TOTAL_SIZE) {
                goto error;
            }
        }

        Bool Success = RTInventoryMoveSlot(
            Runtime,
            SourceInventory,
            &Character->Data.InventoryInfo,
            SourceIndex,
            Destination[Index].Index
        );

        if (!Success) goto error;
    }

    Character->SyncMask.InventoryInfo = true;
    SocketSend(Socket, Connection, Response);
    return;

error:
    Response->Count = 0;
    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(DROP_INVENTORY_ITEM) {
    if (!Character) goto error;

    RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (!WorldContext) goto error;

    // TODO: Check if item type is dropable
    // TODO: Check if item is account binding
    // TODO: Check if item is character binding

    struct _RTItemSlot TempSlot = { 0 };
    if (Packet->StorageType == STORAGE_TYPE_INVENTORY) {
        if (!RTInventoryRemoveSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex, &TempSlot)) goto error;

        Character->SyncMask.InventoryInfo = true;
    }
    else if (Packet->StorageType == STORAGE_TYPE_EQUIPMENT) {
        if (!RTEquipmentRemoveSlot(Runtime, Character, &Character->Data.EquipmentInfo, Packet->InventorySlotIndex, &TempSlot)) goto error;

        Character->SyncMask.EquipmentInfo = true;
    }
    else if (Packet->StorageType == STORAGE_TYPE_WAREHOUSE) {
        if (!RTWarehouseRemoveSlot(Runtime, &Character->Data.WarehouseInfo, Packet->InventorySlotIndex, &TempSlot)) goto error;

        Character->SyncMask.WarehouseInfo = true;
    }
    else if (Packet->StorageType == STORAGE_TYPE_VEHICLE_INVENTORY) {
        if (!RTVehicleInventoryRemoveSlot(Runtime, &Character->Data.VehicleInventoryInfo, Packet->InventorySlotIndex, &TempSlot)) goto error;

        Character->SyncMask.VehicleInventoryInfo = true;
    }
    else {
        goto error;
    }

    RTDropResult DropItem = { 0 };
    DropItem.ItemID.Serial = TempSlot.Item.Serial;
    DropItem.ItemOptions = TempSlot.ItemOptions;
    DropItem.ItemDuration = TempSlot.ItemDuration;
    RTWorldSpawnItem(
        Runtime,
        WorldContext,
        Character->ID,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y,
        DropItem
    );

    S2C_DATA_DROP_INVENTORY_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DROP_INVENTORY_ITEM);
    Response->Success = 1;
    SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_DROP_INVENTORY_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DROP_INVENTORY_ITEM);
        Response->Success = 0;
        SocketSend(Socket, Connection, Response);
    }
}

CLIENT_PROCEDURE_BINDING(SET_ITEM_PROTECTION) {
    if (!Character) goto error;

    RTItemSlotRef ItemSlot = NULL;
    if (Packet->StorageType == C2S_SET_ITEM_PROTECTION_STORAGE_TYPE_INVENTORY) {
        ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
        if (!ItemSlot) goto error;
    }
    else if (Packet->StorageType == C2S_SET_ITEM_PROTECTION_STORAGE_TYPE_EQUIPMENT) {
        ItemSlot = RTEquipmentGetSlot(Runtime, &Character->Data.EquipmentInfo, Packet->InventorySlotIndex);
        if (!ItemSlot) goto error;
    }
    else {
        goto error;
    }

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
    if (!ItemData) goto error;

    if (!RTItemTypeIsProtectable(ItemData->ItemType)) goto error;

    ItemSlot->Item.IsProtected = Packet->IsLocked;

    if (Packet->StorageType == C2S_SET_ITEM_PROTECTION_STORAGE_TYPE_INVENTORY) {
        Character->SyncMask.InventoryInfo = true;

    }
    else if (Packet->StorageType == C2S_SET_ITEM_PROTECTION_STORAGE_TYPE_EQUIPMENT) {
        Character->SyncMask.EquipmentInfo = true;
    }

    S2C_DATA_SET_ITEM_PROTECTION* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SET_ITEM_PROTECTION);
    Response->Success = 1;
    SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_SET_ITEM_PROTECTION* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SET_ITEM_PROTECTION);
        Response->Success = 0;
        SocketSend(Socket, Connection, Response);
    }
}

