#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define RUNTIME_MAX_FIELD_BOSS_RAID_SCHEDULE_WEEK_COUNT 12
#define RUNTIME_MAX_FIELD_BOSS_RAID_SCHEDULE_COUNT (7 * RUNTIME_MAX_FIELD_BOSS_RAID_SCHEDULE_WEEK_COUNT)

struct _RTFieldBossRaidSchedule {
	Timestamp StartTimestamp;
	Int32 WorldIndex;
	Int32 MobSpeciesIndex;
};

struct _RTFieldBossRaidManager {
	Int32 ScheduleWeekCount;

};

#pragma pack(pop)

EXTERN_C_END