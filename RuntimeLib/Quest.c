#include "Character.h"
#include "Event.h"
#include "Npc.h"
#include "Party.h"
#include "Quest.h"
#include "Runtime.h"
#include "World.h"

Void RTCharacterQuestFlagClear(
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	assert(0 <= QuestIndex && QuestIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT);

	Character->QuestFlagInfo.FinishedQuests[QuestIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] &= ~(1 << (QuestIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE));

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTFLAG;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Void RTCharacterQuestFlagSet(
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	assert(0 <= QuestIndex && QuestIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT);

	Character->QuestFlagInfo.FinishedQuests[QuestIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] |= (1 << (QuestIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE));

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTFLAG;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Bool RTCharacterQuestFlagIsSet(
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	assert(0 <= QuestIndex && QuestIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT);

	return (Character->QuestFlagInfo.FinishedQuests[QuestIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] & (1 << (QuestIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE)));
}

Void RTCharacterQuestDeleteFlagClear(
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	assert(0 <= QuestIndex && QuestIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_NORMAL_QUEST_FLAG_COUNT);

	Character->QuestFlagInfo.DeletedQuests[QuestIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] &= ~(1 << (QuestIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE));

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTFLAG;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Void RTCharacterQuestDeleteFlagSet(
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	assert(0 <= QuestIndex && QuestIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_NORMAL_QUEST_FLAG_COUNT);

	Character->QuestFlagInfo.DeletedQuests[QuestIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] |= (1 << (QuestIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE));

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTFLAG;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Bool RTCharacterQuestDeleteFlagIsSet(
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	assert(0 <= QuestIndex && QuestIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_NORMAL_QUEST_FLAG_COUNT);

	return (Character->QuestFlagInfo.DeletedQuests[QuestIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] & (1 << (QuestIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE)));
}

Void RTCharacterDungeonQuestFlagClear(
	RTCharacterRef Character,
	Int32 DungeonIndex
) {
	assert(0 <= DungeonIndex && DungeonIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_QUEST_DUNGEON_FLAG_COUNT);

	Character->DungeonQuestFlagInfo.FinishedDungeons[DungeonIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] &= ~(1 << (DungeonIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE));

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_DUNGEONQUESTFLAG;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Void RTCharacterDungeonQuestFlagSet(
	RTCharacterRef Character,
	Int32 DungeonIndex
) {
	assert(0 <= DungeonIndex && DungeonIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_QUEST_DUNGEON_FLAG_COUNT);

	Character->DungeonQuestFlagInfo.FinishedDungeons[DungeonIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] |= (1 << (DungeonIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE));

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_DUNGEONQUESTFLAG;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Bool RTCharacterDungeonQuestFlagIsSet(
	RTCharacterRef Character,
	Int32 DungeonIndex
) {
	assert(0 <= DungeonIndex && DungeonIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_QUEST_DUNGEON_FLAG_COUNT);

	return (Character->DungeonQuestFlagInfo.FinishedDungeons[DungeonIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] & (1 << (DungeonIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE)));
}

Bool RTCharacterQuestBegin(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 QuestSlotIndex
) {
	if (QuestSlotIndex < 0 || QuestSlotIndex >= RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT) return false;
	if (RTCharacterQuestFlagIsSet(Character, QuestIndex)) return false;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[QuestSlotIndex];
	if (QuestSlot->QuestIndex) return false;

	for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT; Index += 1) {
		if (Character->QuestSlotInfo.QuestSlot[Index].QuestIndex == QuestIndex) return false;
	}

	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
	if (!Quest) return false;

	if (Character->Info.Basic.Level < Quest->Condition.Level ||
		Character->Info.Basic.Level > Quest->Condition.MaxLevel ||
		Character->Info.Honor.Rank < Quest->Condition.MinHonorRank ||
		Character->Info.Honor.Rank > Quest->Condition.MaxHonorRank ||
		Character->Info.Overlord.Level < Quest->Condition.MinOverlordLevel ||
		Character->Info.Overlord.Level > Quest->Condition.MaxOverlordLevel) {
		return false;
	}

	if (Quest->Condition.RelatedQuestID >= 0 && !RTCharacterQuestFlagIsSet(Character, Quest->Condition.RelatedQuestID)) {
		return false;
	}

	if (Quest->QuestBeginNpcIndex >= 0) {
		assert(Quest->QuestBeginNpcIndex < Quest->NpcSet.Count);

		RTQuestNpcDataRef QuestNpc = &Quest->NpcSet.Npcs[Quest->QuestBeginNpcIndex];
		RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, QuestNpc->WorldID, QuestNpc->NpcID);
		if (!Npc) return false;

		if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) {
			return false;
		}

		QuestSlot->NpcActionIndex = QuestNpc->NpcActionOrder;

		if (Quest->QuestBeginNpcIndex + 1 < Quest->NpcSet.Count) {
			RTQuestNpcDataRef NextQuestNpc = &Quest->NpcSet.Npcs[Quest->QuestBeginNpcIndex + 1];
			QuestSlot->NpcActionIndex = NextQuestNpc->NpcActionOrder;
		}
	}

	/* TODO: Check conditions for following fields...
	Int32 Class[2];
	Int32 Skill[2];
	Int32 Item[3];
	Int32 Rank;
	Int32 RankType;
	Int32 NationType;
	*/

	QuestSlot->QuestIndex = Quest->ID;
	QuestSlot->DisplayNotice = 0;
	QuestSlot->DisplayOpenNotice = 0;
	memset(QuestSlot->Counter, 0, sizeof(UInt8) * RUNTIME_MAX_QUEST_COUNTER_COUNT);

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterQuestClear(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 QuestSlotIndex,
	Int32 SlotCount,
	UInt16* SlotIndex,
	UInt64* ResultExp,
	UInt32* ResultSkillExp
) {
	if (QuestSlotIndex < 0 || QuestSlotIndex >= RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT) return false;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[QuestSlotIndex];
	if (QuestSlot->QuestIndex != QuestIndex) return false;

	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
	if (!Quest) return false;
	if (QuestSlot->NpcActionIndex < 0 || QuestSlot->NpcActionIndex >= Quest->NpcSet.Count) return false;

	RTQuestNpcDataRef QuestNpc = &Quest->NpcSet.Npcs[QuestSlot->NpcActionIndex];
	RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, QuestNpc->WorldID, QuestNpc->NpcID);
	if (!Npc) return false;

	if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) {
		return false;
	}

	if (QuestNpc->ActionType != 6) {
		return false;
	}

	Int32 SlotOffset = 0;
	Int32 CounterIndex = 0;
	for (Int32 Index = 0; Index < Quest->MissionMobCount; Index += 1) {
		if (QuestSlot->Counter[CounterIndex] < Quest->MissionMobs[Index].Value[1]) return false;

		CounterIndex += 1;
	}

	if (SlotOffset + Quest->MissionItemCount > SlotCount) return false;

	Int32 ItemCounter[RUNTIME_MAX_QUEST_COUNTER_COUNT] = { 0 };
	for (Int32 Index = 0; Index < Quest->MissionItemCount; Index += 1) {
		RTQuestMissionDataRef MissionData = &Quest->MissionItems[Index];
		ItemCounter[Index] = MissionData->Value[2];
	}

	for (Int32 Index = 0; Index < Quest->MissionItemCount; Index += 1) {
		UInt16 InventorySlotIndex = SlotIndex[SlotOffset + Index];
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, InventorySlotIndex);
		if (!ItemSlot) return false;

		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionItemCount; MissionIndex += 1) {
			RTQuestMissionDataRef MissionData = &Quest->MissionItems[MissionIndex];
			if (MissionData->Value[0] != ItemSlot->Item.ID) continue;

			UInt32 QuestItemOptions = RTQuestItemGetOptions(ItemSlot->ItemOptions);
			if (MissionData->Value[1] != QuestItemOptions) continue;

			UInt32 QuestItemCount = RTQuestItemGetCount(ItemSlot->ItemOptions);
			ItemCounter[MissionIndex] = MAX(0, ItemCounter[MissionIndex] - QuestItemCount);
		}
	}

	for (Int32 Index = 0; Index < Quest->MissionItemCount; Index += 1) {
		if (ItemCounter[Index] > 0) return false;
	}
	
	for (Int32 Index = 0; Index < Quest->MissionItemCount; Index += 1) {
		UInt16 InventorySlotIndex = SlotIndex[SlotOffset + Index];
		Bool Success = RTInventoryClearSlot(Runtime, &Character->InventoryInfo, InventorySlotIndex);
		assert(Success);
	}

	SlotOffset += Quest->MissionItemCount;

	for (Int32 Index = 0; Index < Quest->MissionDungeonCount; Index++) {
		RTQuestMissionDataRef MissionData = &Quest->MissionDungeons[Index];
		if (!RTCharacterDungeonQuestFlagIsSet(Character, MissionData->Value[0])) return false;
	}

	Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);

	if (Quest->Type == RUNTIME_QUEST_TYPE_BATTLE_RANK_UP) {
		if (!RTCharacterBattleRankUp(Runtime, Character)) return false;
	}

	*ResultExp = Quest->Reward[RUNTIME_QUEST_REWARD_EXP];
	*ResultSkillExp = Quest->Reward[RUNTIME_QUEST_REWARD_SKILL_EXP];

	Int32 RewardItemSetID = Quest->Reward[RUNTIME_QUEST_REWARD_ITEM_SET];
	if (RewardItemSetID > 0) {
		if (SlotOffset >= SlotCount) return false;

		UInt16 RewardItemIndex = SlotIndex[SlotOffset++];
		RTQuestRewardItemDataRef RewardItem = RTRuntimeGetQuestRewardItemByIndex(
			Runtime,
			RewardItemSetID,
			RewardItemIndex,
			BattleStyleIndex
		);
		assert(RewardItem);

		if (SlotOffset >= SlotCount) return false;

		struct _RTItemSlot ItemSlot = { 0 };
		ItemSlot.SlotIndex = SlotIndex[SlotOffset++];
		ItemSlot.Item.ID = RewardItem->ItemID[0];
		ItemSlot.ItemOptions = RewardItem->ItemID[1];
		ItemSlot.ItemDuration.DurationIndex = RewardItem->ItemDuration;

		if (!RTInventorySetSlot(Runtime, &Character->InventoryInfo, &ItemSlot)) {
			return false;
		}

		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
		Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
	}

	// TODO: Add missing reward types
	// TODO: Add skill reward type once resolving target skill slot index by skill index

	RTCharacterAddExp(Runtime, Character, Quest->Reward[RUNTIME_QUEST_REWARD_EXP]);
	RTCharacterAddSkillExp(Runtime, Character, Quest->Reward[RUNTIME_QUEST_REWARD_SKILL_EXP]);
	RTCharacterAddHonorPoint(Runtime, Character, Quest->Reward[RUNTIME_QUEST_REWARD_HONOR_POINT]);
	RTCharacterQuestFlagSet(Character, QuestIndex);

	RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	RTRuntimeBroadcastEvent(
		Runtime,
		RUNTIME_EVENT_CHARACTER_UPDATE_SKILL_STATUS,
		World,
		kEntityIDNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y
	);

	memset(QuestSlot, 0, sizeof(struct _RTQuestSlot));
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterQuestCancel(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 QuestSlotIndex
) {
	if (QuestSlotIndex < 0 || QuestSlotIndex >= RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT) return false;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[QuestSlotIndex];
	if (QuestSlot->QuestIndex != QuestIndex) return false;

	memset(QuestSlot, 0, sizeof(struct _RTQuestSlot));
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	// TODO: Reset dungeon state!
	// TODO: Reset added and removed items state!

	return true;
}

