#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// TODO: Split all reads from writes

CLIENT_PROCEDURE_BINDING(ADD_FORCE_SLOT_OPTION) {
	if (!Character) goto error;

	if (Packet->ForceCoreCount < 1 || Packet->ForceCoreCount > 10) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->ItemSlotIndex);
	if (!ItemSlot) goto error;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) goto error;

	RTDataUpgradeGradeRef UpgradeGrade = RTRuntimeDataUpgradeGradeGet(Runtime->Context, ItemData->ItemGrade);
	if (!UpgradeGrade) goto error;

	Int32 CostGrade = UpgradeGrade->CostGrade;

	RTDataUpgradeGradeChangeRef UpgradeGradeChange = RTRuntimeDataUpgradeGradeChangeGet(Runtime->Context, ItemData->ItemType, ItemData->ItemGrade);
	if (UpgradeGradeChange) {
		CostGrade = UpgradeGradeChange->CostGrade;
	}

	for (Int SlotIndex = 0; SlotIndex < Packet->ForceCoreCount; SlotIndex += 1) {
		RTItemSlotRef ForceCoreSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->ForceCoreSlotIndices[SlotIndex]);
		if (!ForceCoreSlot) goto error;
		
		RTItemDataRef ForceCoreData = RTRuntimeGetItemDataByIndex(Runtime, ForceCoreSlot->Item.ID);
		if (!ForceCoreData) goto error;

		if (ForceCoreData->ItemType != RUNTIME_ITEM_TYPE_FORCE_CORE) goto error;
		if (ForceCoreData->ItemGrade != CostGrade) goto error;
	}

	RTItemOptions ItemOptions = { 0 };
	ItemOptions.Serial = ItemSlot->ItemOptions;

	Int32 FilledSlotCount = RTItemOptionGetFilledSlotCount(ItemOptions);
	Int32 TotalSlotCount = ItemOptions.Equipment.SlotCount;
	if (FilledSlotCount >= TotalSlotCount) goto error;

	RTDataForceCodeCostRef ForceCodeCost = RTRuntimeDataForceCodeCostGet(Runtime->Context, CostGrade, FilledSlotCount);
	if (!ForceCodeCost) goto error;

	if (Character->Data.Info.Alz < ForceCodeCost->CurrencyCost) goto error;

	Bool IsOneHandedWeapon = (
		ItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_CHAKRAM
	);
	Int32 ItemType = IsOneHandedWeapon ? RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND : ItemData->ItemType;

	RTDataForceCoreBaseRef ForceCoreBase = RTRuntimeDataForceCoreBaseGet(Runtime->Context, ItemData->ItemGrade, ItemType);
	if (!ForceCoreBase) goto error;

	RTItemSlotRef FixedScrollSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->OptionScrollSlotIndex);
	RTItemDataRef FixedScrollData = NULL;
	RTItemOptions FixedScrollOptions = { 0 };

	if (FixedScrollSlot) {
		FixedScrollData = RTRuntimeGetItemDataByIndex(Runtime, FixedScrollSlot->Item.ID);
		if (!FixedScrollData) goto error;
		
		FixedScrollOptions.Serial = FixedScrollSlot->ItemOptions;
		if (FixedScrollOptions.OptionScroll.ForceEffectIndex < 1) goto error;
	}

	RTItemSlotRef RandomScrollSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->RandomScrollSlotIndex);
	RTItemDataRef RandomScrollData = NULL;
	RTItemOptions RandomScrollOptions = { 0 };

	if (RandomScrollSlot) {
		RandomScrollData = RTRuntimeGetItemDataByIndex(Runtime, RandomScrollSlot->Item.ID);
		if (!RandomScrollData) goto error;

		RandomScrollOptions.Serial = RandomScrollSlot->ItemOptions;
		if (RandomScrollOptions.OptionScroll.ForceEffectIndex > 0) goto error;
	}

	if (!FixedScrollSlot && !RandomScrollSlot) goto error;

	RTItemSlotRef MainScrollSlot = FixedScrollSlot ? FixedScrollSlot : RandomScrollSlot;
	RTItemDataRef MainScrollData = FixedScrollSlot ? FixedScrollData : RandomScrollData;
	RTItemOptions MainScrollOptions = FixedScrollSlot ? FixedScrollOptions : RandomScrollOptions;

	Int32 Seed = (Int32)PlatformGetTickCount();
	if (FixedScrollSlot && RandomScrollSlot) {
		Int32 Value = RandomRange(&Seed, 0, 10000);
		if (Value >= 5000) {
			MainScrollSlot = RandomScrollSlot;
			MainScrollData = RandomScrollData;
			MainScrollOptions = RandomScrollOptions;
		}
	}

	RTDataForceCoreBaseCodeRef ForceCoreBaseCode = NULL;
	if (MainScrollOptions.OptionScroll.ForceEffectIndex < 1) {
		Int32 RandomForceCorePoolCount = 0;
		Int32 RandomForceCorePool[16] = { 0 };

		for (Int Index = 0; Index < ForceCoreBase->ForceCoreBaseCodeCount; Index += 1) {
			RTDataForceCoreBaseCodeRef Code = &ForceCoreBase->ForceCoreBaseCodeList[Index];
			if (Code->HasRandomRate > 0) {
				RandomForceCorePool[RandomForceCorePoolCount] = (Int32)Index;
				RandomForceCorePoolCount += 1;
			}
		}

		assert(RandomForceCorePoolCount > 0);

		Int32 RandomIndex = RandomRange(&Seed, 0, RandomForceCorePoolCount - 1);
		ForceCoreBaseCode = &ForceCoreBase->ForceCoreBaseCodeList[RandomIndex];
	}
	else {
		ForceCoreBaseCode = RTRuntimeDataForceCoreBaseCodeGet(ForceCoreBase, MainScrollOptions.OptionScroll.ForceEffectIndex);
	}

	if (!ForceCoreBaseCode) {
		goto error;
	}

	RTDataForceCodeRateBaseRef ForceCodeRateBase = RTRuntimeDataForceCodeRateBaseGet(Runtime->Context);
	if (!ForceCodeRateBase) goto error;

	Int32 SuccessRate = ForceCodeRateBase->DefaultRate;

	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BIKE) {
		SuccessRate = ForceCodeRateBase->VehicleBikeRate;
	}

	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_CREST || ItemData->ItemType == RUNTIME_ITEM_TYPE_EPAULET) {
		SuccessRate = ForceCodeRateBase->EmblemRate;
	}

	SuccessRate *= Packet->ForceCoreCount;
	
	RTDataForceCodeRateRef ForceCodeRate = RTRuntimeDataForceCodeRateGet(Runtime->Context, ItemData->ItemType, FilledSlotCount, ItemSlot->Item.UpgradeLevel);
	if (!ForceCodeRate) goto error;

	Int32 EqualSlotCount = RTItemOptionGetForceSlotCount(ItemOptions, ForceCoreBaseCode->ForceIndex);
	if (EqualSlotCount == 0) SuccessRate += ForceCodeRate->Rate1;
	if (EqualSlotCount == 1) SuccessRate += ForceCodeRate->Rate2;
	if (EqualSlotCount == 2) SuccessRate += ForceCodeRate->Rate3;

	Bool HasError = false;
	Int32 Value = RandomRange(&Seed, 0, 1000000000);
	if (Value <= SuccessRate) {
		RTItemOptionSlot ForceSlot = { 0 };
		ForceSlot.ForceIndex = ForceCoreBaseCode->ForceIndex;
		ForceSlot.ForceLevel = 1;
		HasError = !RTItemOptionAppendSlot((RTItemOptions*)&ItemSlot->ItemOptions, ForceSlot, ItemSlot->Item.IsAccountBinding);
	}
	else {
		RTItemOptionClearSecondForceSlot((RTItemOptions*)&ItemSlot->ItemOptions);
	}

	if (!HasError) {
		if (FixedScrollSlot) {
			FixedScrollOptions.OptionScroll.StackSize -= 1;
			if (FixedScrollOptions.OptionScroll.StackSize > 0) {
				FixedScrollSlot->ItemOptions = FixedScrollOptions.Serial;
			}
			else {
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, FixedScrollSlot->SlotIndex);

				if (Packet->RandomScrollSlotIndex >= 0) {
					RandomScrollSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->RandomScrollSlotIndex);
				}
			}
		}

		if (RandomScrollSlot) {
			RandomScrollOptions.OptionScroll.StackSize -= 1;
			if (RandomScrollOptions.OptionScroll.StackSize > 0) {
				RandomScrollSlot->ItemOptions = RandomScrollOptions.Serial;
			}
			else {
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, RandomScrollSlot->SlotIndex);
			}
		}

		for (Int SlotIndex = 0; SlotIndex < Packet->ForceCoreCount; SlotIndex += 1) {
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->ForceCoreSlotIndices[SlotIndex]);			
		}

		Character->Data.Info.Alz -= ForceCodeCost->CurrencyCost;
		Character->SyncMask.Info = true;
		Character->SyncMask.InventoryInfo = true;
	}

	ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->ItemSlotIndex);
	assert(ItemSlot);

	S2C_DATA_ADD_FORCE_SLOT_OPTION* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ADD_FORCE_SLOT_OPTION);
	Response->Result = HasError ? 0 : 1;
	Response->ItemOptions = ItemSlot->ItemOptions;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

