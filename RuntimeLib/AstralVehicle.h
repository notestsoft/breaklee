#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

Bool RTCharacterIsAstralVehicleActive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

Void RTCharacterToggleAstralVehicle(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Bool IsActivation,
	Int32 SkillIndex
);

EXTERN_C_END