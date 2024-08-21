#include "Character.h"
#include "Force.h"
#include "Item.h"
#include "Inventory.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

// TODO: This is most likely encoded into item data!
Bool RTItemTypeIsProtectable(
	UInt32 ItemType
) {
	switch (ItemType) {
	case RUNTIME_ITEM_TYPE_HELMED1:
	case RUNTIME_ITEM_TYPE_HELMED2:
	case RUNTIME_ITEM_TYPE_HEADGEAR3:
	case RUNTIME_ITEM_TYPE_SUIT:
	case RUNTIME_ITEM_TYPE_GLOVES:
	case RUNTIME_ITEM_TYPE_BOOTS:
	case RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND:
	case RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND:
	case RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER:
	case RUNTIME_ITEM_TYPE_CHAKRAM:
	case RUNTIME_ITEM_TYPE_EPAULET:
	case RUNTIME_ITEM_TYPE_EPAULET2:
	case RUNTIME_ITEM_TYPE_AMULET:
	case RUNTIME_ITEM_TYPE_RING:
	case RUNTIME_ITEM_TYPE_VEHICLE_BIKE:
	case RUNTIME_ITEM_TYPE_SUMMONING_PET:
	case RUNTIME_ITEM_TYPE_EARRING:
	case RUNTIME_ITEM_TYPE_BRACELET: 
	case RUNTIME_ITEM_TYPE_BELT:
	case RUNTIME_ITEM_TYPE_CHARM: 
	case RUNTIME_ITEM_TYPE_EFFECTOR:
	case RUNTIME_ITEM_TYPE_LINK_WEAPON:
	case RUNTIME_ITEM_TYPE_UNIQUE_COSTUME:
	case RUNTIME_ITEM_TYPE_DIALOGUE_BUBBLE:
	case RUNTIME_ITEM_TYPE_TALISMAN:
	case RUNTIME_ITEM_TYPE_CARNELIAN:
	case RUNTIME_ITEM_TYPE_ARCANA:
		return true;

	default:
		return false;
	}
}

Bool RTCharacterCheckItemStatRequirements(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemDataRef ItemData,
	Int32 ItemUpgradeLevel
) {
	Int64 RequiredStr = ItemData->Weapon.Str + ItemData->Weapon.DeltaStr * (1 + MIN(ItemUpgradeLevel, RUNTIME_ENCHANT_MAX_STAT_INCREASE_LEVEL));
	Int64 RequiredDex = ItemData->Weapon.Dex + ItemData->Weapon.DeltaDex * (1 + MIN(ItemUpgradeLevel, RUNTIME_ENCHANT_MAX_STAT_INCREASE_LEVEL));
	Int64 RequiredInt = ItemData->Weapon.Int + ItemData->Weapon.DeltaInt * (1 + MIN(ItemUpgradeLevel, RUNTIME_ENCHANT_MAX_STAT_INCREASE_LEVEL));

	Int64 CurrentStr = Character->Attributes.Values[RUNTIME_ATTRIBUTE_STAT_STR];
	Int64 CurrentDex = Character->Attributes.Values[RUNTIME_ATTRIBUTE_STAT_DEX];
	Int64 CurrentInt = Character->Attributes.Values[RUNTIME_ATTRIBUTE_STAT_INT];

	if (CurrentStr < RequiredStr || CurrentDex < RequiredDex || CurrentInt < RequiredInt) {
		return false;
	}

	return true;
}

Void RTCharacterApplyItemUpgradeForceEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemDataRef ItemData,
	Int32 ItemUpgradeLevel
) {
    RTDataItemType ItemType = ItemData->ItemType;
    if (ItemType == RUNTIME_ITEM_TYPE_HELMED2) ItemType = RUNTIME_ITEM_TYPE_HELMED1;

	RTDataUpgradeItemBasicRef UpgradeItemBasic = RTRuntimeDataUpgradeItemBasicGet(Runtime->Context, ItemType);
	RTDataUpgradeItemBasicGradeRef UpgradeItemBasicGrade = (UpgradeItemBasic) ? RTRuntimeDataUpgradeItemBasicGradeGet(UpgradeItemBasic, ItemData->ItemGrade) : NULL;
	if (UpgradeItemBasicGrade) {
		for (Int32 Index = 0; Index < UpgradeItemBasicGrade->UpgradeItemBasicGradeValueCount; Index += 1) {
			RTDataUpgradeItemBasicGradeValueRef Value = &UpgradeItemBasicGrade->UpgradeItemBasicGradeValueList[Index];

			Int32 ForceValue = 0;
			for (Int32 Level = 0; Level < MIN(ItemUpgradeLevel, RUNTIME_ENCHANT_UPGRADE_BASIC_LEVEL_LIMIT); Level += 1) {
				Int32 SlopeMultiplier = MAX(0, MIN(RUNTIME_ENCHANT_SLOPE_MULTIPLIER_LIMIT, ((Int32)Level / RUNTIME_ENCHANT_SLOPE_DIVIDER)));
				ForceValue += Value->ForceValueFormula[0] + Value->ForceValueFormula[1] * SlopeMultiplier;
			}

			RTCharacterApplyForceEffect(
				Runtime,
				Character,
				Value->ForceID,
				ForceValue,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}
	}

	RTDataUpgradeItemRef UpgradeItem = RTRuntimeDataUpgradeItemGet(Runtime->Context, ItemData->ItemType);
	RTDataUpgradeItemGradeRef UpgradeItemGrade = (UpgradeItem) ? RTRuntimeDataUpgradeItemGradeGet(UpgradeItem, ItemData->ItemGrade) : NULL;
	if (UpgradeItemGrade) {
		for (Int32 Index = 0; Index < UpgradeItemGrade->UpgradeItemGradeValueCount; Index += 1) {
			RTDataUpgradeItemGradeValueRef Value = &UpgradeItemGrade->UpgradeItemGradeValueList[Index];
			if (ItemUpgradeLevel < Value->ApplyLevel) continue;

			Int32 ForceValue = Value->ForceValueFormula[2];
			for (Int32 Level = 0; Level < (ItemUpgradeLevel - Value->ApplyLevel + 1); Level += 1) {
				Int32 SlopeMultiplier = Level + 1;
				ForceValue += Value->ForceValueFormula[0] + Value->ForceValueFormula[1] * SlopeMultiplier;
			}

			RTCharacterApplyForceEffect(
				Runtime,
				Character,
				Value->ForceID,
				ForceValue,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}
	}
}

Void RTCharacterApplyItemForceOptionEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemSlotRef ItemSlot,
	RTItemDataRef ItemData
) {
	RTItemOptions ItemOptions = { .Serial = ItemSlot->ItemOptions };

	Int32 ForceSlotOffset = 0;

	if (ItemData->MasterGrade) {
		RTItemOptionSlot MasterSlot = ItemOptions.Equipment.Slots[0];

		RTDataMasterItemRef MasterItem = RTRuntimeDataMasterItemGet(Runtime->Context, ItemData->MasterGrade);
		if (MasterItem) {
			RTDataMasterItemOptionRef MasterItemOption = RTRuntimeDataMasterItemOptionGet(MasterItem, MasterSlot.MasterIndex);
			for (Int32 Index = 0; Index < MasterItemOption->MasterItemOptionValueCount; Index += 1) {
				RTDataMasterItemOptionValueRef MasterItemOptionValue = &MasterItemOption->MasterItemOptionValueList[Index];

				RTCharacterApplyForceEffect(
					Runtime,
					Character,
					MasterItemOptionValue->ForceEffectIndex,
					MasterItemOptionValue->ForceValue,
					MasterItemOptionValue->ForceValueType == 1 ? RUNTIME_FORCE_VALUE_TYPE_ADDITIVE : RUNTIME_FORCE_VALUE_TYPE_MULTIPLICATIVE
				);
			}
		}

		ForceSlotOffset = 1;
	}

	// TODO: Check unique items
	// <unique_item	item_index="3343"	>

	for (Int32 Index = ForceSlotOffset; Index < RUNTIME_ITEM_MAX_OPTION_COUNT; Index += 1) {
		RTItemOptionSlot Slot = ItemOptions.Equipment.Slots[Index];
		if (Slot.Serial < 1) continue;

		RTDataForceCodeFormula Formula = RTRuntimeDataForceCodeFormulaGet(
			Runtime->Context,
			ItemData->ItemType,
			ItemData->ItemGrade,
			ItemData->UniqueGrade,
			Slot.ForceIndex,
			Slot.ForceLevel,
			ItemSlot->Item.IsAccountBinding,
			Slot.IsEpic
		);

		if (Formula.ForceEffectIndex > 0) {
			RTCharacterApplyForceEffect(
				Runtime,
				Character,
				Formula.ForceEffectIndex,
				Formula.ForceValue,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}
	}

	if (ItemOptions.Equipment.ExtraForceIndex) {
		RTDataForceCodeFormula Formula = RTRuntimeDataForceCodeFormulaGet(
			Runtime->Context,
			ItemData->ItemType,
			ItemData->ItemGrade,
			ItemData->UniqueGrade,
			ItemOptions.Equipment.ExtraForceIndex,
			1,
			ItemSlot->Item.IsAccountBinding,
			false
		);

		if (Formula.ForceEffectIndex > 0) {
			RTCharacterApplyForceEffect(
				Runtime,
				Character,
				Formula.ForceEffectIndex,
				Formula.ForceValue,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}
	}
}

Void RTCharacterApplyItemExtremeLevelEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemSlotRef ItemSlot,
	RTItemDataRef ItemData
) {
	if (ItemSlot->Item.ExtremeLevel > 0) {
		RTDataItemType TargetItemType = ItemData->ItemType;
		if (TargetItemType == RUNTIME_ITEM_TYPE_HELMED2) TargetItemType = RUNTIME_ITEM_TYPE_HELMED1;

		RTDataExtremeUpgradeValueRef ExtremeUpgradeValue = NULL;
		RTDataExtremeUpgradeBaseGradeRef ExtremeUpgradeBaseGrade = NULL;
		for (Index Index = 0; Index < Runtime->Context->ExtremeUpgradeBaseCount; Index += 1) {
			RTDataExtremeUpgradeBaseRef ExtremeUpgradeBase = &Runtime->Context->ExtremeUpgradeBaseList[Index];
			if (ExtremeUpgradeBase->Type != TargetItemType) continue;

			RTDataExtremeUpgradeBaseGradeRef ExtremeUpgradeBaseGrade = RTRuntimeDataExtremeUpgradeBaseGradeGet(ExtremeUpgradeBase, ItemData->ItemGrade);
			RTDataExtremeUpgradeMainRef ExtremeUpgradeMain = (ExtremeUpgradeBaseGrade) ? RTRuntimeDataExtremeUpgradeMainGet(Runtime->Context, ExtremeUpgradeBaseGrade->ExtremeUpgradeGroup) : NULL;
			RTDataExtremeUpgradeMainLevelRef ExtremeUpgradeMainLevel = (ExtremeUpgradeMain) ? RTRuntimeDataExtremeUpgradeMainLevelGet(ExtremeUpgradeMain, ItemSlot->Item.ExtremeLevel) : NULL;
			ExtremeUpgradeValue = (ExtremeUpgradeMainLevel) ? RTRuntimeDataExtremeUpgradeValueGet(Runtime->Context, ExtremeUpgradeMainLevel->ExtremeUpgradeOption) : NULL;
			if (ExtremeUpgradeValue) break;
		}

		if (ExtremeUpgradeValue) {
			for (Int32 Index = 0; Index < ExtremeUpgradeValue->ExtremeUpgradeValueOptionCount; Index += 1) {
				RTDataExtremeUpgradeValueOptionRef ExtremeUpgradeValueOption = &ExtremeUpgradeValue->ExtremeUpgradeValueOptionList[Index];
				RTCharacterApplyForceEffect(
					Runtime,
					Character,
					ExtremeUpgradeValueOption->ForceEffectIndex,
					ExtremeUpgradeValueOption->ForceValue,
					ExtremeUpgradeValueOption->ForceValueType == 1 ? RUNTIME_FORCE_VALUE_TYPE_ADDITIVE : RUNTIME_FORCE_VALUE_TYPE_MULTIPLICATIVE
				);
			}
		}
	}
}

Void RTCharacterApplyItemChaosLevelEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemSlotRef ItemSlot,
	RTItemDataRef ItemData
) {
	RTDataChaosUpgradeItemListRef UpgradeMain = RTRuntimeDataChaosUpgradeItemListGet(Runtime->Context, ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX);
	RTDataChaosUpgradeGroupValueRef UpgradeGroupValue = (UpgradeMain) ? RTRuntimeDataChaosUpgradeGroupValueGet(Runtime->Context, UpgradeMain->Group) : NULL;
	if (UpgradeGroupValue) {
		for (Int32 Index = 0; Index < UpgradeGroupValue->ChaosUpgradeGroupValueLevelCount; Index += 1) {
			RTDataChaosUpgradeGroupValueLevelRef UpgradeGroupValueLevel = &UpgradeGroupValue->ChaosUpgradeGroupValueLevelList[Index];
			if (UpgradeGroupValueLevel->Level != ItemSlot->Item.UpgradeLevel) continue;

			RTCharacterApplyForceEffect(
				Runtime,
				Character,
				UpgradeGroupValueLevel->ForceEffectIndex,
				UpgradeGroupValueLevel->ForceValue,
				UpgradeGroupValueLevel->ForceValueType == 1 ? RUNTIME_FORCE_VALUE_TYPE_ADDITIVE : RUNTIME_FORCE_VALUE_TYPE_MULTIPLICATIVE
			);
		}
	}
}

Void RTCharacterApplyItemDivineLevelEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemSlotRef ItemSlot,
	RTItemDataRef ItemData
) {
	if (ItemSlot->Item.DivineLevel > 0) {
		RTDataDivineUpgradeMainRef DivineUpgradeMain = RTRuntimeDataDivineUpgradeMainGet(Runtime->Context, ItemData->ItemGrade, ItemData->ItemType);
		RTDataDivineUpgradeGroupValueRef DivineUpgradeGroupValue = (DivineUpgradeMain) ? RTRuntimeDataDivineUpgradeGroupValueGet(Runtime->Context, DivineUpgradeMain->Group) : NULL;
		if (DivineUpgradeGroupValue) {
			for (Int32 Index = 0; Index < DivineUpgradeGroupValue->DivineUpgradeGroupValueLevelCount; Index += 1) {
				RTDataDivineUpgradeGroupValueLevelRef DivineUpgradeGroupValueLevel = &DivineUpgradeGroupValue->DivineUpgradeGroupValueLevelList[Index];
				if (DivineUpgradeGroupValueLevel->Level != ItemSlot->Item.DivineLevel) continue;

				RTCharacterApplyForceEffect(
					Runtime,
					Character,
					DivineUpgradeGroupValueLevel->ForceEffectIndex,
					DivineUpgradeGroupValueLevel->ForceValue,
					DivineUpgradeGroupValueLevel->ForceValueType == 1 ? RUNTIME_FORCE_VALUE_TYPE_ADDITIVE : RUNTIME_FORCE_VALUE_TYPE_MULTIPLICATIVE
				);
			}
		}
	}
}

RTItemHonorMedalSealData RTItemHonorMedalSealDecode(
	RTRuntimeRef Runtime,
	UInt64 Serial
) {
	RTItemHonorMedalSealData Data = { 0 };
	Data.Group = (Serial >> 60) & 0xF;

	for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_HONOR_MEDAL_SEAL_SLOT_COUNT; Index += 1) {
		UInt8 Option = (Serial >> (Index * 6)) & 0b111111;
		RTDataHonorMedalSealRef Seal = RTRuntimeDataHonorMedalSealGet(Runtime->Context, Option);
		if (!Seal) continue;

		Data.ForceEffectIndex[Index] = (Seal) ? Seal->ForceEffectIndex : 0;
	}

	return Data;
}

UInt64 RTItemHonorMedalSealEncode(
	RTRuntimeRef Runtime,
	RTItemHonorMedalSealData Data
) {
	UInt64 Serial = 0;

	for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_HONOR_MEDAL_SEAL_SLOT_COUNT; Index += 1) {
		RTDataHonorMedalSealRef Seal = RTRuntimeDataHonorMedalSealGetByForceID(Runtime->Context, Data.ForceEffectIndex[Index]);
		if (!Seal) continue;

		Serial |= (((UInt64)Seal->Option & 0b111111) << (Index * 6));
	}

	Serial |= ((UInt64)Data.Group & 0xF) << 60;
	return Serial;
}

