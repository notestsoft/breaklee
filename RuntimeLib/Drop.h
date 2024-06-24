#pragma once

#include "Base.h"
#include "Constants.h"
#include "Character.h"
#include "Mob.h"

EXTERN_C_BEGIN

struct _RTDropItem {
    RTItem ItemID;
    UInt64 ItemOptions;
    Int32 DropRate;
    Int32 OptionPoolIndex;
    Int32 DurationIndex;
};

struct _RTWorldDropItem {
    struct _RTDropItem Item;
    Int32 MinMobLevel;
    Int32 MaxMobLevel;
};

struct _RTMobDropItem {
    struct _RTDropItem Item;
    Int32 MobSpeciesIndex;
};

struct _RTQuestDropItem {
    struct _RTDropItem Item;
    Int32 MobSpeciesIndex;
};

struct _RTDropTable {
    Int32 WorldItemCount;
    Int32 MobItemCount;
    Int32 QuestItemCount;
    struct _RTWorldDropItem WorldItems[RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT];
    struct _RTMobDropItem MobItems[RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT];
    struct _RTQuestDropItem QuestItems[RUNTIME_MEMORY_MAX_QUEST_DROP_ITEM_COUNT];
};

struct _RTDropResult {
    RTItem ItemID;
    UInt64 ItemOptions;
    RTItemDuration ItemDuration;
    RTItemProperty ItemProperty;
};
typedef struct _RTDropResult RTDropResult;

Bool RTCalculateDrop(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTCharacterRef Character,
    RTDropResultRef Result
);

Bool RTDropCurrency(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTCharacterRef Character
);

Bool RTCalculateQuestDrop(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTCharacterRef Character,
    RTDropResultRef Result
);

Bool RTCalculatePartyQuestDrop(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTPartyRef Party,
    RTDropResultRef Result
);

EXTERN_C_END
