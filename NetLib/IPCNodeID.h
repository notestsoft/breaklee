#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _IPCNodeID {
    Int Index;
    Int Group;
    Int Type;
};
typedef struct _IPCNodeID IPCNodeID;

static const IPCNodeID kIPCNodeIDNull = { 0, 0, 0 };

#pragma pack(pop)

Bool IPCNodeIDIsNull(
    IPCNodeID NodeID
);

Bool IPCNodeIDIsEqual(
    IPCNodeID Lhs,
    IPCNodeID Rhs
);

DictionaryRef IPCNodeIDDictionaryCreate(
    AllocatorRef Allocator,
    Int Capacity
);

EXTERN_C_END