RTItemForceOptionData RTItemForceOptionDecode(
	UInt64 Serial
) {
	RTItemForceOptionData Data = { 0 };
	RTItemOptions Options = { 0 };
	Options.Serial = Serial;

	for (Int32 Index = 0; Index < RUNTIME_ITEM_MAX_OPTION_COUNT; Index += 1) {
		RTItemOptionSlot Slot = Options.Equipment.Slots[Index];
		if (Slot.IsEpic) {
			assert(Data.EpicSlotCount < RUNTIME_ITEM_MAX_OPTION_COUNT);
			Data.EpicSlots[Data.EpicSlotCount] = Slot;
			Data.EpicSlotCount += 1;
		}
		else {
			for (Int32 Offset = 0; Offset < Slot.ForceLevel; Offset += 1) {
				assert(Data.FilledForceSlotCount < RUNTIME_ITEM_MAX_OPTION_COUNT);
				Data.ForceSlots[Data.FilledForceSlotCount] = Slot.ForceIndex;
				Data.FilledForceSlotCount += 1;
			}
		}
	}

	Data.ForceSlotCount = Options.Equipment.SlotCount;
	return Data;
}

Int32 RTItemForceOptionRemoveForceIndex(
	RTItemForceOptionData* Data,
	UInt8 ForceIndex
) {
	Int32 Count = 0;

	for (Int32 Index = 0; Index < Data->FilledForceSlotCount; Index += 1) {
		if (Data->ForceSlots[Index] != ForceIndex) continue;

		for (Int32 MoveIndex = Index; MoveIndex < Data->FilledForceSlotCount - 1; MoveIndex += 1) {
			Data->ForceSlots[MoveIndex] = Data->ForceSlots[MoveIndex + 1];
		}

		Count += 1;
		Index -= 1;
		Data->FilledForceSlotCount -= 1;
	}

	return Count;
}

Bool RTItemForceOptionInsertForceIndex(
	RTItemForceOptionData* Data,
	UInt8 ForceIndex
) {
	for (Int32 Index = 0; Index < Data->FilledForceSlotCount; Index += 1) {
		if (Data->ForceSlots[Index] != ForceIndex) continue;

		if (Data->FilledForceSlotCount >= RUNTIME_ITEM_MAX_OPTION_COUNT) {
			return 0;
		}

		Int32 TailLength = RUNTIME_ITEM_MAX_OPTION_COUNT - Index - 2;
		if (TailLength > 0) {
			memmove(
				&Data->ForceSlots[Index + 2], 
				&Data->ForceSlots[Index + 1], 
				TailLength * sizeof(UInt8)
			);
		}

		Data->ForceSlots[Index + 1] = Data->ForceSlots[Index];
		Data->FilledForceSlotCount += 1;
		return true;
	}

	return false;
}

UInt64 RTItemForceOptionEncode(
	RTItemForceOptionData Data
) {
	RTItemOptions Options = { 0 };
	Options.Equipment.SlotCount = Data.ForceSlotCount;

	Int32 NextSlotIndex = 0;
	for (Int32 Index = 0; Index < Data.EpicSlotCount; Index += 1) {
		RTItemOptionSlot Slot = Data.EpicSlots[Index];
		if (Slot.Serial > 0) {
			assert(NextSlotIndex < RUNTIME_ITEM_MAX_OPTION_COUNT);
			Options.Equipment.Slots[NextSlotIndex].Serial = Slot.Serial;
			NextSlotIndex += 1;
		}
	}

	for (Int32 Index = 0; Index < Data.FilledForceSlotCount; Index += 1) {
		UInt8 ForceIndex = Data.ForceSlots[Index];
		if (NextSlotIndex > 0 &&
			!Options.Equipment.Slots[NextSlotIndex - 1].IsEpic &&
			Options.Equipment.Slots[NextSlotIndex - 1].ForceIndex == ForceIndex) {
			Options.Equipment.Slots[NextSlotIndex - 1].ForceLevel += 1;
		}
		else if (ForceIndex > 0) {
			if (NextSlotIndex >= RUNTIME_ITEM_MAX_OPTION_COUNT) {
				assert(!Options.Equipment.ExtraForceIndex);
				Options.Equipment.ExtraForceIndex = ForceIndex;
				NextSlotIndex += 1;
			}
			else {
				Options.Equipment.Slots[NextSlotIndex].ForceIndex = ForceIndex;
				Options.Equipment.Slots[NextSlotIndex].ForceLevel = 1;
				NextSlotIndex += 1;
			}
		}
	}
	
	return Options.Serial;
}

UInt64 RTItemDataGetStackSizeMask(
	RTItemDataRef ItemData
) {
	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_QUEST_S ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_SPECIAL_POTION ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_FORCE_CORE_ORDER_SHEET) {
		return RUNTIME_QUEST_ITEM_COUNT_MASK;
	}

	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_EVENT_S ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_FONT_S) {
		// TODO: Check how the item options memory is layouted...
		return 0;
	}

	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_BOSS_ARENA_TOKEN ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_POTION_STACKABLE ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_TIME_REDUCER) {
		return RUNTIME_SHORT_ITEM_COUNT_MASK;
	}

	return UINT64_MAX;
}

UInt64 RTItemDataGetItemOptionsOffset(
	RTItemDataRef ItemData
) {
	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_QUEST_S ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_SPECIAL_POTION ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_FORCE_CORE_ORDER_SHEET) {
		return 7;
	}

	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_EVENT_S ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_FONT_S) {
		// TODO: Check how the item options memory is layouted...
		return 0;
	}

	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_BOSS_ARENA_TOKEN ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_POTION_STACKABLE ||
		ItemData->ItemType == RUNTIME_ITEM_TYPE_TIME_REDUCER) {
		return 16;
	}

	return 0;
}

