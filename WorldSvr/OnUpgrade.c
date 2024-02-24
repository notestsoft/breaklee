#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

// TODO: Split all reads from writes

CLIENT_PROCEDURE_BINDING(ADD_FORCE_SLOT_OPTION) {
	if (!Character) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->ItemSlotIndex);
	if (!ItemSlot) goto error;

	// TODO: Implementation is missing

	S2C_DATA_ADD_FORCE_SLOT_OPTION* Response = PacketInit(S2C_DATA_ADD_FORCE_SLOT_OPTION);
	Response->Command = S2C_ADD_FORCE_SLOT_OPTION;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

// TODO: This is still having some issues!!!
CLIENT_PROCEDURE_BINDING(UPGRADE_ITEM_LEVEL) {
	if (!Character) goto error;

	Int32 TailLength = sizeof(UInt16) * Packet->SafeCount;
	if (sizeof(C2S_DATA_UPGRADE_ITEM_LEVEL) + TailLength > Packet->Signature.Length) goto error;

	if (Packet->CoreCount < 1) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->InventorySlotIndex);
	if (!ItemSlot) goto error;
	
	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) goto error;

	RTDataUpgradeLimitRef UpgradeLimit = RTRuntimeDataUpgradeLimitGet(Runtime->Context, ItemData->ItemGrade);
	if (!UpgradeLimit) goto error;
	if (ItemSlot->Item.UpgradeLevel >= UpgradeLimit->MaxItemLevel) goto error;

	RTDataUpgradeCostCoreRef UpgradeCostCore = RTRuntimeDataUpgradeCostCoreGet(Runtime->Context, ItemSlot->Item.UpgradeLevel);
	if (!UpgradeCostCore) goto error;

	RTDataUpgradeGradeRef UpgradeGrade = RTRuntimeDataUpgradeGradeGet(Runtime->Context, ItemData->ItemGrade);
	if (!UpgradeGrade) goto error;

	RTDataUpgradeCostSafeguardRef UpgradeCostSafeguard = RTRuntimeDataUpgradeCostSafeguardGet(Runtime->Context, ItemSlot->Item.UpgradeLevel);
	if (!UpgradeCostSafeguard) goto error;

	RTItemSlotRef CoreSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->CoreSlotIndices[0]);
	assert(CoreSlot);

	RTItemDataRef CoreData = RTRuntimeGetItemDataByIndex(Runtime, CoreSlot->Item.ID);
	assert(CoreData);

	if (CoreData->ItemType != RUNTIME_ITEM_TYPE_UPGRADE_CORE &&
		CoreData->ItemType != RUNTIME_ITEM_TYPE_UPGRADE_CORE_SET) goto error;
	if (CoreData->ItemGrade != UpgradeGrade->CostGrade) goto error;

	Int32 UpgradeCoreLevel = 0;
	Int32 UpgradeCoreFlagIndex = 0;
	Int32 UpgradeType = RUNTIME_DATA_UPGRADE_TYPE_NORMAL;
	Int32 RemainingCoreCount = 0;
	Int32 RemainingSafeCount = 0;

	if (CoreData->UpgradeCore.IsPerfectCore) {
		UpgradeType = RUNTIME_DATA_UPGRADE_TYPE_PERFECT;
		UpgradeCoreLevel = 0;
		UpgradeCoreFlagIndex = 0;
	}
	else {
		if (CoreSlot->Item.UpgradeLevel < RUNTIME_DATA_UPGRADE_TYPE_NORMAL ||
			CoreSlot->Item.UpgradeLevel >= RUNTIME_DATA_UPGRADE_TYPE_PERFECT) goto error;

		UpgradeType = CoreSlot->Item.UpgradeLevel;
		UpgradeCoreLevel = CoreSlot->Item.UpgradeLevel;
		UpgradeCoreFlagIndex = 4;
	}

	Int32 RequiredCoreCount = INT32_MAX;
	Int32 RequiredSafeCount = INT32_MAX;

	switch (UpgradeType) {
	case RUNTIME_DATA_UPGRADE_TYPE_NORMAL:
		RequiredCoreCount = UpgradeCostCore->NormalCount;
		RequiredSafeCount = UpgradeCostSafeguard->NormalCount;
		break;

	case RUNTIME_DATA_UPGRADE_TYPE_ENCHANT:
		RequiredCoreCount = UpgradeCostCore->EnhancedCount;
		RequiredSafeCount = UpgradeCostSafeguard->EnhancedCount;
		break;

	case RUNTIME_DATA_UPGRADE_TYPE_SUPERIOR:
		RequiredCoreCount = UpgradeCostCore->SuperiorCount;
		RequiredSafeCount = UpgradeCostSafeguard->SuperiorCount;
		break;

	case RUNTIME_DATA_UPGRADE_TYPE_PERFECT:
		RequiredCoreCount = UpgradeCostCore->PerfectCount;
		RequiredSafeCount = 0;
		break;

	default:
		goto error;
	}

	// TODO: We have to snapshot inventory here to rollback!!!
	for (Int32 Index = 0; Index < Packet->CoreCount; Index += 1) {
		RTItemSlotRef CoreSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->CoreSlotIndices[Index]);
		assert(CoreSlot);

		if (CoreSlot->Item.UpgradeLevel != UpgradeCoreLevel) goto error;

		RTItemDataRef CoreData = RTRuntimeGetItemDataByIndex(Runtime, CoreSlot->Item.ID);
		assert(CoreData);

		if (CoreSlot->Item.UpgradeLevel < RUNTIME_DATA_UPGRADE_TYPE_NORMAL ||
			CoreSlot->Item.UpgradeLevel >= RUNTIME_DATA_UPGRADE_TYPE_PERFECT) goto error;
		if (CoreData->ItemGrade != UpgradeGrade->CostGrade) goto error;

		if (!CoreData->Options[UpgradeCoreFlagIndex]) goto error;

		if (CoreData->ItemType == RUNTIME_ITEM_TYPE_UPGRADE_CORE) {
			if (!RTInventoryClearSlot(Runtime, &Character->InventoryInfo, CoreSlot->SlotIndex)) goto error;

			RequiredCoreCount -= 1;
		}
		else if (CoreData->ItemType == RUNTIME_ITEM_TYPE_UPGRADE_CORE_SET) {
			Int32 ConsumableCoreCount = MIN(RequiredCoreCount, (Int32)CoreSlot->ItemOptions);

			CoreSlot->ItemOptions -= ConsumableCoreCount;
			if (CoreSlot->ItemOptions < 1) {
				if (!RTInventoryClearSlot(Runtime, &Character->InventoryInfo, CoreSlot->SlotIndex)) goto error;
			}

			RemainingCoreCount = (Int32)CoreSlot->ItemOptions;
			RequiredCoreCount -= ConsumableCoreCount;
		}

		if (RequiredCoreCount < 1) {
			break;
		}
	}

	if (RequiredCoreCount > 0) goto error;

	struct _RTItemSlot ItemSlotCopy = *ItemSlot;

	Int32 Seed = (Int32)PlatformGetTickCount();
	Int32 UpgradePoint = Client->UpgradePoint;
	Int32 Result = RTItemUpgradeNormal(
		Runtime,
		ItemSlot,
		UpgradeType,
		&UpgradePoint,
		&Seed
	);

	Int32 ConsumedSafeCount = 0;
	if (Result != RUNTIME_UPGRADE_RATE_TYPE_UPGRADE_1 &&
		Result != RUNTIME_UPGRADE_RATE_TYPE_UPGRADE_2) {

		for (Int32 Index = 0; Index < Packet->SafeCount; Index += 1) {
			RTItemSlotRef SafeSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->SafeSlotIndices[Index]);
			assert(SafeSlot);

			RTItemDataRef SafeData = RTRuntimeGetItemDataByIndex(Runtime, SafeSlot->Item.ID);
			assert(SafeData);

			if (SafeData->ItemType != RUNTIME_ITEM_TYPE_SAFEGUARD ||
				SafeData->ItemGrade != UpgradeGrade->CostGrade) goto error;

			Int32 ConsumableSafeCount = MIN(RequiredSafeCount, (Int32)SafeSlot->ItemOptions);

			SafeSlot->ItemOptions -= ConsumableSafeCount;
			if (SafeSlot->ItemOptions < 1) {
				if (!RTInventoryClearSlot(Runtime, &Character->InventoryInfo, SafeSlot->SlotIndex)) goto error;
			}

			RemainingSafeCount = (Int32)SafeSlot->ItemOptions;
			RequiredSafeCount -= ConsumableSafeCount;
			ConsumedSafeCount += ConsumableSafeCount;
		}

		if (ConsumedSafeCount > 0 && RequiredSafeCount < 1) {
			*ItemSlot = ItemSlotCopy;
		}
	}

	Client->UpgradePoint = UpgradePoint;
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH;

	S2C_DATA_UPDATE_UPGRAGE_POINTS* Notification = PacketInit(S2C_DATA_UPDATE_UPGRAGE_POINTS);
	Notification->Command = S2C_UPDATE_UPGRAGE_POINTS;
	Notification->UpgradePoint = Client->UpgradePoint;
	Notification->Timestamp = (UInt32)GetTimestamp() + 1000 * 60 * 60;
	SocketSend(Socket, Connection, Notification);

	S2C_DATA_UPGRADE_ITEM_LEVEL* Response = PacketInit(S2C_DATA_UPGRADE_ITEM_LEVEL);
	Response->Command = S2C_UPGRADE_ITEM_LEVEL;
	Response->Result = Result;
	Response->ItemID = ItemSlot->Item.Serial;
	Response->ItemOption = ItemSlot->ItemOptions;
	Response->RemainingCoreCount = RemainingCoreCount;
	Response->ConsumedSafeCount = ConsumedSafeCount;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
