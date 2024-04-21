#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"
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
    RTEntityID MemberID; 
    Int32 Level;
    UInt16 OverlordLevel;
    Int32 MythRebirth;
    Int32 MythHolyPower;
    Int32 MythLevel;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    UInt8 NameLength;
    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
};

struct _RTPartyInvitation {
    Index CharacterIndex;
    UInt8 WorldServerID;
    UInt8 NameLength;
    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
    Timestamp InvitationTimestamp;
};

struct _RTParty {
	RTEntityID ID;
	RTEntityID LeaderID;
    Index LeaderCharacterIndex;
	Int32 PartyType;
	Int32 MemberCount;
	Int32 InvitationCount;
	struct _RTPartySlot Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
	struct _RTItemSlot Inventory[RUNTIME_PARTY_MAX_INVENTORY_SLOT_COUNT];
	struct _RTQuestSlot QuestSlot[RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT];
    struct _RTPartyInvitation Invitations[RUNTIME_PARTY_MAX_MEMBER_COUNT];
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

Void RTPartyAddMember(
    RTPartyRef Party,
    Index CharacterIndex,
    RTEntityID CharacterID
);

EXTERN_C_END