Int32 RTItemUseInternal(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemSlotRef ItemSlot,
	RTItemDataRef ItemData,
	Void* Payload
) {
#define RUNTIME_ITEM_PROCEDURE(__NAME__, __TYPE__, __INTERNAL__)	    \
	if (ItemData->ItemType == __TYPE__) {								\
		return __NAME__(												\
			Runtime,													\
			Character,													\
			ItemSlot,													\
			ItemData,													\
			Payload	        											\
		);																\
	}
#include <RuntimeLib/ItemProcDefinition.h>

	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

Int32 RTItemOptionFirstEpicSlotIndex(
	RTItemOptions* Options
) {
	for (Int32 Index = 0; Index < RUNTIME_ITEM_MAX_OPTION_COUNT; Index += 1) {
		if (Options->Equipment.Slots[Index].IsEpic) return Index;
	}

	return -1;
}

Int32 RTItemOptionLastEmptySlotIndex(
	RTItemOptions* Options
) {
	for (Int32 Index = RUNTIME_ITEM_MAX_OPTION_COUNT - 1; Index >= 0; Index -= 1) {
		UInt8 Value = *((UInt8*)&Options->Equipment.Slots[Index]);
		if (!Value) return Index;
	}

	return -1;
}

Bool RTItemOptionHasEpic(
	RTItemOptions Options
) {
	for (Int32 Index = 0; Index < RUNTIME_ITEM_MAX_OPTION_COUNT; Index += 1) {
		if (Options.Equipment.Slots[Index].IsEpic) return true;
	}

	return false;
}

RTItemOptionSlot* RTItemOptionGetLastEmptyForceSlot(
	RTItemOptions Options
) {
	for (Int32 Index = RUNTIME_ITEM_MAX_OPTION_COUNT - 1; Index >= 0; Index -= 1) {
		RTItemOptionSlot* Slot = &Options.Equipment.Slots[Index];
		if (*(UInt8*)Slot != 0) continue;

		return Slot;
	}

	return NULL;
}

Int32 RTItemOptionGetLastFilledForceSlotIndex(
	RTItemOptions Options
) {
	RTItemForceOptionData Data = RTItemForceOptionDecode(Options.Serial);
	
	for (Int32 Index = Data.FilledForceSlotCount - 1; Index >= 0; Index -= 1) {
		UInt8 ForceIndex = Data.ForceSlots[Index];
		if (ForceIndex < 1) continue;

		return Index;
	}

	return -1;
}

RTItemOptionSlot RTItemOptionGetLastFilledForceSlot(
	RTItemOptions Options
) {
	for (Int32 Index = RUNTIME_ITEM_MAX_OPTION_COUNT - 1; Index >= 0; Index -= 1) {
		RTItemOptionSlot Slot = Options.Equipment.Slots[Index];
		if (Slot.IsEpic) continue;
		if (Slot.ForceIndex < 1) continue;

		return Slot;
	}

	RTItemOptionSlot ZeroSlot = { 0 };
	return ZeroSlot;
}

Int32 RTItemOptionGetForceSlotCount(
	RTItemOptions Options,
	Int32 ForceIndex
) {
	Int32 Count = 0;

	for (Int32 Index = 0; Index < RUNTIME_ITEM_MAX_OPTION_COUNT; Index += 1) {
		RTItemOptionSlot Slot = Options.Equipment.Slots[Index];
		if (Slot.IsEpic) continue;
		if (Slot.ForceIndex != ForceIndex) continue;

		Count += Slot.ForceLevel;
	}

	return Count;
}

Bool RTItemOptionPreserveEpicSlot(
	RTItemOptions* Options
) {
	UInt8 Value = *((UInt8*)&Options->Equipment.Slots[0]);
	if (Value > 0) {
		return RTItemOptionPushSlots(Options);
	}

	return true;
}

Int32 RTItemOptionGetFilledSlotCount(
	RTItemOptions Options
) {
	Int32 FilledSlotCount = 0;

	for (Int32 Index = RUNTIME_ITEM_MAX_OPTION_COUNT - 1; Index >= 0; Index -= 1) {
		if (Options.Equipment.Slots[Index].IsEpic) continue;
		if (Options.Equipment.Slots[Index].ForceIndex < 1) continue;

		FilledSlotCount += 1;
	}

	return FilledSlotCount;
}

Bool RTItemOptionPushSlots(
	RTItemOptions* Options
) {
	Int32 LastEmptySlotIndex = RTItemOptionLastEmptySlotIndex(Options);
	if (LastEmptySlotIndex < 0) return false;

	for (Int32 Index = LastEmptySlotIndex - 1; Index >= 0; Index -= 1) {
		Options->Equipment.Slots[Index + 1] = Options->Equipment.Slots[Index];
	}

	RTItemOptionSlot EmptySlot = { 0 };
	Options->Equipment.Slots[0] = EmptySlot;
	return true;
}

Void RTItemOptionClearForceSlot(
	RTItemOptions* Options,
	Int32 Index
) {
	assert(Index < RUNTIME_ITEM_MAX_OPTION_COUNT);

	RTItemForceOptionData Data = RTItemForceOptionDecode(Options->Serial);
	if (Data.ForceSlots[Index] > 0) {
		Data.ForceSlots[Index] = 0;

		for (Int32 Offset = Index + 1; Offset < Data.FilledForceSlotCount; Offset += 1) {
			Data.ForceSlots[Offset - 1] = Data.ForceSlots[Offset];
		}

		Data.FilledForceSlotCount -= 1;
		Options->Serial = RTItemForceOptionEncode(Data);
	}
}

Void RTItemOptionClearSecondForceSlot(
	RTItemOptions* Options
) {
	RTItemForceOptionData Data = RTItemForceOptionDecode(Options->Serial);
	if (Data.FilledForceSlotCount > 1) {
		Data.ForceSlots[1] = 0;

		for (Int32 Offset = 2; Offset < Data.FilledForceSlotCount; Offset += 1) {
			Data.ForceSlots[Offset - 1] = Data.ForceSlots[Offset];
		}

		Data.FilledForceSlotCount -= 1;
		Options->Serial = RTItemForceOptionEncode(Data);
	}
}

Bool RTItemOptionAppendSlot(
	RTItemOptions* Options,
	RTItemOptionSlot Slot,
	Bool IsExtended
) {
	RTItemForceOptionData Data = RTItemForceOptionDecode(Options->Serial);

	if (Slot.IsEpic) {
		if (Data.EpicSlotCount >= RUNTIME_ITEM_MAX_OPTION_COUNT) return false;

		Data.EpicSlots[Data.EpicSlotCount].Serial = Slot.Serial;
		Data.EpicSlotCount += 1;
	}
	else {
		if (Data.FilledForceSlotCount + 1 > MIN(Data.ForceSlotCount, RUNTIME_ITEM_MAX_OPTION_COUNT)) return false;

		if (!RTItemForceOptionInsertForceIndex(&Data, Slot.ForceIndex)) {
			for (Int32 Index = Data.FilledForceSlotCount; Index < Data.FilledForceSlotCount + 1; Index += 1) {
				Data.ForceSlots[Index] = Slot.ForceIndex;
			}

			Data.FilledForceSlotCount += 1;
		}

		/*
		RTItemForceOptionData TargetData = Data;
		TargetData.FilledForceSlotCount = 0;

		for (Int32 Index = 0; Index < Data.ForceSlotCount; Index += 1) {
			if (Slot.ForceLevel < 2 &&
				Data.ForceSlots[Index].ForceIndex == Slot.ForceIndex &&
				Data.ForceSlots[Index].ForceLevel + Slot.ForceLevel < 0b111) {
				Slot.ForceLevel += Data.ForceSlots[Index].ForceLevel;
			}
			else if (Data.ForceSlots[Index].Serial > 0) {
				TargetData.ForceSlots[TargetData.FilledForceSlotCount] = Data.ForceSlots[Index];
				TargetData.FilledForceSlotCount += 1;
			}
		}

		if (TargetData.FilledForceSlotCount >= MIN(TargetData.ForceSlotCount, RUNTIME_ITEM_MAX_OPTION_COUNT)) return false;

		TargetData.ForceSlots[TargetData.FilledForceSlotCount].Serial = Slot.Serial;
		TargetData.FilledForceSlotCount += 1;

		if (IsExtended && TargetData.FilledForceSlotCount >= Data.ForceSlotCount && Data.ForceSlots.) {
			RTItemOptionSlot Slot = Data.ForceSlots[FilledIndex];

			for (Int32 Index = FilledIndex - 1; Index >= 0; Index -= 1) {
				Data.ForceSlots[Index + 1] = Data.ForceSlots[Index];
			}

			Data.ForceSlots[0] = Slot;
		}
		*/
	}

	Options->Serial = RTItemForceOptionEncode(Data);
	return true;
}

UInt32 RTItemGetSerialID(
	RTItem Item
) {
	Item.Extension = 0;
	Item.IsAccountBinding = 0;
	Item.IsCharacterBinding = 0;
	Item.IsTradableBinding = 0;
	Item.UpgradeLevel = 0;
	Item.ExtremeLevel = 0;
	Item.DivineLevel = 0;
	return Item.ID;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemStub) {
	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemPotion) {
	switch (ItemData->Potion.PotionType) {
		case RUNTIME_ITEM_SUBTYPE_POTION_HP:
		case RUNTIME_ITEM_SUBTYPE_POTION_AUTO_HP:
			RTCharacterAddHP(Runtime, Character, ItemData->Potion.PotionValue, true);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_MP:
			RTCharacterAddMP(Runtime, Character, ItemData->Potion.PotionValue, true);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_SP:
			RTCharacterAddSP(Runtime, Character, ItemData->Potion.PotionValue);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_STAT: {
			Int32 StatIndex = 0;
			switch (ItemData->Potion.StatType) {
			case RUNTIME_ITEM_POTION_STAT_TYPE_STR:
				StatIndex = RUNTIME_CHARACTER_STAT_STR;
				break;

			case RUNTIME_ITEM_POTION_STAT_TYPE_DEX:
				StatIndex = RUNTIME_CHARACTER_STAT_DEX;
				break;

			case RUNTIME_ITEM_POTION_STAT_TYPE_INT:
				StatIndex = RUNTIME_CHARACTER_STAT_INT;
				break;

			default:
				return RUNTIME_ITEM_USE_RESULT_FAILED;
			}

			if (!RTCharacterRemoveStat(Runtime, Character, StatIndex, ItemData->Potion.PotionValue)) {
				return RUNTIME_ITEM_USE_RESULT_FAILED;
			}

			break;
		}

		case RUNTIME_ITEM_SUBTYPE_POTION_HONOR:
			RTCharacterAddHonorPoint(Runtime, Character, ItemData->Potion.PotionValue);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_FULL_RECOVERY:
			RTCharacterAddHP(Runtime, Character, INT32_MAX, true);
			RTCharacterAddMP(Runtime, Character, INT32_MAX, true);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_STAT_RESET:
			if (!RTCharacterResetStats(Runtime, Character)) return RUNTIME_ITEM_USE_RESULT_FAILED;
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_RAGE:
			RTCharacterAddRage(Runtime, Character, ItemData->Potion.PotionValue);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_BP:
			RTCharacterAddBP(Runtime, Character, ItemData->Potion.PotionValue);
			break;

		default:
			return RUNTIME_ITEM_USE_RESULT_FAILED;
	}

	if (ItemData->MaxStackSize > 0) {
		ItemSlot->ItemOptions = MAX(0, ItemSlot->ItemOptions - 1);
		if (ItemSlot->ItemOptions < 1) {
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

			Character->SyncMask.InventoryInfo = true;
		}
	}
	else {
		// TODO: Do not delete inexhaustable items!!!
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

		Character->SyncMask.InventoryInfo = true;
	}

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSkillBook) {
    struct {
        UInt8 SkillSlotIndex;
    } *Data = Payload;

    RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, ItemData->SkillBook.SkillID);
    if (!SkillData) return RUNTIME_ITEM_USE_RESULT_FAILED;

    // TODO: Check if character is allowed to learn the skill by battle style and skill ranks...

    RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, Data->SkillSlotIndex);
    if (SkillSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

    // TODO: Check if ItemOptions begins at 0 for Level 1

    Int32 SkillLevel = MAX(1, (Int32)ItemSlot->ItemOptions);
    SkillSlot = RTCharacterAddSkillSlot(Runtime, Character, SkillData->SkillID, SkillLevel, Data->SkillSlotIndex);

    RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	Character->SyncMask.InventoryInfo = true;

    return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemImmediateReward) {
	switch (ItemData->Options[0]) {
	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_ALZ:
		Character->Data.Info.Alz += ItemSlot->ItemOptions;
		Character->SyncMask.Info = true;
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_EXP:
		RTCharacterAddExp(Runtime, Character, ItemSlot->ItemOptions);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_SKILLEXP:
		RTCharacterAddSkillExp(Runtime, Character, (Int32)ItemSlot->ItemOptions);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_PETEXP:
		UNIMPLEMENTED;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_GUILDEXP:
		UNIMPLEMENTED;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_HONOR:
		RTCharacterAddHonorPoint(Runtime, Character, ItemSlot->ItemOptions);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_AXP:
		RTCharacterAddAbilityExp(Runtime, Character, ItemSlot->ItemOptions);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_WAREXP:
		UNIMPLEMENTED;
		
	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_MEDALSCORE:
		if (!RTCharacterAddHonorMedalScore(Runtime, Character, 0, (Int32)ItemSlot->ItemOptions)) {
			return RUNTIME_ITEM_USE_RESULT_FAILED;
		}
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_OXP: {
		if (Character->Data.OverlordMasteryInfo.Info.Level < 1) return RUNTIME_ITEM_USE_RESULT_FAILED;
		RTCharacterAddOverlordExp(Runtime, Character, ItemSlot->ItemOptions);
		break;
	}

	default:
		return RUNTIME_ITEM_USE_RESULT_FAILED;
	}

	// TODO: Check if this item should be consumed and check if it is a stackable item
	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	Character->SyncMask.InventoryInfo = true;

    return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemCharacterSlotExtender) {
	if (!RTAccountOpenCharacterSlot(Runtime, Character)) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	Character->SyncMask.InventoryInfo = true;

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemWeapon) {
	if (!RTCharacterCheckItemStatRequirements(Runtime, Character, ItemData, ItemSlot->Item.UpgradeLevel)) {
		return RUNTIME_ITEM_USE_RESULT_FAILED;
	}

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP,
		ItemData->Weapon.AttackRate,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_ATTACK_UP,
		ItemData->Weapon.Attack,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP,
		ItemData->Weapon.MagicAttack,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTItemOptions ItemOptions = { .Serial = ItemSlot->ItemOptions };
	if (ItemOptions.Equipment.SlotCount > 0) {
		RTCharacterApplyItemUpgradeForceEffect(Runtime, Character, ItemData, 2);
	}

	RTCharacterApplyItemUpgradeForceEffect(Runtime, Character, ItemData, ItemSlot->Item.UpgradeLevel);
	RTCharacterApplyItemForceOptionEffect(Runtime, Character, ItemSlot, ItemData);
	RTCharacterApplyItemExtremeLevelEffect(Runtime, Character, ItemSlot, ItemData);
	RTCharacterApplyItemDivineLevelEffect(Runtime, Character, ItemSlot, ItemData);

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemArmor) {
	if (!RTCharacterCheckItemStatRequirements(Runtime, Character, ItemData, ItemSlot->Item.UpgradeLevel)) {
		return RUNTIME_ITEM_USE_RESULT_FAILED;
	}

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_HP_UP,
		ItemData->Armor.HP,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_DEFENSE_RATE_UP,
		ItemData->Armor.DefenseRate,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_DEFENSE_UP,
		ItemData->Armor.Defense,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTItemOptions ItemOptions = { .Serial = ItemSlot->ItemOptions };
	if (ItemOptions.Equipment.SlotCount > 0) {
		RTCharacterApplyItemUpgradeForceEffect(Runtime, Character, ItemData, 2);
	}

	RTCharacterApplyItemUpgradeForceEffect(Runtime, Character, ItemData, ItemSlot->Item.UpgradeLevel);
	RTCharacterApplyItemForceOptionEffect(Runtime, Character, ItemSlot, ItemData);
	RTCharacterApplyItemExtremeLevelEffect(Runtime, Character, ItemSlot, ItemData);
	RTCharacterApplyItemDivineLevelEffect(Runtime, Character, ItemSlot, ItemData);

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemAccessory) {
	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		ItemData->Accessory.ForceEffect1,
		ItemData->Accessory.ForceValue1,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		ItemData->Accessory.ForceEffect2,
		ItemData->Accessory.ForceValue2,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		ItemData->Accessory.ForceEffect3,
		ItemData->Accessory.ForceValue3,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		ItemData->Accessory.ForceEffect4,
		ItemData->Accessory.ForceValue4,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		ItemData->Accessory.ForceEffect5,
		ItemData->Accessory.ForceValue5,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		ItemData->ExtendedAccessory.ForceEffect1,
		ItemData->ExtendedAccessory.ForceValue1,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		ItemData->ExtendedAccessory.ForceEffect2,
		ItemData->ExtendedAccessory.ForceValue2,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		ItemData->ExtendedAccessory.ForceEffect3,
		ItemData->ExtendedAccessory.ForceValue3,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyItemForceOptionEffect(Runtime, Character, ItemSlot, ItemData);
	RTCharacterApplyItemChaosLevelEffect(Runtime, Character, ItemSlot, ItemData);

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemFrontierStone) {
	RTItemOptionFrontierStone* Data = Payload;

	if (!RTCharacterIsAlive(Runtime, Character)) return RUNTIME_ITEM_USE_RESULT_IS_DEAD;

	if (ItemSlot->ItemOptions > 0) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTWorldContextRef TargetWorld = RTRuntimeGetWorldByID(Runtime, Data->WorldIndex);
	if (!TargetWorld) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (TargetWorld->WorldData->Type == RUNTIME_WORLD_TYPE_DUNGEON ||
		TargetWorld->WorldData->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return RUNTIME_ITEM_USE_RESULT_FAILED;

	if (RTWorldIsTileColliding(Runtime, TargetWorld, Data->X, Data->Y, Character->Movement.CollisionMask)) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemOptions ItemOptions = { .FrontierStone = *Data };
	ItemSlot->ItemOptions = ItemOptions.Serial;
	Character->SyncMask.InventoryInfo = true;

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEffector) {
	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemVehicleBike) {
	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_DEFENSE_UP,
		ItemData->VehicleBike.Defense,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_INCREASE_MOVEMENT_SPEED,
		ItemData->VehicleBike.MovementSpeed,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_DEFENSE_RATE_UP,
		ItemData->VehicleBike.DefenseRate,
		RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	);

	RTCharacterApplyItemUpgradeForceEffect(Runtime, Character, ItemData, ItemSlot->Item.UpgradeLevel);
	RTCharacterApplyItemForceOptionEffect(Runtime, Character, ItemSlot, ItemData);
	RTCharacterApplyItemExtremeLevelEffect(Runtime, Character, ItemSlot, ItemData);
	RTCharacterApplyItemDivineLevelEffect(Runtime, Character, ItemSlot, ItemData);

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemVehicleBoard) {
	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemCharm) {
	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSpecialPotion) {
	if (!RTCharacterIsAlive(Runtime, Character)) return RUNTIME_ITEM_USE_RESULT_IS_DEAD;

	// TODO: Check item cooldowns
	// TODO: Depending on buff slot they should override the old buff

	UInt64 ItemOptions = RTQuestItemGetOptions(ItemSlot->ItemOptions);
	UInt64 ItemCount = RTQuestItemGetCount(ItemSlot->ItemOptions);
	if (ItemCount < 1) return RUNTIME_ITEM_USE_RESULT_REJECTED;

	ItemCount -= 1;
	ItemSlot->ItemOptions = RTQuestItemOptions(ItemOptions, ItemCount);

	if (ItemCount < 1) {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	}

	Int32 ForceEffectValue = (Int32)ItemOptions;
 
	RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
	assert(World);
 
    Int32 BuffResult = RTCharacterApplyBuff(
        Runtime,
		Character,
		ItemData->SpecialPotion.ForceEffectIndex,
		ForceEffectValue,
		ItemData->SpecialPotion.Duration,
		ItemData->SpecialPotion.Cooldown
	);
 
    {
        NOTIFICATION_DATA_CHARACTER_DATA* Notification = RTNotificationInit(CHARACTER_DATA);
        Notification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_BUFF_POTION;
        Notification->BuffResult = BuffResult;
        RTNotificationDispatchToCharacter(Notification, Character);
    }
 
	if (BuffResult == RUNTIME_SKILL_RESULT_BUFF_SUCCESS) {
		return RUNTIME_ITEM_USE_RESULT_SUCCESS;
	}

	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSlotExtender) {
	struct _RTItemConverterPayload* Data = Payload;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->TargetSlotIndex);
	if (!TargetItemSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
	if (!TargetItemData) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemOptions TargetItemOptions = { 0 };
	TargetItemOptions.Serial = TargetItemSlot->ItemOptions;
	if (TargetItemSlot->Item.IsAccountBinding) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (TargetItemOptions.Equipment.SlotCount >= TargetItemData->Armor.MaxSlotCount) return RUNTIME_ITEM_USE_RESULT_FAILED;

	TargetItemOptions.Equipment.SlotCount += 1;

	Bool IsExtendable = (
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER) ||
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND) ||
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND) ||
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_CHAKRAM) ||
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_SUIT) ||
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_GLOVES) ||
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_BOOTS) ||
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_HELMED1) ||
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_HELMED2) ||
		(TargetItemData->ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BIKE)
	);
	if (!IsExtendable) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTDataUpgradeExtendGradeRef UpgradeGrade = RTRuntimeDataUpgradeExtendGradeGet(Runtime->Context, TargetItemData->ItemGrade);
	if (!UpgradeGrade) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (ItemData->ItemGrade != UpgradeGrade->CostGrade) return RUNTIME_ITEM_USE_RESULT_FAILED;

	TargetItemSlot->Item.IsAccountBinding = true;
	TargetItemSlot->ItemOptions = TargetItemOptions.Serial;

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSlotConverter) {
	struct _RTItemConverterPayload* Data = Payload;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->TargetSlotIndex);
	if (!TargetItemSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
	if (!TargetItemData) return RUNTIME_ITEM_USE_RESULT_FAILED;
	
	if (ItemData->SlotConverter.TargetItemType != TargetItemData->ItemType) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (ItemData->ItemGrade != TargetItemData->ItemGrade) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemOptions TargetItemOptions = { 0 };
	TargetItemOptions.Serial = TargetItemSlot->ItemOptions;
	if (TargetItemOptions.Equipment.SlotCount >= ItemData->SlotConverter.MaxSlotCount) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (TargetItemOptions.Equipment.SlotCount >= TargetItemData->Options[3]) return RUNTIME_ITEM_USE_RESULT_FAILED;

	Int32 SourceItemSlotIndex = ItemSlot->SlotIndex;
	Int32 TargetItemSlotIndex = TargetItemSlot->SlotIndex;

	Int32 Seed = (Int32)PlatformGetTickCount();
	Int32 RandomValue = RandomRange(&Seed, 0, 1000);
	if (RandomValue < ItemData->SlotConverter.SuccessRate) {
		TargetItemOptions.Equipment.SlotCount += 1;
		TargetItemSlot->ItemOptions = TargetItemOptions.Serial;
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceItemSlotIndex);
		return RUNTIME_ITEM_USE_RESULT_SUCCESS;
	}
	else {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceItemSlotIndex);

		if (!ItemData->SlotConverter.HasSafeguard) {
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, TargetItemSlotIndex);
			return RUNTIME_ITEM_USE_RESULT_SUCCESS_DESTROY_ITEM;
		}

		return RUNTIME_ITEM_USE_RESULT_SUCCESS_SAFEGUARD_ITEM;
	}
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEpicConverter) {
	struct _RTItemConverterPayload* Data = Payload;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->TargetSlotIndex);
	if (!TargetItemSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
	if (!TargetItemData) return RUNTIME_ITEM_USE_RESULT_FAILED;

	if (ItemData->EpicConverter.TargetItemType != TargetItemData->ItemType) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (ItemData->EpicConverter.RequiredItemLevel > TargetItemSlot->Item.UpgradeLevel) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (ItemData->ItemGrade > TargetItemData->ItemGrade) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemOptions TargetItemOptions = { 0 };
	TargetItemOptions.Serial = TargetItemSlot->ItemOptions;
	if (TargetItemOptions.Equipment.SlotCount < ItemData->EpicConverter.RequiredSlotCount) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTDataEpicOptionPoolRef OptionPool = RTRuntimeDataEpicOptionPoolGet(Runtime->Context, ItemData->EpicConverter.PoolID);
	if (!OptionPool) return RUNTIME_ITEM_USE_RESULT_FAILED;

	Int32 MaxRandomValue = 0;
	for (Int32 Index = 0; Index < OptionPool->EpicOptionPoolValueCount; Index += 1) {
		MaxRandomValue += OptionPool->EpicOptionPoolValueList[Index].Rate;
	}

	Int32 Seed = (Int32)PlatformGetTickCount();
	Int32 RandomValue = RandomRange(&Seed, 0, MaxRandomValue);
	Int32 RateOffset = 0;
	Bool Success = false;

	for (Int32 Index = 0; Index < OptionPool->EpicOptionPoolValueCount; Index += 1) {
		RTDataEpicOptionPoolValueRef OptionPoolValue = &OptionPool->EpicOptionPoolValueList[Index];

		if (RandomValue <= OptionPoolValue->Rate + RateOffset) {
			Int32 SlotIndex = RTItemOptionFirstEpicSlotIndex(&TargetItemOptions);

			Bool IsReset = OptionPoolValue->EpicOption < 1 && OptionPoolValue->EpicGrade < 1;
			if (IsReset && SlotIndex < 0) return RUNTIME_ITEM_USE_RESULT_FAILED;

			if (!IsReset) {
				if (SlotIndex >= 0) return RUNTIME_ITEM_USE_RESULT_FAILED;
				if (!RTItemOptionPreserveEpicSlot(&TargetItemOptions)) return RUNTIME_ITEM_USE_RESULT_FAILED;

				SlotIndex = 0;
			}

			TargetItemOptions.Equipment.Slots[SlotIndex].IsEpic = !IsReset;
			TargetItemOptions.Equipment.Slots[SlotIndex].ForceIndex = OptionPoolValue->EpicOption;
			TargetItemOptions.Equipment.Slots[SlotIndex].ForceLevel = OptionPoolValue->EpicGrade;
			Success = true;
			break;
		}
		
		RateOffset += OptionPoolValue->Rate;
	}

	TargetItemSlot->ItemOptions = TargetItemOptions.Serial;
	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

	return (Success) ? RUNTIME_ITEM_USE_RESULT_SUCCESS : RUNTIME_ITEM_USE_RESULT_SUCCESS_SAFEGUARD_ITEM;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemDivineConverter) {
	struct _RTItemConverterPayload* Data = Payload;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->TargetSlotIndex);
	if (!TargetItemSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
	if (!TargetItemData) return RUNTIME_ITEM_USE_RESULT_FAILED;

	// TODO: Check divine converter subtype from Options[0]
	//	1 = Weapon
	//	2 = Armor
	//	3 = Bike

	RTDataDivineUpgradeMainRef UpgradeMain = RTRuntimeDataDivineUpgradeMainGet(
		Runtime->Context,
		TargetItemData->ItemGrade,
		TargetItemData->ItemType
	);
	if (!UpgradeMain) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTDataDivineUpgradeGroupCostRef UpgradeGroupCost = RTRuntimeDataDivineUpgradeGroupCostGet(
		Runtime->Context,
		UpgradeMain->Group
	);
	if (!UpgradeGroupCost) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTDataDivineUpgradeGroupCostLevelRef UpgradeGroupCostLevel = RTRuntimeDataDivineUpgradeGroupCostLevelGet(
		UpgradeGroupCost,
		TargetItemSlot->Item.DivineLevel
	);
	if (!UpgradeGroupCostLevel) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (UpgradeGroupCostLevel->RequiredCoreCount > 0 || UpgradeGroupCostLevel->RequiredSafeCount > 0) return RUNTIME_ITEM_USE_RESULT_FAILED;

	TargetItemSlot->Item.DivineLevel += 1;
	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemChaosConverter) {
	struct _RTItemConverterPayload* Data = Payload;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->TargetSlotIndex);
	if (!TargetItemSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
	if (!TargetItemData) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTDataChaosUpgradeItemListRef ChaosUpgradeItemList = RTRuntimeDataChaosUpgradeItemListGet(Runtime->Context, TargetItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX);
	if (!ChaosUpgradeItemList) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (!ChaosUpgradeItemList->CheckLevel) return RUNTIME_ITEM_USE_RESULT_FAILED;

	if (TargetItemSlot->Item.UpgradeLevel >= ChaosUpgradeItemList->ItemGrade) return RUNTIME_ITEM_USE_RESULT_FAILED;

	TargetItemSlot->Item.UpgradeLevel += 1;
	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEpicBooster) {
	struct _RTItemConverterPayload* Data = Payload;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->TargetSlotIndex);
	if (!TargetItemSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
	if (!TargetItemData) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTDataEpicBoostMainRef EpicBoostMain = RTRuntimeDataEpicBoostMainGet(Runtime->Context, TargetItemData->ItemType);
	if (!EpicBoostMain) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemOptions TargetItemOptions = { .Serial = TargetItemSlot->ItemOptions };
	if (TargetItemData->MasterGrade > 0) {
		if (TargetItemOptions.Equipment.Slots[0].MasterIndex < 1) return RUNTIME_ITEM_USE_RESULT_FAILED;

		RTDataMasterItemRef MasterItem = RTRuntimeDataMasterItemGet(Runtime->Context, TargetItemData->MasterGrade);
		if (!MasterItem) return RUNTIME_ITEM_USE_RESULT_FAILED;

		RTDataMasterItemOptionRef MasterItemOption = RTRuntimeDataMasterItemOptionGet(MasterItem, TargetItemOptions.Equipment.Slots[0].MasterIndex);
		if (!MasterItemOption) return RUNTIME_ITEM_USE_RESULT_FAILED;

		RTDataEpicBoostMasterRateRef EpicBoostMasterRate = RTRuntimeDataEpicBoostMasterRateGet(Runtime->Context, MasterItemOption->BoostCount);
		if (!EpicBoostMasterRate) return RUNTIME_ITEM_USE_RESULT_FAILED;

		Int32 ResultGrade = 0;
		for (Int32 Index = 0; Index < MasterItem->MasterItemOptionCount; Index += 1) {
			RTDataMasterItemOptionRef ResultItemOption = &MasterItem->MasterItemOptionList[Index];
			if (MasterItemOption->BoostCount != ResultItemOption->BoostCount + 1) continue;
			if (MasterItemOption->MasterItemOptionValueCount != ResultItemOption->MasterItemOptionValueCount) continue;

			Bool IsEqual = true;
			for (Int32 ValueIndex = 0; ValueIndex < ResultItemOption->MasterItemOptionValueCount; ValueIndex += 1) {
				if (MasterItemOption->MasterItemOptionValueList[ValueIndex].ForceEffectIndex != ResultItemOption->MasterItemOptionValueList[ValueIndex].ForceEffectIndex) {
					IsEqual = false;
					break;
				}
			}

			if (IsEqual) {
				ResultGrade = ResultItemOption->ID;
				break;
			}
		}

		if (ResultGrade < 1) return RUNTIME_ITEM_USE_RESULT_FAILED;

		Int32 Seed = (Int32)PlatformGetTickCount();
		Int32 RandomValue = RandomRange(&Seed, 0, 1000);
		if (RandomValue < EpicBoostMasterRate->Rate) {
			TargetItemOptions.Equipment.Slots[0].MasterIndex = ResultGrade;
			TargetItemSlot->ItemOptions = TargetItemOptions.Serial;
			return RUNTIME_ITEM_USE_RESULT_SUCCESS;
		}
	}
	else {
		if (!TargetItemOptions.Equipment.Slots[0].IsEpic) return RUNTIME_ITEM_USE_RESULT_FAILED;

		RTDataEpicBoostNormalRateRef EpicBoostNormalRate = RTRuntimeDataEpicBoostNormalRateGet(Runtime->Context, TargetItemOptions.Equipment.Slots[0].ForceLevel);
		if (!EpicBoostNormalRate) return RUNTIME_ITEM_USE_RESULT_FAILED;

		Int32 Seed = (Int32)PlatformGetTickCount();
		Int32 RandomValue = RandomRange(&Seed, 0, 1000);
		if (RandomValue < EpicBoostNormalRate->Rate) {
			TargetItemOptions.Equipment.Slots[0].ForceLevel = EpicBoostNormalRate->ResultGrade;
			TargetItemSlot->ItemOptions = TargetItemOptions.Serial;
			return RUNTIME_ITEM_USE_RESULT_SUCCESS;
		}
	}

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	Character->SyncMask.InventoryInfo = true;

	return RUNTIME_ITEM_USE_RESULT_EPIC_BOOST_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemHolyWater) {
	if (!RTCharacterEnableForceWing(Runtime, Character)) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	Character->SyncMask.InventoryInfo = true;

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemStackablePotion) {
	// TODO: Check payload length bounds for inventory slot index 
	struct {
		UInt32 RegisteredStackSize;
		UInt32 InventoryItemCount;
		UInt16 InventorySlotIndex[0];
	} *Data = Payload;

	UInt64 TotalStackSize = 0;
	UInt64 Amount = ItemSlot->ItemOptions >> 16;

	for (Index Index = 0; Index < Data->InventoryItemCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->InventorySlotIndex[Index]);
		if (!ItemSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;
		if (Amount != ItemSlot->ItemOptions >> 16) return RUNTIME_ITEM_USE_RESULT_FAILED;

		TotalStackSize += ItemSlot->ItemOptions & 0xFFFF;
	}

	if (Data->RegisteredStackSize > TotalStackSize) return RUNTIME_ITEM_USE_RESULT_FAILED;

	UInt64 TotalAmount = Amount * Data->RegisteredStackSize;

	switch (ItemData->Options[0]) {
	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_ALZ:
		Character->Data.Info.Alz += TotalAmount;
		Character->SyncMask.Info = true;
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_EXP:
		RTCharacterAddExp(Runtime, Character, TotalAmount);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_SKILLEXP:
		RTCharacterAddSkillExp(Runtime, Character, (UInt32)TotalAmount);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_PETEXP:
		UNIMPLEMENTED;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_GUILDEXP:
		UNIMPLEMENTED;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_HONOR:
		RTCharacterAddHonorPoint(Runtime, Character, TotalAmount);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_AXP:
		RTCharacterAddAbilityExp(Runtime, Character, TotalAmount);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_WAREXP:
		UNIMPLEMENTED;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_MEDALSCORE:
		if (!RTCharacterAddHonorMedalScore(Runtime, Character, 0, (Int32)TotalAmount)) {
			return RUNTIME_ITEM_USE_RESULT_FAILED;
		}
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_OXP: {
		if (Character->Data.OverlordMasteryInfo.Info.Level < 1) return RUNTIME_ITEM_USE_RESULT_FAILED;
		RTCharacterAddOverlordExp(Runtime, Character, TotalAmount);
		break;
	}

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_WINGEXP: {
		if (Character->Data.ForceWingInfo.Info.Grade < 1) return RUNTIME_ITEM_USE_RESULT_FAILED;
		RTCharacterAddWingExp(Runtime, Character, TotalAmount);
		break;
	}

	default:
		return RUNTIME_ITEM_USE_RESULT_FAILED;
	}

	Int32 RemainingStackSize = Data->RegisteredStackSize;
	for (Index Index = 0; Index < Data->InventoryItemCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->InventorySlotIndex[Index]);

		Int32 ItemStackSize = ItemSlot->ItemOptions & 0xFFFF;
		Int32 ItemAmount = ItemSlot->ItemOptions >> 16;
		Int32 ConsumableStackSize = MIN(ItemStackSize, RemainingStackSize);

		ItemStackSize -= ConsumableStackSize;
		RemainingStackSize -= ConsumableStackSize;

		if (ItemStackSize < 1) {
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
		}
		else {
			ItemSlot->ItemOptions = Amount << 16 | (ItemStackSize & 0xFFFF);
		}
	}

	Character->SyncMask.InventoryInfo = true;

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemHonorMedalResetSelective) {
	struct _RTItemHonorMedalResetSelectivePayload* Data = (struct _RTItemHonorMedalResetSelectivePayload*)Payload;
	
	RTHonorMedalSlotRef MedalSlot = RTCharacterGetHonorMedalSlot(Runtime, Character, Data->CategoryIndex, Data->GroupIndex, Data->SlotIndex);
	if (!MedalSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

	MedalSlot->ForceEffectIndex = 0;
	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

	Character->SyncMask.InventoryInfo = true;
	Character->SyncMask.HonorMedalInfo = true;

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemTransformationCard) {
	if (!RTCharacterTransformUnlock(Runtime, Character, ItemData->TransformationCard.TransformIndex)) {
		return RUNTIME_ITEM_USE_RESULT_FAILED;
	}

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

	Character->SyncMask.InventoryInfo = true;
	Character->SyncMask.TransformInfo = true;

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemChangeGender) {
	Character->Data.StyleInfo.Style.Gender = !Character->Data.StyleInfo.Style.Gender;
	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

	Character->SyncMask.Info = true;
	Character->SyncMask.InventoryInfo = true;

	NOTIFICATION_DATA_CHANGE_GENDER* Notification = RTNotificationInit(CHANGE_GENDER);
	Notification->Gender = Character->Data.StyleInfo.Style.Gender;
	Notification->Success = 1;
	RTNotificationDispatchToCharacter(Notification, Character);
	
	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemCrest) {
	RTCharacterApplyItemForceOptionEffect(Runtime, Character, ItemSlot, ItemData);
	
	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemLotto) { 
	RTDataLotteryItemPoolRef ItemPool = RTRuntimeDataLotteryItemPoolGet(Runtime->Context, ItemData->Lottery.PoolID);
	if (!ItemPool) return RUNTIME_ITEM_USE_RESULT_FAILED;

	Int32 Seed = (Int32)PlatformGetTickCount();
	Int32 RandomRate = RandomRange(&Seed, 0, INT32_MAX);
	Int32 RandomRateOffset = 0;

	// TODO: Option[0] = Item1, Option[1] = Maybe Perfect Drop?, Option[2] = ?, Option[3] = ?, Option[4] = Item2, ...
	for (Int32 Index = 0; Index < ItemPool->LotteryItemPoolItemCount; Index += 1) {
		RTDataLotteryItemPoolItemRef Item = &ItemPool->LotteryItemPoolItemList[Index];
		if (RandomRate <= Item->Rate + RandomRateOffset || Index + 1 == ItemPool->LotteryItemPoolItemCount) {
			struct _RTItemSlot CreatedItemSlot = { 0 };
			CreatedItemSlot.Item.Serial = Item->ItemID;
			CreatedItemSlot.ItemOptions = Item->ItemOption;
			CreatedItemSlot.ItemDuration.Serial = Item->ItemDuration;
			CreatedItemSlot.SlotIndex = RTInventoryGetNextFreeSlotIndex(Runtime, &Character->Data.TemporaryInventoryInfo);
			if (!RTInventoryInsertSlot(Runtime, &Character->Data.TemporaryInventoryInfo, &CreatedItemSlot)) return RUNTIME_ITEM_USE_RESULT_FAILED;

			NOTIFICATION_DATA_CREATE_ITEM* Notification = RTNotificationInit(CREATE_ITEM);
			Notification->ItemType = 0;
			Notification->ItemCount = 1;

			NOTIFICATION_DATA_CREATE_ITEM_SLOT* NotificationItem = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CREATE_ITEM_SLOT);
			NotificationItem->ItemID = CreatedItemSlot.Item.Serial;
			NotificationItem->ItemOptions = CreatedItemSlot.ItemOptions;
			NotificationItem->SlotIndex = CreatedItemSlot.SlotIndex + RUNTIME_INVENTORY_TOTAL_SIZE;
			NotificationItem->ItemDuration = CreatedItemSlot.ItemDuration.Serial;

			RTNotificationDispatchToCharacter(Notification, Character);

			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

			Character->SyncMask.TemporaryInventoryInfo = true;
			Character->SyncMask.InventoryInfo = true;

			return RUNTIME_ITEM_USE_RESULT_SUCCESS;
		}

		RandomRateOffset += Item->Rate;
	}

	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemCoreEnhancer) {
	struct _RTItemConverterPayload* Data = Payload;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->TargetSlotIndex);
	if (!TargetItemSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
	if (!TargetItemData) return RUNTIME_ITEM_USE_RESULT_FAILED;

	if (ItemSlot->ItemOptions < 1) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (ItemData->EnchantCodeLink < TargetItemData->EnchantCodeLink) return RUNTIME_ITEM_USE_RESULT_FAILED;
	if (TargetItemSlot->Item.UpgradeLevel > 0) return RUNTIME_ITEM_USE_RESULT_FAILED;

	TargetItemSlot->Item.UpgradeLevel = ItemData->CoreEnhancer.UpgradeType;

	ItemSlot->ItemOptions -= 1;
	if (ItemSlot->ItemOptions < 1) {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	}

	Character->SyncMask.InventoryInfo = true;

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemForceGemPackage) {
    RTCharacterAddForceGem(Runtime, Character, ItemSlot->ItemOptions);
	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	Character->SyncMask.InventoryInfo = true;

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemRemoteNpc) {
	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_REMOTE_SHOP) {
		ItemSlot->ItemOptions = MAX(0, ItemSlot->ItemOptions - 1);
		if (ItemSlot->ItemOptions <= 0) {
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
		}

		Character->SyncMask.InventoryInfo = true;
	}

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTRuneSlotExtender) {
	if (ItemData->RuneSlotExtender.AbilityType == RUNTIME_ABILITY_TYPE_ESSENCE) {
		Int32 MaxSlotCount = RUNTIME_CHARACTER_MAX_ESSENCE_ABILITY_SLOT_COUNT - RUNTIME_CHARACTER_ESSENCE_ABILITY_SLOT_COUNT;
		if (Character->Data.AbilityInfo.Info.ExtendedEssenceAbilityCount >= MaxSlotCount) return RUNTIME_ITEM_USE_RESULT_FAILED;

		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

		Character->Data.AbilityInfo.Info.ExtendedEssenceAbilityCount += 1;
		Character->SyncMask.AbilityInfo = true;
		Character->SyncMask.InventoryInfo = true;
		return RUNTIME_ITEM_USE_RESULT_SUCCESS;
	}

	if (ItemData->RuneSlotExtender.AbilityType == RUNTIME_ABILITY_TYPE_BLENDED) {
		Int32 MaxSlotCount = RUNTIME_CHARACTER_MAX_BLENDED_ABILITY_SLOT_COUNT - RUNTIME_CHARACTER_BLENDED_ABILITY_SLOT_COUNT;
		if (Character->Data.AbilityInfo.Info.ExtendedBlendedAbilityCount >= MaxSlotCount) return RUNTIME_ITEM_USE_RESULT_FAILED;

		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

		Character->Data.AbilityInfo.Info.ExtendedBlendedAbilityCount += 1;
		Character->SyncMask.AbilityInfo = true;
		Character->SyncMask.InventoryInfo = true;
		return RUNTIME_ITEM_USE_RESULT_SUCCESS;
	}

	if (ItemData->RuneSlotExtender.AbilityType == RUNTIME_ABILITY_TYPE_KARMA) {
		Int32 MaxSlotCount = RUNTIME_CHARACTER_MAX_KARMA_ABILITY_SLOT_COUNT - RUNTIME_CHARACTER_KARMA_ABILITY_SLOT_COUNT;
		if (Character->Data.AbilityInfo.Info.ExtendedKarmaAbilityCount >= MaxSlotCount) return RUNTIME_ITEM_USE_RESULT_FAILED;

		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);

		Character->Data.AbilityInfo.Info.ExtendedKarmaAbilityCount += 1;
		Character->SyncMask.AbilityInfo = true;
		Character->SyncMask.InventoryInfo = true;
		return RUNTIME_ITEM_USE_RESULT_SUCCESS;
	}

	return RUNTIME_ITEM_USE_RESULT_FAILED;
}
