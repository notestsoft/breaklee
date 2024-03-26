#include "Character.h"
#include "Force.h"
#include "Item.h"
#include "Inventory.h"
#include "Runtime.h"

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

	if (Character->Info.Stat[RUNTIME_CHARACTER_STAT_STR] < RequiredStr ||
		Character->Info.Stat[RUNTIME_CHARACTER_STAT_DEX] < RequiredDex ||
		Character->Info.Stat[RUNTIME_CHARACTER_STAT_INT] < RequiredInt) {
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
	RTDataUpgradeItemBasicRef UpgradeItemBasic = RTRuntimeDataUpgradeItemBasicGet(Runtime->Context, ItemData->ItemType);
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
					RUNTIME_FORCE_EFFECT_TYPE_ADDITIVE
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

Bool RTItemUseInternal(
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
	return false;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemPotion) {
	/* TODO: Send notification event in character!
	S2C_DATA_CHARACTER_UPDATE* Notification = PacketInit(S2C_DATA_CHARACTER_UPDATE);
	Notification->Command = S2C_CHARACTER_UPDATE;

	switch (ItemData->Options[0]) {
	case RUNTIME_ITEM_POTION_TYPE_HP:
		RTCharacterAddHP(Runtime, Character, ItemData->Options[4], true);
		Notification->Type = S2C_DATA_CHARACTER_UPDATE_TYPE_HPPOTION;
		Notification->HP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		break;

	case RUNTIME_ITEM_POTION_TYPE_MP:
		RTCharacterAddMP(Runtime, Character, ItemData->Options[4]);
		Notification->Type = S2C_DATA_CHARACTER_UPDATE_TYPE_MPPOTION;
		Notification->MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		break;

	case RUNTIME_ITEM_POTION_TYPE_SP:
		break;

	default:
		goto error;
	}
	*/

	switch (ItemData->Potion.PotionType) {
		case RUNTIME_ITEM_SUBTYPE_POTION_HP:
			RTCharacterAddHP(Runtime, Character, ItemData->Options[4], true);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_MP:
			RTCharacterAddMP(Runtime, Character, ItemData->Options[4], true);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_SP:
			RTCharacterAddSP(Runtime, Character, ItemData->Options[4]);
			break;

		default:
			return false;
	}

	if (ItemData->MaxStackSize > 0) {
		ItemSlot->ItemOptions = MAX(0, ItemSlot->ItemOptions - 1);
		if (ItemSlot->ItemOptions < 1) {
			RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlot->SlotIndex);

			Character->SyncMask.InventoryInfo = true;
			Character->SyncPriority.Low = true;
		}
	}
	else {
		// TODO: Do not delete inexhaustable items!!!
		RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlot->SlotIndex);

		Character->SyncMask.InventoryInfo = true;
		Character->SyncPriority.Low = true;
	}

	return true;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSkillBook) {
    struct {
        UInt8 SkillSlotIndex;
    } *Data = Payload;

    RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, ItemData->SkillBook.SkillID);
    if (!SkillData) return false;

    // TODO: Check if character is allowed to learn the skill by battle style and skill ranks...

    RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, Data->SkillSlotIndex);
    if (SkillSlot) return false;

    // TODO: Check if ItemOptions begins at 0 for Level 1

    Int32 SkillLevel = MAX(1, (Int32)ItemSlot->ItemOptions);
    SkillSlot = RTCharacterAddSkillSlot(Runtime, Character, SkillData->SkillID, SkillLevel, Data->SkillSlotIndex);

    RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlot->SlotIndex);

	Character->SyncMask.InventoryInfo = true;
	Character->SyncPriority.Low = true;

    return true;
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

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_OXP: {
		if (Character->Info.Overlord.Level < 1) return false;
		RTCharacterAddOverlordExp(Runtime, Character, ItemSlot->ItemOptions);
		return true;
	}

	default:
		return false;
	}

	// TODO: Check if this item should be consumed and check if it is a stackable item
	RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlot->SlotIndex);

	Character->SyncMask.InventoryInfo = true;
	Character->SyncPriority.Low = true;

    return true;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemWeapon) {
	if (!RTCharacterCheckItemStatRequirements(Runtime, Character, ItemData, ItemSlot->Item.UpgradeLevel)) return false;
	
	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP,
		ItemData->Weapon.AttackRate,
		RUNTIME_FORCE_EFFECT_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_ATTACK_UP,
		ItemData->Weapon.Attack,
		RUNTIME_FORCE_EFFECT_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP,
		ItemData->Weapon.MagicAttack,
		RUNTIME_FORCE_EFFECT_TYPE_ADDITIVE
	);

	RTCharacterApplyItemUpgradeForceEffect(
		Runtime,
		Character,
		ItemData,
		ItemSlot->Item.UpgradeLevel
	);

	RTDataUpgradeItemRef UpgradeItem = RTRuntimeDataUpgradeItemGet(Runtime->Context, ItemData->ItemType);
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

	return true;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemArmor) {
	if (!RTCharacterCheckItemStatRequirements(Runtime, Character, ItemData, ItemSlot->Item.UpgradeLevel)) return false;

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_HP_UP,
		ItemData->Armor.HP,
		RUNTIME_FORCE_EFFECT_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_DEFENSE_RATE_UP,
		ItemData->Armor.DefenseRate,
		RUNTIME_FORCE_EFFECT_TYPE_ADDITIVE
	);

	RTCharacterApplyForceEffect(
		Runtime,
		Character,
		RUNTIME_FORCE_EFFECT_DEFENSE_UP,
		ItemData->Armor.Defense,
		RUNTIME_FORCE_EFFECT_TYPE_ADDITIVE
	);

	RTCharacterApplyItemUpgradeForceEffect(
		Runtime,
		Character,
		ItemData,
		ItemSlot->Item.UpgradeLevel
	);

	return true;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemAccessory) {

	return false;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEffector) {
	return false;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSlotExtender) {
	struct _RTItemSlotExtenderPayload* Data = Payload;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Data->TargetSlotIndex);
	if (!TargetItemSlot) return false;

	RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
	if (!TargetItemData) return false;


	return false;
}
