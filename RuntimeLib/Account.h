#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCharacterAccountInfo {
	UInt32 CharacterSlotID;
	UInt64 CharacterSlotOrder;
	UInt32 CharacterSlotOpenMask;
	Int32 IsPremium;
	UInt32 ForceGem;
};

#pragma pack(pop)

Void RTCharacterAddForceGem(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 Amount
);

Bool RTCharacterConsumeForceGem(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 Amount
);

Bool RTAccountIsCharacterSlotOpen(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 Index
);

Bool RTAccountOpenCharacterSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

EXTERN_C_END