Bool RTCharacterQuestDelete(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	// TODO: Do additional cleanup if necessary
	RTCharacterQuestFlagSet(Character, QuestIndex);
	RTCharacterQuestDeleteFlagSet(Character, QuestIndex);
	return true;
}

Bool RTCharacterQuestAction(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 NpcSetIndex,
	Int32 QuestSlotIndex,
	Int32 ActionIndex,
	Int32 ActionSlotIndex
) {
	if (QuestSlotIndex < 0 || QuestSlotIndex >= RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT) return false;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[QuestSlotIndex];
	if (QuestSlot->QuestIndex != QuestIndex) return false;
	if (QuestSlot->NpcActionIndex != ActionIndex) return false;

	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
	if (!Quest) return false;

	if (Quest->NpcSet.ID != NpcSetIndex) return false;
	if (ActionIndex < 0 || ActionIndex >= Quest->NpcSet.Count) return false;

	RTQuestNpcDataRef QuestNpc = &Quest->NpcSet.Npcs[ActionIndex];
	RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, QuestNpc->WorldID, QuestNpc->NpcID);
	if (!Npc) return false;

	// TODO: Add npc action logic!
	if (QuestNpc->ActionType == RUNTIME_QUEST_ACTION_TYPE_TRIGGER) {
		RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
		if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
            World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return false;

		RTDungeonTriggerEvent(World, QuestNpc->Value[0]);
	}

	/* TODO: Quest Dungeon NPC has position 0,0
	if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) {
		return false;
	}
	*/

	if (ActionIndex + 1 < Quest->NpcSet.Count) {
		RTQuestNpcDataRef NextQuestNpc = &Quest->NpcSet.Npcs[ActionIndex + 1];
		QuestSlot->NpcActionIndex = NextQuestNpc->NpcActionOrder;
	}

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterSetQuestDisplayNotice(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 QuestSlotIndex,
	UInt8 DisplayNotice,
	UInt8 DisplayOpenNotice
) {
	if (QuestSlotIndex < 0 || QuestSlotIndex >= RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT) return false;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[QuestSlotIndex];
	if (QuestSlot->QuestIndex != QuestIndex) return false;

	QuestSlot->DisplayNotice = DisplayNotice;
	QuestSlot->DisplayOpenNotice = DisplayOpenNotice;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

// TODO: Notification should be triggered here...
Bool RTCharacterIncrementQuestMobCounter(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Index MobSpeciesIndex
) {
	for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
		if (!QuestSlot->QuestIndex) continue;

		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = 0;
		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionMobCount; MissionIndex += 1) {
			RTQuestMissionDataRef Mission = &Quest->MissionMobs[MissionIndex];
			if (MobSpeciesIndex == Mission->Value[0]) {
				Character->QuestSlotInfo.QuestSlot[SlotIndex].Counter[QuestCounterIndex] = MIN(
					Mission->Value[1],
					Character->QuestSlotInfo.QuestSlot[SlotIndex].Counter[QuestCounterIndex] + 1
				);
				Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
				Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

				return true;
			}

			QuestCounterIndex += 1;
		}
	}

	RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
	if (World->WorldData->Type == RUNTIME_WORLD_TYPE_DUNGEON || World->WorldData->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) {
		RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
		Int32 PatternPartID = DungeonData->PatternPartIndices[World->PatternPartIndex];
		RTMissionDungeonPatternPartDataRef PatternPart = RTRuntimeGetPatternPartByID(Runtime, PatternPartID);
		assert(PatternPart);
	}

	return false;
}

