#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTPremiumServiceSlot {
	Int64 Index;
	UInt8 Unknown1[16];
	Int16 Order;
	UInt32 Duration;
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