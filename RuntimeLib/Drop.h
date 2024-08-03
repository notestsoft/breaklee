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
    Int32 MinMobLevel;
    Int32 MaxMobLevel;
};

struct _RTDropTable {
    ArrayRef WorldDropPool;
    DictionaryRef MobDropPool;
    DictionaryRef QuestDropPool;
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