Bool RTCharacterUpdateQuestItemCounter(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItem Item,
	UInt64 ItemOptions
) {
	for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
		if (!QuestSlot->QuestIndex) continue;

		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = Quest->MissionMobCount;
		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionItemCount; MissionIndex += 1) {
			RTQuestMissionDataRef Mission = &Quest->MissionItems[MissionIndex];
			if (Mission->Value[0] == Item.ID && Mission->Value[1] == RTQuestItemGetOptions(ItemOptions)) {
				QuestSlot->Counter[QuestCounterIndex + MissionIndex] = RTQuestItemGetCount(ItemOptions);
				return true;
			}

			QuestCounterIndex += 1;
		}
	}

	return false;
}

Bool RTCharacterHasQuestItemCounter(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItem Item,
	UInt64 ItemOptions
) {
	for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
		if (!QuestSlot->QuestIndex) continue;

		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = Quest->MissionMobCount;
		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionItemCount; MissionIndex += 1) {
			RTQuestMissionDataRef Mission = &Quest->MissionItems[MissionIndex];
			if (Mission->Value[0] == Item.ID && Mission->Value[1] == ItemOptions) {
				return QuestSlot->Counter[QuestCounterIndex + MissionIndex] < Quest->MissionItems[MissionIndex].Value[2];
			}

			QuestCounterIndex += 1;
		}
	}

	return false;
}

