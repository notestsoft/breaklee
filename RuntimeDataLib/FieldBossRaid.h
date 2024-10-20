RUNTIME_DATA_FILE_BEGIN(FieldBossRaid.enc)

RUNTIME_DATA_TYPE_BEGIN(FieldBossRaidTime, "FieldBossRaid.RaidTime.FieldBossinfo")
	RUNTIME_DATA_PROPERTY(Int32, RaidStartHour, "RaidStartHour")
	RUNTIME_DATA_PROPERTY(Int32, RaidStartMinutes, "RaidStartMin")
	RUNTIME_DATA_PROPERTY(Int32, RaidDelaySeconds, "RaidWaitSec")
	RUNTIME_DATA_PROPERTY(Int32, RaidDurationSeconds, "RaidDurationSec")
RUNTIME_DATA_TYPE_END(FieldBossRaidTime)

RUNTIME_DATA_TYPE_BEGIN(FieldBossRaidInfo, "FieldBossRaid.RaidBoss.FieldBossinfo")
	RUNTIME_DATA_PROPERTY(Int32, WorldIndex, "WorldIdx")
	RUNTIME_DATA_PROPERTY(Int32, MobSpeciesIndex, "BossIdx")
	RUNTIME_DATA_PROPERTY(Int32, MinRequiredScore, "MinPointInd")
RUNTIME_DATA_TYPE_END(FieldBossRaidInfo)

RUNTIME_DATA_TYPE_BEGIN(FieldBossRaidSpawn, "FieldBossRaid.BossInfo.Client_BossInfo")
	RUNTIME_DATA_PROPERTY(Int32, WorldIndex, "WorldIdx")
	RUNTIME_DATA_PROPERTY(Int32, MobSpeciesIndex, "BossIdx")
	RUNTIME_DATA_PROPERTY(Int32, PositionX, "pos_x")
	RUNTIME_DATA_PROPERTY(Int32, PositionY, "pos_y")
RUNTIME_DATA_TYPE_END(FieldBossRaidSpawn)

RUNTIME_DATA_FILE_END