// TODO: This is still having some issues!!!
CLIENT_PROCEDURE_BINDING(UPGRADE_ITEM_LEVEL) {
	if (!Character) goto error;

	Int32 TailLength = sizeof(UInt16) * Packet->SafeCount;
	if (sizeof(C2S_DATA_UPGRADE_ITEM_LEVEL) + TailLength > Packet->Length) goto error;

	if (Packet->CoreCount < 1) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	if (!ItemSlot) goto error;

    struct _RTItemSlot ItemSlotCopy = *ItemSlot;

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

	RTItemSlotRef CoreSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->CoreSlotIndices[0]);
	if (!CoreSlot) goto error;

	RTItemDataRef CoreData = RTRuntimeGetItemDataByIndex(Runtime, CoreSlot->Item.ID);
	if (!CoreData) goto error;

	if (CoreData->ItemType != RUNTIME_ITEM_TYPE_UPGRADE_CORE &&
		CoreData->ItemType != RUNTIME_ITEM_TYPE_UPGRADE_CORE_SET) goto error;
	if (CoreData->ItemGrade != UpgradeGrade->CostGrade) goto error;

	Int32 UpgradeCoreLevel = 0;
	Int32 UpgradeCoreFlagIndex = 0;
	Int32 UpgradeType = RUNTIME_DATA_UPGRADE_TYPE_NORMAL;
	Int32 RemainingCoreCount = 0;
	Int32 RemainingSafeCount = 0;

	if (CoreData->UpgradeCore.IsPerfectCore > 0) {
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
	for (Int Index = 0; Index < Packet->CoreCount; Index += 1) {
		RTItemSlotRef CoreSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->CoreSlotIndices[Index]);
		assert(CoreSlot);

		if (CoreSlot->Item.UpgradeLevel != UpgradeCoreLevel) goto error;

		RTItemDataRef CoreData = RTRuntimeGetItemDataByIndex(Runtime, CoreSlot->Item.ID);
		assert(CoreData);

		if (CoreSlot->Item.UpgradeLevel < RUNTIME_DATA_UPGRADE_TYPE_NORMAL ||
			CoreSlot->Item.UpgradeLevel >= RUNTIME_DATA_UPGRADE_TYPE_PERFECT) goto error;
		if (CoreData->ItemGrade != UpgradeGrade->CostGrade) goto error;

		if (!CoreData->Options[UpgradeCoreFlagIndex]) goto error;

		if (CoreData->ItemType == RUNTIME_ITEM_TYPE_UPGRADE_CORE) {
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, CoreSlot->SlotIndex);
			RequiredCoreCount -= 1;
		}
		else if (CoreData->ItemType == RUNTIME_ITEM_TYPE_UPGRADE_CORE_SET) {
			Int32 ConsumableCoreCount = MIN(RequiredCoreCount, (Int32)CoreSlot->ItemOptions);

			CoreSlot->ItemOptions -= ConsumableCoreCount;
			RemainingCoreCount = (Int32)CoreSlot->ItemOptions;
			RequiredCoreCount -= ConsumableCoreCount;

			if (CoreSlot->ItemOptions < 1) {
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, CoreSlot->SlotIndex);
			}
		}

		if (RequiredCoreCount < 1) {
			break;
		}
	}

	if (RequiredCoreCount > 0) goto error;

    // TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
    ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);

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
	if (Result != RUNTIME_UPGRADE_RESULT_UPGRADE_1 &&
		Result != RUNTIME_UPGRADE_RESULT_UPGRADE_2) {

		for (Int Index = 0; Index < Packet->SafeCount; Index += 1) {
			RTItemSlotRef SafeSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->SafeSlotIndices[Index]);
			assert(SafeSlot);

			RTItemDataRef SafeData = RTRuntimeGetItemDataByIndex(Runtime, SafeSlot->Item.ID);
			assert(SafeData);

			if (SafeData->ItemType != RUNTIME_ITEM_TYPE_SAFEGUARD ||
				SafeData->ItemGrade != UpgradeGrade->CostGrade) goto error;

			Int32 ConsumableSafeCount = MIN(RequiredSafeCount, (Int32)SafeSlot->ItemOptions);

			SafeSlot->ItemOptions -= ConsumableSafeCount;
			RemainingSafeCount = (Int32)SafeSlot->ItemOptions;
			RequiredSafeCount -= ConsumableSafeCount;
			ConsumedSafeCount += ConsumableSafeCount;

			if (SafeSlot->ItemOptions < 1) {
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SafeSlot->SlotIndex);
			}
		}

		if (ConsumedSafeCount > 0 && RequiredSafeCount < 1) {
            // TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
            ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
			*ItemSlot = ItemSlotCopy;
		}
	}

	Client->UpgradePoint = UpgradePoint;
	Character->SyncMask.InventoryInfo = true;

	S2C_DATA_UPDATE_UPGRADE_POINTS* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPDATE_UPGRADE_POINTS);
	Notification->UpgradePoint = Client->UpgradePoint;
	Notification->Timestamp = (UInt32)GetTimestampMs() + 1000 * 60 * 60;
	SocketSend(Socket, Connection, Notification);

	S2C_DATA_UPGRADE_ITEM_LEVEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPGRADE_ITEM_LEVEL);
	Response->Result = Result;
	Response->ItemID = ItemSlot->Item.Serial;
	Response->ItemOption = ItemSlot->ItemOptions;
	Response->RemainingCoreCount = RemainingCoreCount;
	Response->ConsumedSafeCount = ConsumedSafeCount;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPGRADE_EXTREME_LEVEL) {
	if (!Character) goto error;

	Int32 TailLength = sizeof(UInt16) * Packet->InventorySlotCount;
	if (sizeof(C2S_DATA_UPGRADE_EXTREME_LEVEL) + TailLength > Packet->Length) goto error;

	RTItemSlotRef TargetSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->TargetSlotIndex);
	if (!TargetSlot) goto error;

	RTItemDataRef TargetData = RTRuntimeGetItemDataByIndex(Runtime, TargetSlot->Item.ID);
	if (!TargetData) goto error;
    
    RTDataItemType TargetItemType = TargetData->ItemType;
    if (TargetItemType == RUNTIME_ITEM_TYPE_HELMED2) TargetItemType = RUNTIME_ITEM_TYPE_HELMED1;

	RTDataExtremeUpgradeBaseGradeRef ExtremeUpgradeBaseGrade = NULL;
	for (Int Index = 0; Index < Runtime->Context->ExtremeUpgradeBaseCount; Index += 1) {
		RTDataExtremeUpgradeBaseRef ExtremeUpgradeBase = &Runtime->Context->ExtremeUpgradeBaseList[Index];
		if (ExtremeUpgradeBase->Type != TargetItemType) continue;

		ExtremeUpgradeBaseGrade = RTRuntimeDataExtremeUpgradeBaseGradeGet(ExtremeUpgradeBase, TargetData->ItemGrade);
		if (ExtremeUpgradeBaseGrade) break;
	}
	if (!ExtremeUpgradeBaseGrade) goto error;

	RTDataExtremeUpgradeMainRef ExtremeUpgradeMain = RTRuntimeDataExtremeUpgradeMainGet(Runtime->Context, ExtremeUpgradeBaseGrade->ExtremeUpgradeGroup);
	if (!ExtremeUpgradeMain) goto error;

	RTDataExtremeUpgradeMainLevelRef ExtremeUpgradeMainLevel = RTRuntimeDataExtremeUpgradeMainLevelGet(ExtremeUpgradeMain, TargetSlot->Item.ExtremeLevel + 1);
	if (!ExtremeUpgradeMainLevel) goto error;

	RTDataExtremeUpgradeFormulaRef ExtremeUpgradeFormula = RTRuntimeDataExtremeUpgradeFormulaGet(Runtime->Context, ExtremeUpgradeBaseGrade->FormulaGroup);
	if (!ExtremeUpgradeFormula) goto error;

	RTDataExtremeUpgradeFormulaLevelRef ExtremeUpgradeFormulaLevel = RTRuntimeDataExtremeUpgradeFormulaLevelGet(ExtremeUpgradeFormula, TargetSlot->Item.ExtremeLevel + 1);
	if (!ExtremeUpgradeFormulaLevel) goto error;

	if (Packet->InventorySlotCount != ExtremeUpgradeFormulaLevel->RequiredCoreCount) {
		S2C_DATA_UPGRADE_EXTREME_LEVEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPGRADE_EXTREME_LEVEL);
		Response->Result = S2C_UPGRADE_EXTRENE_LEVEL_RESULT_INSUFFICIENT_CORES;
		Response->Currency = Character->Data.Info.Alz;
		SocketSend(Socket, Connection, Response);
		return;
	}

	if (TargetSlot->Item.UpgradeLevel < 15) goto error;
	if (TargetSlot->Item.ExtremeLevel >= ExtremeUpgradeBaseGrade->ExtremeUpgradeMax) goto error;

	if (Character->Data.Info.Alz < ExtremeUpgradeFormulaLevel->CurrencyPrice) {
		S2C_DATA_UPGRADE_EXTREME_LEVEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPGRADE_EXTREME_LEVEL);
		Response->Result = S2C_UPGRADE_EXTRENE_LEVEL_RESULT_INSUFFICIENT_CURRENCY;
		Response->Currency = Character->Data.Info.Alz;
		SocketSend(Socket, Connection, Response);
		return;
	}

	Int32 CorePower = 0;

	for (Int Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
		if (!ItemSlot) goto error;

		RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
		if (!ItemData) goto error;
		if (ItemData->ItemType != RUNTIME_ITEM_TYPE_EXTREME_CORE) goto error;

		RTDataExtremeUpgradeOptionRef ExtremeUpgradeOption = RTRuntimeDataExtremeUpgradeOptionGet(Runtime->Context, ItemData->ItemGrade);
		if (!ExtremeUpgradeOption) goto error;
		if (ExtremeUpgradeOption->ExtremeUpgradeOptionLevelCount < 1) goto error;

		CorePower += ExtremeUpgradeOption->ExtremeUpgradeOptionLevelList[0].OptionLevel;
	}

	S2C_DATA_UPGRADE_EXTREME_LEVEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPGRADE_EXTREME_LEVEL);

	Int32 SuccessRate = 1000 * (ExtremeUpgradeFormulaLevel->ExtremeUpgradeFactor * CorePower) / (100 * ExtremeUpgradeFormulaLevel->BaseExtremeLevel);
	Int32 RandomValue = RandomRange(&Client->DiceSeed, 0, 1000);
	if (RandomValue < SuccessRate) {
		Response->Result = S2C_UPGRADE_EXTREME_LEVEL_RESULT_SUCCESS;
		TargetSlot->Item.ExtremeLevel += 1;
	}
	else {
		Int32 BreakRate = ExtremeUpgradeFormulaLevel->BreakItemRate;
		Int32 RandomValue = RandomRange(&Client->DiceSeed, 0, 1000);
		if (RandomValue < BreakRate) {
			Response->Result = S2C_UPGRADE_EXTREME_LEVEL_RESULT_BROKEN;
			TargetSlot->Item.IsBroken = true;
		}
		else {
			Response->Result = S2C_UPGRADE_EXTREME_LEVEL_RESULT_RESET;
			TargetSlot->Item.ExtremeLevel = 0;
		}
	}

	Character->Data.Info.Alz -= ExtremeUpgradeFormulaLevel->CurrencyPrice;

	for (Int Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
	}

	Character->SyncMask.Info = true;
	Character->SyncMask.InventoryInfo = true;

	Response->Currency = Character->Data.Info.Alz;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPGRADE_EXTREME_REPAIR) {
	if (!Character) goto error;

	RTItemSlotRef SourceSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->SourceSlotIndex);
	if (!SourceSlot) goto error;

	RTItemDataRef SourceData = RTRuntimeGetItemDataByIndex(Runtime, SourceSlot->Item.ID);
	if (!SourceData) goto error;

	RTItemSlotRef TargetSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->TargetSlotIndex);
	if (!TargetSlot) goto error;

	RTItemDataRef TargetData = RTRuntimeGetItemDataByIndex(Runtime, TargetSlot->Item.ID);
	if (!TargetData) goto error;

	if (SourceData->ItemType != RUNTIME_ITEM_TYPE_REPAIR_KIT) goto error;
	if (!TargetSlot->Item.IsBroken) goto error;

	S2C_DATA_UPGRADE_EXTREME_REPAIR* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPGRADE_EXTREME_REPAIR);
	Response->Result = S2C_UPGRADE_EXTREME_REPAIR_RESULT_ERROR;

	UInt16 SourceItemSlotIndex = SourceSlot->SlotIndex;
	UInt16 TargetItemSlotIndex = TargetSlot->SlotIndex;
	Int32 RandomValue = RandomRange(&Client->DiceSeed, 0, 1000);
	if (RandomValue < SourceData->RepairKit.SuccessRate) {
		Response->Result = S2C_UPGRADE_EXTREME_REPAIR_RESULT_SUCCESS;
		TargetSlot->Item.IsBroken = false;
	}
	else if (SourceData->RepairKit.HasSafeguard > 0) {
		Response->Result = S2C_UPGRADE_EXTREME_REPAIR_RESULT_FAILED;
	}
	else {
		Response->Result = S2C_UPGRADE_EXTREME_REPAIR_RESULT_BROKEN;
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, TargetItemSlotIndex);
	}

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceItemSlotIndex);
	Character->SyncMask.InventoryInfo = true;

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPGRADE_CHAOS_LEVEL) {
	if (!Character) goto error;

	Int32 TailLength = sizeof(UInt16) * (Packet->CoreCount + Packet->SafeCount);
	if (sizeof(S2C_DATA_UPGRADE_CHAOS_LEVEL) + TailLength > Packet->Length) goto error;

	if (Packet->CoreCount < 1) goto error;

	UInt16* CoreSlotIndices = &Packet->CoreSlotIndices[0];
	UInt16* SafeSlotIndices = &Packet->CoreSlotIndices[Packet->CoreCount];

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	if (!ItemSlot) goto error;

	struct _RTItemSlot ItemSlotCopy = *ItemSlot;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) goto error;

	RTDataChaosUpgradeItemListRef UpgradeMain = RTRuntimeDataChaosUpgradeItemListGet(Runtime->Context, ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX);
	if (!UpgradeMain) goto error;
	if (ItemSlot->Item.UpgradeLevel < UpgradeMain->CheckLevel) goto error;
	if (ItemSlot->Item.UpgradeLevel >= UpgradeMain->ItemGrade) goto error;

	RTDataChaosUpgradeGroupCostRef UpgradeGroupCost = RTRuntimeDataChaosUpgradeGroupCostGet(Runtime->Context, UpgradeMain->Group);
	if (!UpgradeGroupCost) goto error;

	RTDataChaosUpgradeGroupCostLevelRef UpgradeGroupCostLevel = RTRuntimeDataChaosUpgradeGroupCostLevelGet(UpgradeGroupCost, ItemSlot->Item.UpgradeLevel);
	if (!UpgradeGroupCostLevel) goto error;

	RTItemSlotRef CoreSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, CoreSlotIndices[0]);
	assert(CoreSlot);

	RTItemDataRef CoreData = RTRuntimeGetItemDataByIndex(Runtime, CoreSlot->Item.ID);
	assert(CoreData);

	if (CoreData->ItemType != RUNTIME_ITEM_TYPE_CHAOS_CORE &&
		CoreData->ItemType != RUNTIME_ITEM_TYPE_CHAOS_CORE_SET) goto error;

	Int32 RemainingCoreCount = 0;
	Int32 RemainingSafeCount = 0;
	Int32 RequiredCoreCount = UpgradeGroupCostLevel->RequiredCoreCount;
	Int32 RequiredSafeCount = UpgradeGroupCostLevel->RequiredSafeCount;

	// TODO: We have to snapshot inventory here to rollback!!!
	for (Int Index = 0; Index < Packet->CoreCount; Index += 1) {
		RTItemSlotRef CoreSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, CoreSlotIndices[Index]);
		assert(CoreSlot);

		RTItemDataRef CoreData = RTRuntimeGetItemDataByIndex(Runtime, CoreSlot->Item.ID);
		assert(CoreData);

		if (CoreData->ItemType == RUNTIME_ITEM_TYPE_CHAOS_CORE) {
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, CoreSlot->SlotIndex);
			RequiredCoreCount -= 1;
		}
		else if (CoreData->ItemType == RUNTIME_ITEM_TYPE_CHAOS_CORE_SET) {
			Int32 ConsumableCoreCount = MIN(RequiredCoreCount, (Int32)CoreSlot->ItemOptions);

			CoreSlot->ItemOptions -= ConsumableCoreCount;
			RemainingCoreCount = (Int32)CoreSlot->ItemOptions;
			RequiredCoreCount -= ConsumableCoreCount;

			if (CoreSlot->ItemOptions < 1) {
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, CoreSlot->SlotIndex);
			}
		}

		if (RequiredCoreCount < 1) {
			break;
		}
	}

	if (RequiredCoreCount > 0) goto error;

	// TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
	ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);

	Int32 Seed = (Int32)PlatformGetTickCount();
	Int32 UpgradePoint = Client->UpgradePoint;
	Bool DestroyItem = false;
	Int32 Result = RTItemUpgradeChaos(
		Runtime,
		ItemSlot,
		&UpgradePoint,
		&Seed,
		&DestroyItem
	);

	Int32 ConsumedSafeCount = 0;
	if (Result != RUNTIME_CHAOS_UPGRADE_RESULT_UPGRADE) {
		for (Int Index = 0; Index < Packet->SafeCount; Index += 1) {
			RTItemSlotRef SafeSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, SafeSlotIndices[Index]);
			assert(SafeSlot);

			RTItemDataRef SafeData = RTRuntimeGetItemDataByIndex(Runtime, SafeSlot->Item.ID);
			assert(SafeData);

			if (SafeData->ItemType != RUNTIME_ITEM_TYPE_CHAOS_SAFEGUARD) goto error;

			if (SafeData->Options[0] == 0) {
				RequiredSafeCount = 1;
			}

			Bool Found = false;
			for (Int SafeGuardItemIndex = 0; SafeGuardItemIndex < Runtime->Context->ChaosUpgradeSafeguardItemCount; SafeGuardItemIndex += 1) {
				RTDataChaosUpgradeSafeguardItemRef UpgradeSafeGuardItem = &Runtime->Context->ChaosUpgradeSafeguardItemList[SafeGuardItemIndex];
				if (UpgradeSafeGuardItem->Group != UpgradeMain->Group) continue;
				if (UpgradeSafeGuardItem->Option != SafeData->Options[0]) continue;

				Found = true;
				break;
			}

			if (!Found) goto error;

			Int32 ConsumableSafeCount = MIN(RequiredSafeCount, (Int32)SafeSlot->ItemOptions);

			SafeSlot->ItemOptions -= ConsumableSafeCount;
			RemainingSafeCount = (Int32)SafeSlot->ItemOptions;
			RequiredSafeCount -= ConsumableSafeCount;
			ConsumedSafeCount += ConsumableSafeCount;

			if (SafeSlot->ItemOptions < 1) {
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SafeSlot->SlotIndex);
			}
		}

		if (ConsumedSafeCount > 0 && RequiredSafeCount < 1) {
			// TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
			ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
			*ItemSlot = ItemSlotCopy;
			DestroyItem = false;
		}
	}

	if (DestroyItem) {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	}

	Client->UpgradePoint = UpgradePoint;
	Character->SyncMask.InventoryInfo = true;

	S2C_DATA_UPDATE_UPGRADE_POINTS* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPDATE_UPGRADE_POINTS);
	Notification->UpgradePoint = Client->UpgradePoint;
	Notification->Timestamp = (UInt32)GetTimestampMs() + 1000 * 60 * 60;
	SocketSend(Socket, Connection, Notification);

	S2C_DATA_UPGRADE_CHAOS_LEVEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPGRADE_CHAOS_LEVEL);
	Response->Result = Result;
	Response->RemainingCoreCount = RemainingCoreCount;
	Response->ConsumedSafeCount = ConsumedSafeCount;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(DIVINE_UPGRADE_ITEM_LEVEL) {
	if (!Character) goto error;

	Int32 TailLength = sizeof(UInt16) * (Packet->CoreCount + Packet->SafeCount);
	if (sizeof(S2C_DATA_DIVINE_UPGRADE_ITEM_LEVEL) + TailLength > Packet->Length) goto error;

	if (Packet->CoreCount < 1) goto error;

	UInt16* CoreSlotIndices = &Packet->CoreSlotIndices[0];
	UInt16* SafeSlotIndices = &Packet->CoreSlotIndices[Packet->CoreCount];

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	if (!ItemSlot) goto error;

	struct _RTItemSlot ItemSlotCopy = *ItemSlot;
	Int32 DivineLevel = ItemSlot->Item.DivineLevel;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) goto error;

	RTDataDivineUpgradeMainRef DivineUpgradeMain = RTRuntimeDataDivineUpgradeMainGet(Runtime->Context, ItemData->ItemGrade, ItemData->ItemType);
	if (!DivineUpgradeMain) goto error;

	RTDataDivineUpgradeGroupCostRef GroupCost = RTRuntimeDataDivineUpgradeGroupCostGet(Runtime->Context, DivineUpgradeMain->Group);
	if (!GroupCost) goto error;

	RTDataDivineUpgradeGroupCostLevelRef GroupCostLevel = RTRuntimeDataDivineUpgradeGroupCostLevelGet(GroupCost, ItemSlot->Item.DivineLevel);
	if (!GroupCostLevel) goto error;

	RTItemSlotRef CoreSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, CoreSlotIndices[0]);
	assert(CoreSlot);

	RTItemDataRef CoreData = RTRuntimeGetItemDataByIndex(Runtime, CoreSlot->Item.ID);
	assert(CoreData);

	if (CoreData->ItemType != RUNTIME_ITEM_TYPE_DIVINE_CORE &&
		CoreData->ItemType != RUNTIME_ITEM_TYPE_DIVINE_CORE_SET) goto error;

	Int32 RemainingCoreCount = 0;
	Int32 RemainingSafeCount = 0;
	Int32 RequiredCoreCount = GroupCostLevel->RequiredCoreCount;
	Int32 RequiredSafeCount = GroupCostLevel->RequiredSafeCount;

	// TODO: We have to snapshot inventory here to rollback!!!
	for (Int Index = 0; Index < Packet->CoreCount; Index += 1) {
		RTItemSlotRef CoreSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, CoreSlotIndices[Index]);
		assert(CoreSlot);

		RTItemDataRef CoreData = RTRuntimeGetItemDataByIndex(Runtime, CoreSlot->Item.ID);
		assert(CoreData);

		if (CoreData->ItemType == RUNTIME_ITEM_TYPE_DIVINE_CORE) {
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, CoreSlot->SlotIndex);
			RequiredCoreCount -= 1;
		}
		else if (CoreData->ItemType == RUNTIME_ITEM_TYPE_DIVINE_CORE_SET) {
			Int32 ConsumableCoreCount = MIN(RequiredCoreCount, (Int32)CoreSlot->ItemOptions);

			CoreSlot->ItemOptions -= ConsumableCoreCount;
			RemainingCoreCount = (Int32)CoreSlot->ItemOptions;
			RequiredCoreCount -= ConsumableCoreCount;

			if (CoreSlot->ItemOptions < 1) {
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, CoreSlot->SlotIndex);
			}
		}

		if (RequiredCoreCount < 1) {
			break;
		}
	}

	if (RequiredCoreCount > 0) goto error;

	// TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
	ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);

	Int32 ResultLevel = 0;
	Int32 Seed = (Int32)PlatformGetTickCount();
	Int32 Result = RTItemUpgradeDivine(
		Runtime,
		ItemSlot,
		&Seed,
		&ResultLevel
	);

	Int32 ConsumedSafeCount = 0;
	if (Result == RUNTIME_DIVINE_UPGRADE_RESULT_DOWNGRADE) {
		for (Int Index = 0; Index < Packet->SafeCount; Index += 1) {
			RTItemSlotRef SafeSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, SafeSlotIndices[Index]);
			assert(SafeSlot);

			RTItemDataRef SafeData = RTRuntimeGetItemDataByIndex(Runtime, SafeSlot->Item.ID);
			assert(SafeData);

			if (SafeData->ItemType != RUNTIME_ITEM_TYPE_DIVINE_SAFE_GUARD) goto error;

			Int32 ConsumableSafeCount = MIN(RequiredSafeCount, (Int32)SafeSlot->ItemOptions);

			SafeSlot->ItemOptions -= ConsumableSafeCount;
			RemainingSafeCount = (Int32)SafeSlot->ItemOptions;
			RequiredSafeCount -= ConsumableSafeCount;
			ConsumedSafeCount += ConsumableSafeCount;

			if (SafeSlot->ItemOptions < 1) {
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SafeSlot->SlotIndex);
			}
		}

		if (ConsumedSafeCount > 0 && RequiredSafeCount < 1) {
			// TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
			ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
			*ItemSlot = ItemSlotCopy;
		}
	}

	Character->SyncMask.InventoryInfo = true;

	S2C_DATA_DIVINE_UPGRADE_ITEM_LEVEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIVINE_UPGRADE_ITEM_LEVEL);
	Response->Result = Result;
	Response->ResultLevel = ResultLevel;
	Response->ConsumedSafeguardCount = ConsumedSafeCount;
	Response->RemainingSafeguardCount = RemainingCoreCount;
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_DIVINE_UPGRADE_ITEM_LEVEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIVINE_UPGRADE_ITEM_LEVEL);
		Response->Result = RUNTIME_DIVINE_UPGRADE_RESULT_ERROR;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(DIVINE_UPGRADE_SEAL) {
	if (!Character) goto error;

	RTItemSlotRef SourceSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->SourceSlotIndex);
	if (!SourceSlot) goto error;

	RTItemDataRef SourceData = RTRuntimeGetItemDataByIndex(Runtime, SourceSlot->Item.ID);
	if (!SourceData) goto error;

	RTItemSlotRef TargetSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->TargetSlotIndex);
	if (!TargetSlot) goto error;

	RTItemDataRef TargetData = RTRuntimeGetItemDataByIndex(Runtime, TargetSlot->Item.ID);
	if (!TargetData) goto error;

	RTDataDivineUpgradeMainRef DivineUpgradeMain = RTRuntimeDataDivineUpgradeMainGet(Runtime->Context, TargetData->ItemGrade, TargetData->ItemType);
	if (!DivineUpgradeMain) goto error;

	RTItemOptions SourceItemOptions = { .Serial = SourceSlot->ItemOptions };
	if (SourceSlot->ItemOptions) {
		if (TargetSlot->Item.DivineLevel > 0) goto error;
		if (SourceItemOptions.DivineSeal.ItemCategory != DivineUpgradeMain->ItemCategory) goto error;
		if (SourceItemOptions.DivineSeal.ItemGrade != TargetData->ItemGrade) goto error;

		TargetSlot->Item.DivineLevel = SourceItemOptions.DivineSeal.ItemLevel;
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceSlot->SlotIndex);
		Character->SyncMask.InventoryInfo = true;

		S2C_DATA_DIVINE_UPGRADE_SEAL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIVINE_UPGRADE_SEAL);
		Response->Result = S2C_DIVINE_UPGRADE_SEAL_RESULT_UNSEAL;
		Response->ResultSerial = TargetSlot->Item.Serial;
		SocketSend(Socket, Connection, Response);
		return;
	}
	else {
		if (TargetSlot->Item.DivineLevel < 1) goto error;

		SourceItemOptions.DivineSeal.ItemLevel = TargetSlot->Item.DivineLevel;
		SourceItemOptions.DivineSeal.ItemGrade = TargetData->ItemGrade;
		SourceItemOptions.DivineSeal.ItemCategory = DivineUpgradeMain->ItemCategory;
		SourceSlot->ItemOptions = SourceItemOptions.Serial;
		TargetSlot->Item.DivineLevel = 0;
		Character->SyncMask.InventoryInfo = true;

		S2C_DATA_DIVINE_UPGRADE_SEAL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIVINE_UPGRADE_SEAL);
		Response->Result = S2C_DIVINE_UPGRADE_SEAL_RESULT_SEAL;
		Response->ResultSerial = SourceItemOptions.Serial;
		SocketSend(Socket, Connection, Response);
		return;
	}