Bool RTCharacterHasQuestDungeon(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 DungeonID
) {
	for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
		if (!QuestSlot->QuestIndex) continue;

		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = Quest->MissionMobCount + Quest->MissionItemCount;
		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionDungeonCount; MissionIndex += 1) {
			if (Quest->MissionDungeons[MissionIndex].Value[0] == DungeonID && QuestSlot->Counter[QuestCounterIndex] < 1) {
				return true;
			}

			QuestCounterIndex += 1;
		}
	}

	return false;
}

Void RTPartyQuestFlagClear(
	RTPartyRef Party,
	Int32 QuestIndex
) {
	// TODO: Implementation missing, does party even have flags?
}

Void RTPartyQuestFlagSet(
	RTPartyRef Party,
	Int32 QuestIndex
) {
	// TODO: Implementation missing, does party even have flags?
}

Bool RTPartyQuestFlagIsSet(
	RTPartyRef Party,
	Int32 QuestIndex
) {
	// TODO: Implementation missing, does party even have flags?
	return false;
}

Bool RTCharacterPartyQuestBegin(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 SlotIndex,
	Int16 QuestItemSlotIndex
) {
	if (RTEntityIsNull(Character->PartyID)) return false;
	if (SlotIndex < 0 || SlotIndex >= RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT) return false;
	
	RTPartyRef Party = RTRuntimeGetParty(Runtime, Character->PartyID);
	assert(Party);

    if (RTPartyQuestFlagIsSet(Party, QuestIndex)) return false;

	RTQuestSlotRef QuestSlot = &Party->QuestSlot[SlotIndex];
	if (QuestSlot->QuestIndex > 0) return true;

	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
	if (!Quest) return false;

	if (Character->Info.Basic.Level < Quest->Condition.Level ||
		Character->Info.Basic.Level > Quest->Condition.MaxLevel ||
		Character->Info.Honor.Rank < Quest->Condition.MinHonorRank ||
		Character->Info.Honor.Rank > Quest->Condition.MaxHonorRank ||
		Character->Info.Overlord.Level < Quest->Condition.MinOverlordLevel ||
		Character->Info.Overlord.Level > Quest->Condition.MaxOverlordLevel) {
		return false;
	}

	if (Quest->Condition.RelatedQuestID >= 0 && !RTPartyQuestFlagIsSet(Party, Quest->Condition.RelatedQuestID)) {
		return false;
	}

	if (Quest->QuestBeginNpcIndex >= 0) {
		assert(Quest->QuestBeginNpcIndex < Quest->NpcSet.Count);
		RTQuestNpcDataRef QuestNpc = &Quest->NpcSet.Npcs[Quest->QuestBeginNpcIndex];
		RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, QuestNpc->WorldID, QuestNpc->NpcID);
		if (!Npc) return false;

		if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) {
			return false;
		}

		QuestSlot->NpcActionIndex = QuestNpc->NpcActionOrder;

		if (Quest->QuestBeginNpcIndex + 1 < Quest->NpcSet.Count) {
			RTQuestNpcDataRef NextQuestNpc = &Quest->NpcSet.Npcs[Quest->QuestBeginNpcIndex + 1];
			QuestSlot->NpcActionIndex = NextQuestNpc->NpcActionOrder;
		}
	}

	/* TODO: Check conditions for following fields...
	Int32 Class[2];
	Int32 Skill[2];
	Int32 Item[3];
	Int32 Rank;
	Int32 RankType;
	Int32 NationType;
	*/

	QuestSlot->QuestIndex = Quest->ID;
	memset(QuestSlot->Counter, 0, sizeof(UInt8) * RUNTIME_MAX_QUEST_COUNTER_COUNT);
	return true;
}

