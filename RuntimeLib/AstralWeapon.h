#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

Bool RTCharacterIsAstralWeaponActive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

Void RTCharacterToggleAstralWeapon(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Bool IsActivation,
	Bool IsNotificationEnabled
);

EXTERN_C_END