error:
	{
		S2C_DATA_DIVINE_UPGRADE_SEAL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIVINE_UPGRADE_SEAL);
		Response->Result = S2C_DIVINE_UPGRADE_SEAL_RESULT_ERROR;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(EXTREME_UPGRADE_SEAL) {
	if (!Character) goto error;

	RTItemSlotRef SourceSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->SourceSlotIndex);
	if (!SourceSlot) goto error;

	RTItemDataRef SourceData = RTRuntimeGetItemDataByIndex(Runtime, SourceSlot->Item.ID);
	if (!SourceData) goto error;

	RTItemSlotRef TargetSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->TargetSlotIndex);
	if (!TargetSlot) goto error;

	RTItemDataRef TargetData = RTRuntimeGetItemDataByIndex(Runtime, TargetSlot->Item.ID);
	if (!TargetData) goto error;

    RTDataItemType TargetItemType = TargetData->ItemType;
    if (TargetItemType == RUNTIME_ITEM_TYPE_HELMED2) TargetItemType = RUNTIME_ITEM_TYPE_HELMED1;

	if (SourceData->ItemType != RUNTIME_ITEM_TYPE_EXTREME_SEAL_STONE) goto error;

	S2C_DATA_EXTREME_UPGRADE_SEAL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, EXTREME_UPGRADE_SEAL);
	Response->IsUnsealing = SourceSlot->ItemOptions > 0;
	if (Response->IsUnsealing) {
		if (TargetSlot->Item.ExtremeLevel > 0) goto error;
		if (TargetSlot->Item.UpgradeLevel < 15) goto error;

		RTDataExtremeUpgradeBaseGradeRef ExtremeUpgradeBaseGrade = NULL;
		for (Int Index = 0; Index < Runtime->Context->ExtremeUpgradeBaseCount; Index += 1) {
			RTDataExtremeUpgradeBaseRef ExtremeUpgradeBase = &Runtime->Context->ExtremeUpgradeBaseList[Index];
			if (ExtremeUpgradeBase->Type != TargetItemType) continue;

			ExtremeUpgradeBaseGrade = RTRuntimeDataExtremeUpgradeBaseGradeGet(ExtremeUpgradeBase, TargetData->ItemGrade);
			if (ExtremeUpgradeBaseGrade) break;
		}
		if (!ExtremeUpgradeBaseGrade) goto error;
		
		Int32 SourceExtremeLevel = SourceSlot->ItemOptions & 0b111;
		if (SourceExtremeLevel > ExtremeUpgradeBaseGrade->ExtremeUpgradeMax) goto error;
		
		RTDataExtremeUpgradeCategoryRef SourceCategory = RTRuntimeDataExtremeUpgradeCategoryGetByCategory(Runtime->Context, (UInt32)(SourceSlot->ItemOptions >> 8));
		if (!SourceCategory) goto error;

		RTDataExtremeUpgradeCategoryRef TargetCategory = RTRuntimeDataExtremeUpgradeCategoryGet(Runtime->Context, TargetItemType);
		if (!TargetCategory) goto error;

		if (SourceCategory->Group != TargetCategory->Group) goto error;

		TargetSlot->Item.ExtremeLevel = SourceExtremeLevel;
		Response->ItemID = TargetSlot->Item;

		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceSlot->SlotIndex);
		Character->SyncMask.InventoryInfo = true;
	}
	else {
		if (SourceSlot->ItemOptions > 0) goto error;
		if (TargetSlot->Item.IsBroken) goto error;
		if (TargetSlot->Item.ExtremeLevel < SourceData->ExtremeSealStone.MinLevel) goto error;
		if (TargetSlot->Item.ExtremeLevel > SourceData->ExtremeSealStone.MaxLevel) goto error;

		RTDataExtremeUpgradeCategoryRef TargetCategory = RTRuntimeDataExtremeUpgradeCategoryGet(Runtime->Context, TargetItemType);
		if (!TargetCategory) goto error;

		SourceSlot->ItemOptions = ((UInt64)TargetCategory->Category << 8) | TargetSlot->Item.ExtremeLevel;
		TargetSlot->Item.ExtremeLevel = 0;
		Response->ItemID.Serial = SourceSlot->ItemOptions;
		Character->SyncMask.InventoryInfo = true;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CHAOS_UPGRADE_SEAL) {
	if (!Character) goto error;

	RTItemSlotRef SourceSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->SourceSlotIndex);
	if (!SourceSlot) goto error;

	RTItemDataRef SourceData = RTRuntimeGetItemDataByIndex(Runtime, SourceSlot->Item.ID);
	if (!SourceData) goto error;

	RTItemSlotRef TargetSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->TargetSlotIndex);
	if (!TargetSlot) goto error;

	RTItemDataRef TargetData = RTRuntimeGetItemDataByIndex(Runtime, TargetSlot->Item.ID);
	if (!TargetData) goto error;

	RTDataChaosUpgradeItemListRef ChaosUpgradeItemList = RTRuntimeDataChaosUpgradeItemListGet(Runtime->Context, TargetSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX);
	if (!ChaosUpgradeItemList) goto error;

	RTItemOptions SourceItemOptions = { .Serial = SourceSlot->ItemOptions };
	if (SourceSlot->ItemOptions) {
		if (TargetSlot->Item.UpgradeLevel > 0) goto error;
		if (SourceItemOptions.ChaosSeal.ItemType != TargetData->ItemType) goto error;
		if (SourceItemOptions.ChaosSeal.ItemGroup != ChaosUpgradeItemList->Group) goto error;

		TargetSlot->Item.UpgradeLevel = SourceItemOptions.ChaosSeal.ItemLevel;
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceSlot->SlotIndex);
		Character->SyncMask.InventoryInfo = true;

		S2C_DATA_CHAOS_UPGRADE_SEAL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CHAOS_UPGRADE_SEAL);
		Response->Result = S2C_CHAOS_UPGRADE_SEAL_RESULT_UNSEAL;
		Response->ResultSerial = TargetSlot->Item.Serial;
		SocketSend(Socket, Connection, Response);
		return;
	}
	else {
		if (TargetSlot->Item.UpgradeLevel < 1) goto error;

		SourceItemOptions.ChaosSeal.ItemLevel = TargetSlot->Item.UpgradeLevel;
		SourceItemOptions.ChaosSeal.ItemGroup = ChaosUpgradeItemList->Group;
		SourceItemOptions.ChaosSeal.ItemType = TargetData->ItemType;
		SourceSlot->ItemOptions = SourceItemOptions.Serial;
		TargetSlot->Item.UpgradeLevel = 0;
		Character->SyncMask.InventoryInfo = true;

		S2C_DATA_CHAOS_UPGRADE_SEAL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CHAOS_UPGRADE_SEAL);
		Response->Result = S2C_CHAOS_UPGRADE_SEAL_RESULT_SEAL;
		Response->ResultSerial = SourceItemOptions.Serial;
		SocketSend(Socket, Connection, Response);
		return;
	}

error:
	{
		S2C_DATA_CHAOS_UPGRADE_SEAL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CHAOS_UPGRADE_SEAL);
		Response->Result = S2C_CHAOS_UPGRADE_SEAL_RESULT_ERROR;
		SocketSend(Socket, Connection, Response);
	}
}