// TODO: We should rewind write operations when this thing fails!
Bool RTCharacterPartyQuestClear(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 QuestSlotIndex,
	UInt16 InventorySlotIndices[RUNTIME_MAX_QUEST_COUNTER_COUNT],
	Int32 RewardItemIndex,
	UInt16 RewardItemSlotIndex
) {
	if (RTEntityIsNull(Character->PartyID)) return false;
	if (QuestSlotIndex < 0 || QuestSlotIndex >= RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT) return false;

	RTPartyRef Party = RTRuntimeGetParty(Runtime, Character->PartyID);
	assert(Party);

	if (RTPartyQuestFlagIsSet(Party, QuestIndex)) return false;

	RTQuestSlotRef QuestSlot = &Party->QuestSlot[QuestSlotIndex];
	if (QuestSlot->QuestIndex != QuestIndex) return false;

	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
	if (!Quest) return false;
	if (QuestSlot->NpcActionIndex < 0 || QuestSlot->NpcActionIndex >= Quest->NpcSet.Count) return false;

	RTQuestNpcDataRef QuestNpc = &Quest->NpcSet.Npcs[QuestSlot->NpcActionIndex];
	RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, QuestNpc->WorldID, QuestNpc->NpcID);
	if (!Npc) return false;

	if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) {
		return false;
	}

	if (QuestNpc->ActionType != 6) {
		return false;
	}

	Int32 CounterIndex = 0;
	for (Int32 Index = 0; Index < Quest->MissionMobCount; Index += 1) {
		if (QuestSlot->Counter[CounterIndex] < Quest->MissionMobs[Index].Value[1]) return false;

		CounterIndex += 1;
	}

	// TODO: Rewind if any error happens!
	for (Int32 Index = 0; Index < Quest->MissionItemCount; Index += 1) {
		RTQuestMissionDataRef MissionData = &Quest->MissionItems[Index];

		UInt16 InventorySlotIndex = InventorySlotIndices[Index];
		if (InventorySlotIndex < 0 || InventorySlotIndex >= RUNTIME_PARTY_MAX_INVENTORY_SLOT_COUNT) return false;

		RTItemSlotRef ItemSlot = &Party->Inventory[InventorySlotIndex];
		if (ItemSlot->Item.PartyQuestItemID != MissionData->Value[0]) return false;

		Int32 ItemOptions = RTQuestItemGetOptions(ItemSlot->ItemOptions);
		if (ItemOptions != MissionData->Value[1]) return false;

		Int32 ItemCount = RTQuestItemGetCount(ItemSlot->ItemOptions);
		if (ItemCount < MissionData->Value[2]) return false;

		LogMessageFormat(LOG_LEVEL_WARNING, "Take Slot (%d, %d, %d)", InventorySlotIndex, ItemSlot->Item.ID, ItemSlot->ItemOptions);

		memset(ItemSlot, 0, sizeof(struct _RTItemSlot));
	}

	// TODO: Add dungeon counter check!!!
	for (Int32 Index = 0; Index < Quest->MissionDungeonCount; Index++) {

	}

	// TODO: Check (BattleStyleIndex) shouldn't be necessary in party dungeon quests!
	//       May just pass 0 and verify for it also..
	Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
	Int32 RewardItemSetID = Quest->Reward[RUNTIME_QUEST_REWARD_ITEM_SET];
	RTQuestRewardItemDataRef RewardItem = RTRuntimeGetQuestRewardItemByIndex(
		Runtime,
		RewardItemSetID,
		RewardItemIndex,
		BattleStyleIndex
	);
	if (RewardItem) {
		RTItemSlotRef ItemSlot = &Party->Inventory[RewardItemSlotIndex];
		assert(!ItemSlot->Item.Serial);
		if (ItemSlot->Item.Serial > 0) return false;

		ItemSlot->Item.ID = RewardItem->ItemID[0];
		ItemSlot->ItemOptions = RewardItem->ItemID[1];
		ItemSlot->ItemDuration.DurationIndex = RewardItem->ItemDuration;

		LogMessageFormat(LOG_LEVEL_WARNING, "Give Slot (%d, %d, %d)", RewardItemSlotIndex, ItemSlot->Item.ID, ItemSlot->ItemOptions);
	}

	memset(&Party->QuestSlot[QuestSlotIndex], 0, sizeof(struct _RTQuestSlot));

	RTPartyQuestFlagSet(Party, QuestIndex);

	return true;
}

