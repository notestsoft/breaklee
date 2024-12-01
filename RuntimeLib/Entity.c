#include "Entity.h"

UInt32 RTEntityGetSerial(
    RTEntityID Entity
) {
    return *(UInt32*)(&Entity);
}

Bool RTEntityIsNull(
    RTEntityID Entity
) {
    return (
        Entity.EntityIndex == 0 &&
        Entity.WorldIndex == 0 &&
        Entity.EntityType == 0
    );
}

Bool RTEntityIsEqual(
    RTEntityID Entity, 
    RTEntityID Other
) {
    UInt32 Lhs = *(UInt32 *)(&Entity);
    UInt32 Rhs = *(UInt32 *)(&Other);
    return Lhs == Rhs;
}

Bool _EntityDictionaryKeyComparator(
    Void* Lhs,
    Void* Rhs
) {
    return RTEntityIsEqual((*(RTEntityID*)Lhs), (*(RTEntityID*)Rhs));
}

UInt64 _EntityDictionaryKeyHasher(
    Void* Key
) {
    return (UInt64)RTEntityGetSerial((*(RTEntityID*)Key));
}

Int32 _EntityDictionaryKeySizeCallback(
    Void* Key
) {
    return sizeof(RTEntityID);
}

DictionaryRef EntityDictionaryCreate(
    AllocatorRef Allocator,
    Int Capacity
) {
    return DictionaryCreate(
        Allocator,
        &_EntityDictionaryKeyComparator,
        &_EntityDictionaryKeyHasher,
        &_EntityDictionaryKeySizeCallback,
        Capacity
    );
}
