#pragma once

#include "Base.h"
#include "Constants.h"
#include "Entity.h"
#include "Item.h"
#include "Mercenary.h"
#include "Quest.h"

EXTERN_C_BEGIN

enum {
    RUNTIME_PARTY_TYPE_NORMAL       = 0,
    RUNTIME_PARTY_TYPE_SOLO_DUNGEON = 1,
};

#pragma pack(push, 1)

struct _RTPartyMemberInfo {
    UInt32 CharacterIndex;
    Int32 Level;
    Int32 DungeonIndex;
    UInt8 Unknown1;
    UInt8 IsEliteDungeon;
    UInt8 IsEventDungeon;
    UInt8 Unknown2;
    UInt8 WorldServerIndex;
    UInt8 BattleStyleIndex;
    Int32 WorldIndex;
    UInt16 OverlordLevel;
    Int32 MythRebirth;
    Int32 MythHolyPower;
    Int32 MythLevel;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    UInt8 NameLength;
    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
    struct _RTMercenarySlot Mercenaries[RUNTIME_PARTY_MAX_MERCENARY_COUNT];
};

#pragma pack(pop)

struct _RTPartySlot {
	Int32 SlotIndex;
    RTEntityID MemberID; // TODO: Remove
    struct _RTPartyMemberInfo Info;
};

struct _RTPartyInvitation {
    RTEntityID PartyID;
    Index InviterCharacterIndex;
    struct _RTPartySlot Member;
    Timestamp InvitationTimestamp;
};

struct _RTParty {
	RTEntityID ID;
    Index LeaderCharacterIndex;
    Index WorldServerIndex;
	Int32 PartyType;
	Int32 MemberCount;
	struct _RTPartySlot Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
	struct _RTItemSlot Inventory[RUNTIME_PARTY_MAX_INVENTORY_SLOT_COUNT];
	struct _RTQuestSlot QuestSlot[RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT];
};

Bool RTPartyIsSoloDungeon(
    RTEntityID PartyID
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

Bool RTPartyChangeLeader(
    RTPartyRef Party,
    Index CharacterIndex
);

EXTERN_C_END