Bool RTCharacterPartyQuestCancel(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 SlotIndex
) {
	// TODO: Migrate to party
	return true;
}

Bool RTCharacterPartyQuestAction(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 NpcSetIndex,
	Int32 QuestSlotIndex,
	Int32 ActionIndex,
	Int32 ActionSlotIndex
) {
	if (RTEntityIsNull(Character->PartyID)) return false;
	if (QuestSlotIndex < 0 || QuestSlotIndex >= RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT) return false;
	
	RTPartyRef Party = RTRuntimeGetParty(Runtime, Character->PartyID);
	assert(Party);

    if (RTPartyQuestFlagIsSet(Party, QuestIndex)) return false;

	RTQuestSlotRef QuestSlot = &Party->QuestSlot[QuestSlotIndex];
	if (QuestSlot->QuestIndex != QuestIndex) return false;
	if (QuestSlot->NpcActionIndex != ActionIndex) return false;

	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
	if (!Quest) return false;

	if (Quest->NpcSet.ID != NpcSetIndex) return false;
	if (ActionIndex < 0 || ActionIndex >= Quest->NpcSet.Count) return false;

	RTQuestNpcDataRef QuestNpc = &Quest->NpcSet.Npcs[ActionIndex];
	if (QuestNpc->Index != ActionIndex) return false;

	RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, QuestNpc->WorldID, QuestNpc->NpcID);
	if (!Npc) return false;

	if (QuestNpc->ActionType == RUNTIME_QUEST_ACTION_TYPE_GIVE) {
		if (ActionSlotIndex < 0 || ActionSlotIndex >= RUNTIME_PARTY_MAX_INVENTORY_SLOT_COUNT) return false;

		RTItemSlotRef ItemSlot = &Party->Inventory[ActionSlotIndex];
		if (ItemSlot->Item.Serial > 0) return false;

		ItemSlot->Item.ID = QuestNpc->Value[0];
		ItemSlot->ItemOptions = QuestNpc->Value[1];

		LogMessageFormat(LOG_LEVEL_WARNING, "Give item (%d, %d, %d)", ActionSlotIndex, ItemSlot->Item.ID, ItemSlot->ItemOptions);
	}

	if (QuestNpc->ActionType == RUNTIME_QUEST_ACTION_TYPE_TAKE) {
		if (ActionSlotIndex < 0 || ActionSlotIndex >= RUNTIME_PARTY_MAX_INVENTORY_SLOT_COUNT) return false;

		RTItemSlotRef ItemSlot = &Party->Inventory[ActionSlotIndex];
		if (ItemSlot->Item.PartyQuestItemID != QuestNpc->Value[0]) return false;

		// TODO: Sometimes this is not masked check how to test this...
		// Int32 ItemOptions = RTQuestItemGetOptions(ItemSlot->ItemOptions);
		if (ItemSlot->ItemOptions != QuestNpc->Value[1]) return false;

		LogMessageFormat(LOG_LEVEL_WARNING, "Take item (%d, %d, %d)", ActionSlotIndex, ItemSlot->Item.ID, ItemSlot->ItemOptions);

		memset(ItemSlot, 0, sizeof(struct _RTItemSlot));
	}

	if (QuestNpc->ActionType == RUNTIME_QUEST_ACTION_TYPE_GIVE_MAP_CODE) {
		assert(true);
	}

	if (QuestNpc->ActionType == RUNTIME_QUEST_ACTION_TYPE_GIVE_WARP_CODE) {
		assert(true);
	}

	if (QuestNpc->ActionType == RUNTIME_QUEST_ACTION_TYPE_GIVE_ALZ) {
		assert(true);
	}

	if (QuestNpc->ActionType == RUNTIME_QUEST_ACTION_TYPE_TAKE_ALZ) {
		assert(true);
	}

	if (QuestNpc->ActionType == RUNTIME_QUEST_ACTION_TYPE_TRIGGER) {
		RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
		if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON && World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return false;

		RTDungeonTriggerEvent(World, QuestNpc->Value[0]);
	}

	/* TODO: Quest Dungeon NPC has position 0,0
	if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) {
		return false;
	}
	*/

	if (ActionIndex + 1 < Quest->NpcSet.Count) {
		RTQuestNpcDataRef NextQuestNpc = &Quest->NpcSet.Npcs[ActionIndex + 1];
		QuestSlot->NpcActionIndex = NextQuestNpc->Index; // TODO: Check how to use NpcActionOrder as some are grouped together when they are sequencial in same packet
	}

	return true;
}

