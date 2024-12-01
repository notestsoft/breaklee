#include "IPCNodeID.h"

Bool IPCNodeIDIsNull(
    IPCNodeID NodeID
) {
    return (
        NodeID.Index == 0 &&
        NodeID.Group == 0 &&
        NodeID.Type == 0
    );
}

Bool IPCNodeIDIsEqual(
    IPCNodeID Lhs, 
    IPCNodeID Rhs
) {
    return (
        Lhs.Index == Rhs.Index &&
        Lhs.Group == Rhs.Group &&
        Lhs.Type == Rhs.Type
    );
}

Bool _IPCNodeIDDictionaryKeyComparator(
    Void* Lhs,
    Void* Rhs
) {
    return IPCNodeIDIsEqual(*(IPCNodeID*)Lhs, *(IPCNodeID*)Rhs);
}

UInt64 _IPCNodeIDDictionaryKeyHasher(
    Void* Key
) {
    UInt64 Hash = 5381;
    Hash = Hash * 33 + ((IPCNodeID*)Key)->Index;
    Hash = Hash * 33 + ((IPCNodeID*)Key)->Group;
    Hash = Hash * 33 + ((IPCNodeID*)Key)->Type;
    return Hash;
}

Int32 _IPCNodeIDDictionaryKeySizeCallback(
    Void* Key
) {
    return sizeof(IPCNodeID);
}

DictionaryRef IPCNodeIDDictionaryCreate(
    AllocatorRef Allocator,
    Int Capacity
) {
    return DictionaryCreate(
        Allocator,
        &_IPCNodeIDDictionaryKeyComparator,
        &_IPCNodeIDDictionaryKeyHasher,
        &_IPCNodeIDDictionaryKeySizeCallback,
        Capacity
    );
}
