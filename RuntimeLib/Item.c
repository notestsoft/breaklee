#include "Character.h"
#include "Force.h"
#include "Item.h"
#include "Inventory.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Bool RTCharacterCheckItemStatRequirements(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemDataRef ItemData,
	Int32 ItemUpgradeLevel
) {
	// NOTE: This is only working because Weapon & Armor share same memory layout but actually we should pass Stat + StatDelta!!!

	Int32 RequiredStr = ItemData->Weapon.Str + ItemData->Weapon.DeltaStr * MIN(ItemUpgradeLevel, RUNTIME_ENCHANT_MAX_STAT_INCREASE_LEVEL);
	Int32 RequiredDex = ItemData->Weapon.Dex + ItemData->Weapon.DeltaDex * MIN(ItemUpgradeLevel, RUNTIME_ENCHANT_MAX_STAT_INCREASE_LEVEL);
	Int32 RequiredInt = ItemData->Weapon.Int + ItemData->Weapon.DeltaInt * MIN(ItemUpgradeLevel, RUNTIME_ENCHANT_MAX_STAT_INCREASE_LEVEL);

	if (Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR] < RequiredStr ||
		Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] < RequiredDex ||
		Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT] < RequiredInt) {
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
	if (UpgradeItemBasic) {
		RTDataUpgradeItemBasicGradeRef UpgradeItemBasicGrade = RTRuntimeDataUpgradeItemBasicGradeGet(UpgradeItemBasic, ItemData->ItemGrade);
		if (UpgradeItemBasicGrade) {
			for (Int32 Index = 0; Index < UpgradeItemBasicGrade->UpgradeItemBasicGradeValueCount; Index += 1) {
				RTDataUpgradeItemBasicGradeValueRef Value = &UpgradeItemBasicGrade->UpgradeItemBasicGradeValueList[Index];

				Int32 ForceValue = 0;
				for (Int32 Level = 0; Level < ItemUpgradeLevel; Level += 1) {
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
	}
	/* @Next
	RTDataUpgradeItemRef UpgradeItem = RTRuntimeDataUpgradeItemGet(Runtime->Context, ItemData->ItemType);
	if (UpgradeItem) {
		RTDataUpgradeItemGradeRef UpgradeItemGrade = RTRuntimeDataUpgradeItemGradeGet(UpgradeItem, ItemData->ItemGrade);
		if (UpgradeItemGrade) {
			for (Int32 Index = 0; Index < UpgradeItemGrade->UpgradeItemGradeValueCount; Index += 1) {
				RTDataUpgradeItemBasicGradeValueRef Value = &UpgradeItemGrade->UpgradeItemGradeValueList[Index];

				Int32 ForceValue = 0;
				for (Int32 Level = 0; Level < ItemUpgradeLevel; Level += 1) {
					Int32 SlopeMultiplier = MAX(0, MIN(RUNTIME_ENCHANT_SLOPE_MULTIPLIER_LIMIT, ((Int32)Level / RUNTIME_ENCHANT_SLOPE_DIVIDER)));
					ForceValue += Value->ForceValueFormula[0] + Value->ForceValueFormula[1] * SlopeMultiplier;
				}

				RTCharacterApplyForceEffect(
					Runtime,
					Character,
					Value->ForceID,
					ForceValue,
					RUNTIME_FORCE_EFFECT_TYPE_ADDITIVE
				);
			}
		}
	}

	 force_value="6,0,0,1"
	
	RUNTIME_DATA_PROPERTY(Int32, ForceID, "force_id")
		RUNTIME_DATA_PROPERTY_ARRAY(Int32, ForceValueFormula, "force_value", 4, ',')
		RUNTIME_DATA_PROPERTY(Int32, ForceValueType, "value_type")
		RUNTIME_DATA_PROPERTY(Int32, ApplyLevel, "Apply_lv")
		*/
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
			memmove(&Data->ForceSlots[Index + 2], &Data->ForceSlots[Index + 1], TailLength * sizeof(UInt8));
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

Int32 RTItemUseInternal(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemSlotRef ItemSlot,
	RTItemDataRef ItemData,
	Void* Payload
) {
	// TODO: Check if it is a general option of an item that it been consumed from the inventory
#define RUNTIME_ITEM_PROCEDURE(__NAME__, __TYPE__, __INTERNAL__)	\
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
		RTCharacterAddCurrency(Runtime, Character, RUNTIME_CHARACTER_CURRENCY_ALZ, ItemSlot->ItemOptions);
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
		if (Character->Data.Info.Overlord.Level < 1) return RUNTIME_ITEM_USE_RESULT_FAILED;
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

	RTCharacterApplyItemUpgradeForceEffect(
		Runtime,
		Character,
		ItemData,
		ItemSlot->Item.UpgradeLevel
	);

    RTDataItemType ItemType = ItemData->ItemType;
    if (ItemType == RUNTIME_ITEM_TYPE_HELMED2) ItemType = RUNTIME_ITEM_TYPE_HELMED1;

	RTDataUpgradeItemRef UpgradeItem = RTRuntimeDataUpgradeItemGet(Runtime->Context, ItemType);
	if (UpgradeItem) {
		RTDataUpgradeItemGradeRef UpgradeItemGrade = RTRuntimeDataUpgradeItemGradeGet(UpgradeItem, ItemData->ItemGrade);
		if (UpgradeItemGrade) {
			for (Int32 Index = 0; Index < UpgradeItemGrade->UpgradeItemGradeValueCount; Index += 1) {
				RTDataUpgradeItemGradeValueRef Value = &UpgradeItemGrade->UpgradeItemGradeValueList[Index];
				if (Value->ApplyLevel > ItemSlot->Item.UpgradeLevel) continue;
				
				Int32 SlopeMultiplier = MIN(RUNTIME_ENCHANT_SLOPE_MULTIPLIER_LIMIT, (ItemSlot->Item.UpgradeLevel / RUNTIME_ENCHANT_SLOPE_DIVIDER - 1));
				// TODO: This has 4 values involved
				/*
				Int32 ForceValue = Value->ForceValueFormula[0] + Value->ForceValueFormula[1] * SlopeMultiplier;

				RTCharacterApplyForceEffect(
					Runtime,
					Character,
					Value->ForceID,
					ForceValue,
					Value->ForceValueType
				);
				*/
			}
		}
	}

	/*
	UInt64 ExtremeLevel : 4;
	UInt64 DivineLevel : 4;
	*/

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

	RTCharacterApplyItemUpgradeForceEffect(
		Runtime,
		Character,
		ItemData,
		ItemSlot->Item.UpgradeLevel
	);

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemAccessory) {
	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEffector) {
	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEpaulet) {
	return RUNTIME_ITEM_USE_RESULT_FAILED;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemVehicleBike) {
	return RUNTIME_ITEM_USE_RESULT_FAILED;
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
	// TODO: Use an equipment type union to name Options[3] to MaxSlotCount!
	if (TargetItemOptions.Equipment.SlotCount >= TargetItemData->Options[3]) return RUNTIME_ITEM_USE_RESULT_FAILED;

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

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemHolyWater) {
	if (!RTCharacterEnableForceWing(Runtime, Character)) return RUNTIME_ITEM_USE_RESULT_FAILED;

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	Character->SyncMask.InventoryInfo = true;

	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemStackablePotion) {
	// TODO: Check payload length for inventory slot index 
	struct {
		UInt32 RegisteredStackSize;
		UInt32 InventoryItemCount;
		UInt16 InventorySlotIndex[0];
	} *Data = Payload;

	// TODO: Cleanup inventory slot processing
	UInt64 StackSize = 0;
	UInt64 Amount = 0;
	for (Index Index = 0; Index < Data->InventoryItemCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Data->InventorySlotIndex[Index]);
		if (!ItemSlot) return RUNTIME_ITEM_USE_RESULT_FAILED;

		StackSize += ItemSlot->ItemOptions & 0xFFFF;
		Amount += ItemSlot->ItemOptions >> 16;
	}

	if (Data->RegisteredStackSize > StackSize) return RUNTIME_ITEM_USE_RESULT_FAILED;

	UInt64 TotalAmount = Amount * Data->RegisteredStackSize;

	switch (ItemData->Options[0]) {
	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_ALZ:
		RTCharacterAddCurrency(Runtime, Character, RUNTIME_CHARACTER_CURRENCY_ALZ, TotalAmount);
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
		if (Character->Data.Info.Overlord.Level < 1) return RUNTIME_ITEM_USE_RESULT_FAILED;
		RTCharacterAddOverlordExp(Runtime, Character, TotalAmount);
		break;
	}

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_WINGEXP: {
		if (Character->Data.ForceWingInfo.Grade < 1) return RUNTIME_ITEM_USE_RESULT_FAILED;
		RTCharacterAddWingExp(Runtime, Character, TotalAmount);
		break;
	}

	default:
		return RUNTIME_ITEM_USE_RESULT_FAILED;
	}

	// TODO: Add support for multiple item consumptions
	StackSize -= Data->RegisteredStackSize;
	if (StackSize < 1) {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
	}
	else {
		ItemSlot->ItemOptions = Amount << 16 | (StackSize & 0xFFFF);
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
	return RUNTIME_ITEM_USE_RESULT_SUCCESS;
}