Bool RTPartyIncrementQuestMobCounter(
	RTRuntimeRef Runtime,
	RTEntityID PartyID,
	Index MobSpeciesIndex
) {
	if (RTEntityIsNull(PartyID)) return false;

	RTPartyRef Party = RTRuntimeGetParty(Runtime, PartyID);

	for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Party->QuestSlot[SlotIndex];
		if (!QuestSlot->QuestIndex) continue;

		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = 0;

		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionMobCount; MissionIndex += 1) {
			RTQuestMissionDataRef Mission = &Quest->MissionMobs[MissionIndex];
			if (MobSpeciesIndex == Mission->Value[0]) {
				QuestSlot->Counter[QuestCounterIndex] = MIN(
					Mission->Value[1],
					QuestSlot->Counter[QuestCounterIndex] + 1
				);

				RTWorldContextRef World = RTRuntimeGetWorldByParty(Runtime, PartyID);
				assert(World);

				RTEventData EventData = { 0 };
				EventData.PartyQuestMissionMobKill.QuestID = Quest->ID;
				EventData.PartyQuestMissionMobKill.MobSpeciesIndex = MobSpeciesIndex;
				RTRuntimeBroadcastEventData(
					Runtime,
					RUNTIME_EVENT_PARTY_QUEST_MISSION_MOB_KILL,
					World,
					kEntityIDNull,
					PartyID,
					0,
					0,
					EventData
				);

				return true;
			}

			QuestCounterIndex += 1;
		}
	}

	return false;
}

