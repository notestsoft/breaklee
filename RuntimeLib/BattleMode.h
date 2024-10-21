#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

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

EXTERN_C_END