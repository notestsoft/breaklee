#pragma once

#include "Base.h"

#pragma pack(push, 1)

enum {
    RUNTIME_ENTITY_TYPE_NONE		= 0,
	RUNTIME_ENTITY_TYPE_CHARACTER	= 1,
	RUNTIME_ENTITY_TYPE_MOB			= 2,
	RUNTIME_ENTITY_TYPE_ITEM		= 3,
	RUNTIME_ENTITY_TYPE_PARTY		= 4,
	RUNTIME_ENTITY_TYPE_OBJECT		= 5,

	RUNTIME_ENTITY_TYPE_TOTEM		= 11,
};

struct _RTEntityID {
	union {
		struct {
			UInt16 EntityIndex;
			UInt8 WorldIndex;
			UInt8 EntityType;
		};
		UInt32 Serial;
	};
};
typedef struct _RTEntityID RTEntityID;

typedef Void (*RTEntityVisitorCallback)(
	RTEntityID Entity,
	Void* Userdata
);

static const RTEntityID kEntityIDNull = { .EntityIndex = 0, .WorldIndex = 0, .EntityType = 0 };

UInt32 RTEntityGetSerial(
	RTEntityID Entity
);

Bool RTEntityIsNull(
	RTEntityID Entity
);

Bool RTEntityIsEqual(
	RTEntityID Entity, 
	RTEntityID Other
);

DictionaryRef EntityDictionaryCreate(
	AllocatorRef Allocator,
	Int Capacity
);

#pragma pack(pop)