Bool RTPartyHasQuestItemCounter(
	RTRuntimeRef Runtime,
	RTPartyRef Party,
	RTItem Item,
	UInt64 ItemOptions
) {
	for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Party->QuestSlot[SlotIndex];
		if (!QuestSlot->QuestIndex) continue;

		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = Quest->MissionMobCount;

		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionItemCount; MissionIndex += 1) {
			if (QuestSlot->Counter[QuestCounterIndex] >= Quest->MissionItems[MissionIndex].Value[2]) {
				continue;
			}

			RTQuestMissionDataRef Mission = &Quest->MissionItems[MissionIndex];
			if (Mission->Value[0] == Item.PartyQuestItemID && Mission->Value[1] == ItemOptions) {
				return true;
			}

			QuestCounterIndex += 1;
		}
	}

	return false;
}

Bool RTCharacterHasPartyQuestDungeon(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 DungeonID
) {
	for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
		if (!QuestSlot->QuestIndex) continue;

		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = Quest->MissionMobCount + Quest->MissionItemCount;

		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionDungeonCount; MissionIndex += 1) {
			if (Quest->MissionDungeons[MissionIndex].Value[0] == DungeonID && QuestSlot->Counter[QuestCounterIndex] < 1) {
				return true;
			}

			QuestCounterIndex += 1;
		}
	}

	return false;
}
