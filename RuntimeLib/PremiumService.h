#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTPremiumServiceSlot {
	RTItem ItemID;
	UInt64 ItemOptions;
	Int32 UnknownCount;
	Int32 Unknown1;
	Int16 SlotIndex;
	RTItemDuration Duration;
};

struct _RTPremiumServiceInfo {
	UInt8 SlotCount;
};

struct _RTCharacterPremiumServiceInfo {
	struct _RTPremiumServiceInfo Info;
    struct _RTPremiumServiceSlot Slots[RUNTIME_CHARACTER_MAX_PREMIUM_SERVICE_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END