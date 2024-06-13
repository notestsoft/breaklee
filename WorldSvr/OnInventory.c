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
static struct _RTCharacterEquipmentInfo kEquipmentInfoBackup;
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
    memcpy(&kEquipmentInfoBackup, &Character->Data.EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
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
        if (!RTWarehouseSetSlot(Runtime, &Character->Data.WarehouseInfo, &TempSlot)) goto error;
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
    memcpy(&Character->Data.EquipmentInfo, &kEquipmentInfoBackup, sizeof(struct _RTCharacterEquipmentInfo));
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
    memcpy(&kEquipmentInfoBackup, &Character->Data.EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
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

    RTCharacterInitializeAttributes(Runtime, Character);

    return SocketSend(Socket, Connection, Response);

error:
    memcpy(&Character->Data.EquipmentInfo, &kEquipmentInfoBackup, sizeof(struct _RTCharacterEquipmentInfo));
    memcpy(&Character->Data.InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));

    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MERGE_INVENTORY) {

    /*
    CLIENT_PROTOCOL_STRUCT(C2S_DATA_MERGE_INVENTORY_SLOT_RESULT,
        UInt16 SlotIndex;
        UInt32 StackSize;
    )

        CLIENT_PROTOCOL(C2S, MERGE_INVENTORY, DEFAULT, 2501,
            UInt16 MergeInventorySlotCount;
            UInt16 ResultInventorySlotCount;
            UInt16 Unknown1;
            UInt16 MergeInventorySlotIndex[0]; // MergeSlotCount
            // C2S_DATA_MERGE_INVENTORY_SLOT_RESULT ResultInventorySlots[ResultInventorySlotCount];
        )

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, SourceSlot->Item.ID);
    if (!ItemData || ItemData->MaxStackSize <= 1) return false;
    */
}

CLIENT_PROCEDURE_BINDING(SORT_INVENTORY) {
    if (!Character) {
        return SocketDisconnect(Socket, Connection);
    }

    struct _RTCharacterInventoryInfo TempInventoryMemory = { 0 };
    RTCharacterInventoryInfoRef TempInventory = &TempInventoryMemory;
    memcpy(TempInventory, &Character->Data.InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));

    Bool InventoryOccupancyMask[RUNTIME_INVENTORY_TOTAL_SIZE] = { 0 };
    memset(InventoryOccupancyMask, 0, sizeof(Bool) * RUNTIME_INVENTORY_TOTAL_SIZE);
    for (Int32 Index = 0; Index < Character->Data.InventoryInfo.Count; Index += 1) {
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
    return SocketSend(Socket, Connection, Response);

error: 
    {
        S2C_DATA_SORT_INVENTORY* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SORT_INVENTORY);
        Response->Success = 0;
        return SocketSend(Socket, Connection, Response);
    }
}

CLIENT_PROCEDURE_BINDING(MOVE_INVENTORY_ITEM_LIST) {
    if (!Character) {
        return SocketDisconnect(Socket, Connection);
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

    return SocketSend(Socket, Connection, Response);

error:
    Response->Count = 0;
    return SocketSend(Socket, Connection, Response);
}
