#include "Runtime.h"
#include "NewbieSupport.h"

Bool RTCharacterCanTakenNewbieSupportReward(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt8 CategoryType,
	UInt8 RewardIndex,
	UInt8 ConditionValue1,
	UInt8 ConditionValue2
) {
	if (Character->NewbieSupportInfo.Count >= RUNTIME_CHARACTER_MAX_NEWBIE_SUPPORT_SLOT_COUNT) return false;

	for (Index Index = 0; Index < Character->NewbieSupportInfo.Count; Index += 1) {
		RTNewbieSupportSlotRef Slot = &Character->NewbieSupportInfo.Slots[Index];
		if (Slot->CategoryType != CategoryType) continue;
		if (Slot->RewardIndex != RewardIndex) continue;
		if (Slot->ConditionValue1 != ConditionValue1) continue;
		if (Slot->ConditionValue2 != ConditionValue2) continue;

		return false;
	}

	return true;
}

Bool RTCharacterTakeNewbieSupportReward(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 CategoryType,
	UInt8 RewardIndex,
	UInt8 ConditionValue1,
    UInt8 ConditionValue2,
    Int32 InventorySlotCount,
    UInt16* InventorySlotIndex
) {
	if (!RTCharacterCanTakenNewbieSupportReward(Runtime, Character, CategoryType, RewardIndex, ConditionValue1, ConditionValue2)) return false;

	RTDataNewbieSupportCategoryRef Category = RTRuntimeDataNewbieSupportCategoryGet(Runtime->Context, CategoryType);
	if (!Category) return false;

	RTDataNewbieSupportCategoryRewardRef Reward = RTRuntimeDataNewbieSupportCategoryRewardGet(Category, ConditionValue1, ConditionValue2);
	if (!Reward) return false;

	assert(Category->Type < RUNTIME_NEWBIE_SUPPORT_CATEGORY_TYPE_COUNT);

	Int32 ConditionValues1[RUNTIME_NEWBIE_SUPPORT_CATEGORY_TYPE_COUNT] = { 0 };
	ConditionValues1[RUNTIME_NEWBIE_SUPPORT_CATEGORY_TYPE_LEVEL] = Character->Info.Basic.Level;
	ConditionValues1[RUNTIME_NEWBIE_SUPPORT_CATEGORY_TYPE_SKILL_RANK] = Character->Info.Skill.Rank;
	ConditionValues1[RUNTIME_NEWBIE_SUPPORT_CATEGORY_TYPE_HONOR_RANK] = Character->Info.Honor.Rank;

	Int32 ConditionValues2[RUNTIME_NEWBIE_SUPPORT_CATEGORY_TYPE_COUNT] = { 0 };
	ConditionValues2[RUNTIME_NEWBIE_SUPPORT_CATEGORY_TYPE_LEVEL] = 0;
	ConditionValues2[RUNTIME_NEWBIE_SUPPORT_CATEGORY_TYPE_SKILL_RANK] = Character->Info.Skill.Level;
	ConditionValues2[RUNTIME_NEWBIE_SUPPORT_CATEGORY_TYPE_HONOR_RANK] = 0;

	if (ConditionValues1[Category->Type] < Reward->ConditionValue1) return false;
	if (ConditionValues2[Category->Type] < Reward->ConditionValue2) return false;

	RTDataNewbieSupportRewardPoolRef RewardPool = RTRuntimeDataNewbieSupportRewardPoolGet(Runtime->Context, Reward->RewardID);
	if (!RewardPool) return false;

	for (Index Index = 0; Index < InventorySlotCount; Index += 1) {
		if (!RTInventoryIsSlotEmpty(Runtime, &Character->InventoryInfo, InventorySlotIndex[Index])) return false;
	}

	UInt32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
	UInt32 BattleStyleFlag = 1 << (BattleStyleIndex - 1);

	if (RewardIndex < 0 || RewardIndex > RewardPool->NewbieSupportRewardPoolItemCount) return false;

	RTDataNewbieSupportRewardPoolItemRef RewardItem = &RewardPool->NewbieSupportRewardPoolItemList[RewardIndex];
	if (RewardItem->BattleStyleFlag && (RewardItem->BattleStyleFlag & BattleStyleFlag) < 1) return false;

	for (Index RewardItemIndex = 0; RewardItemIndex < RewardItem->Count; RewardItemIndex += 1) {
		struct _RTItemSlot TempSlot = { 0 };
		TempSlot.Item.Serial = RewardItem->ItemID;
		TempSlot.ItemOptions = RewardItem->ItemOptions;
		TempSlot.ItemDuration.Serial = RewardItem->ItemDuration;
		TempSlot.SlotIndex = InventorySlotIndex[RewardItemIndex];

		Bool Success = RTInventorySetSlot(Runtime, &Character->InventoryInfo, &TempSlot);
		assert(Success);
	}

	RTNewbieSupportSlotRef NewbieSupportSlot = &Character->NewbieSupportInfo.Slots[Character->NewbieSupportInfo.Count];
	NewbieSupportSlot->CategoryType = CategoryType;
	NewbieSupportSlot->ConditionValue1 = ConditionValue1;
	NewbieSupportSlot->ConditionValue2 = ConditionValue2;
	NewbieSupportSlot->RewardIndex = RewardIndex;
	Character->NewbieSupportInfo.Count += 1;

	Character->SyncMask.NewbieSupportInfo = true;
	Character->SyncMask.InventoryInfo = true;
	Character->SyncPriority.High = true;
	return true;
}