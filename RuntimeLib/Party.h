#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"
#include "Entity.h"
#include "Quest.h"

EXTERN_C_BEGIN

enum {
    RUNTIME_PARTY_TYPE_NORMAL       = 0,
    RUNTIME_PARTY_TYPE_SOLO_DUNGEON = 1,
    RUNTIME_PARTY_TYPE_TEMPORARY    = 2,
};

struct _RTPartySlot {
	Int32 SlotIndex;
    Index CharacterIndex;
    Index NodeIndex;
    RTEntityID MemberID; // TODO: Remove
    Int32 Level;
    UInt16 OverlordLevel;
    Int32 MythRebirth;
    Int32 MythHolyPower;
    Int32 MythLevel;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1];
};

struct _RTPartyInvitation {
    struct _RTPartySlot Member;
    Timestamp InvitationTimestamp;
};

struct _RTParty {
	RTEntityID ID;
	RTEntityID LeaderID; // TODO: Remove
    Index LeaderCharacterIndex;
	Int32 PartyType;
	Int32 MemberCount;
	struct _RTPartySlot Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
	struct _RTItemSlot Inventory[RUNTIME_PARTY_MAX_INVENTORY_SLOT_COUNT];
	struct _RTQuestSlot QuestSlot[RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT];
};

Void RTPartyQuestFlagClear(
    RTPartyRef Party,
    Int32 QuestIndex
);

Void RTPartyQuestFlagSet(
    RTPartyRef Party,
    Int32 QuestIndex
);

Bool RTPartyQuestFlagIsSet(
    RTPartyRef Party,
    Int32 QuestIndex
);

Bool RTCharacterPartyQuestBegin(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex,
    Int16 QuestItemSlotIndex
);

Bool RTCharacterPartyQuestClear(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 QuestSlotIndex,
    UInt16 InventorySlotIndices[RUNTIME_MAX_QUEST_COUNTER_COUNT],
    Int32 RewardItemIndex,
    UInt16 RewardItemSlotIndex
);

Bool RTCharacterPartyQuestCancel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex
);

Bool RTCharacterPartyQuestAction(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 NpcSetIndex,
    Int32 SlotIndex,
    Int32 ActionIndex,
    Int32 ActionSlotIndex
);

RTPartySlotRef RTPartyGetMember(
    RTPartyRef Party,
    Index CharacterIndex
);

RTPartySlotRef RTPartyAddMember(
    RTPartyRef Party,
    Index CharacterIndex,
    RTEntityID CharacterID
);

Void RTPartyQuestFlagClear(
    RTPartyRef Party,
    Int32 QuestIndex
);

Void RTPartyQuestFlagSet(
    RTPartyRef Party,
    Int32 QuestIndex
);

Bool RTPartyQuestFlagIsSet(
    RTPartyRef Party,
    Int32 QuestIndex
);

Bool RTCharacterPartyQuestBegin(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex,
    Int16 QuestItemSlotIndex
);

Bool RTCharacterPartyQuestClear(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 QuestSlotIndex,
    UInt16 InventorySlotIndices[RUNTIME_MAX_QUEST_COUNTER_COUNT],
    Int32 RewardItemIndex,
    UInt16 RewardItemSlotIndex
);

Bool RTCharacterPartyQuestCancel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex
);

Bool RTCharacterPartyQuestAction(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 NpcSetIndex,
    Int32 SlotIndex,
    Int32 ActionIndex,
    Int32 ActionSlotIndex
);

Bool RTPartyIncrementQuestMobCounter(
    RTRuntimeRef Runtime,
    RTEntityID PartyID,
    Index MobSpeciesIndex
);

Bool RTPartyHasQuestItemCounter(
    RTRuntimeRef Runtime,
    RTPartyRef Party,
    RTItem Item,
    UInt64 ItemOptions
);

Bool RTCharacterHasPartyQuestDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 DungeonID
);

EXTERN_C_END
