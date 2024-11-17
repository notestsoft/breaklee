#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTBattleModeInfo {
	Int32 BattleModeDuration;
	UInt8 BattleModeIndex;
	UInt8 BattleModeOverCharge;
	UInt8 BattleModeStyleRank;
	UInt8 AuraModeIndex;
	UInt8 AuraModeOverCharge;
	UInt8 AuraModeStyleRank;
	Int32 AuraModeDuration;
};

struct _RTCharacterBattleModeInfo {
	struct _RTBattleModeInfo Info;
	Int32 VehicleState;
};

#pragma pack(pop)

Bool RTCharacterIsBattleModeActive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

Bool RTCharacterStartBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillIndex
);

Void RTCharacterUpdateBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

Bool RTCharacterCancelBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

Bool RTCharacterIsAuraModeActive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

Bool RTCharacterStartAuraMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillIndex
);

Bool RTCharacterCancelAuraMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

Void RTCharacterInitializeBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

EXTERN_C_END