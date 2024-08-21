#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTResearchSupportMaterialSlot {
    UInt8 SlotIndex;
    RTItem ItemID;
    UInt64 Unknown1;
    UInt64 Unknown2;
    UInt32 Unknown3;
    UInt16 Unknown4;
};

struct _RTResearchSupportMissionSlot {
    UInt8 SlotIndex;
    UInt8 UnknownIndex;
    Int16 Exp;
    RTItem ItemID;
    Int64 IsSlotOpen;
    Int32 IsRewardClaimed;
};

struct _RTResearchSupportMissionBoard {
    struct _RTResearchSupportMissionSlot Missions[RUNTIME_CHARACTER_MAX_RESEARCH_SUPPORT_SLOT_COUNT];
};

struct _RTCharacterResearchSupportInfo {
    Int32 Exp;
    Int32 DecodedCircuitCount;
    UInt8 Unknown1;
    UInt8 ResetCount;
    UInt8 Unknown2;
    Timestamp SeasonStartDate;
    Timestamp SeasonEndDate;
    struct _RTResearchSupportMaterialSlot MaterialSlots[RUNTIME_CHARACTER_MAX_RESEARCH_SUPPORT_MATERIAL_COUNT];
    struct _RTResearchSupportMissionBoard ActiveMissionBoard;
    UInt8 Unknown3[40];
    struct _RTResearchSupportMissionBoard MissionBoards[RUNTIME_CHARACTER_MAX_RESEARCH_SUPPORT_BOARD_COUNT];
};

#pragma pack(pop)

EXTERN_C_END