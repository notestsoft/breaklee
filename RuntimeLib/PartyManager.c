#include "PartyManager.h"

RTPartyManagerRef RTPartyManagerCreate(
    AllocatorRef Allocator,
    Index MaxPartyCount
) {
    RTPartyManagerRef PartyManager = AllocatorAllocate(Allocator, sizeof(struct _RTPartyManager));
    if (!PartyManager) FatalError("Memory allocation failed!");

    PartyManager->Allocator = Allocator;
    PartyManager->MaxPartyCount = MaxPartyCount;
    PartyManager->PartyPool = MemoryPoolCreate(Allocator, sizeof(struct _RTParty), MaxPartyCount);
    PartyManager->CharacterToPartyEntity = IndexDictionaryCreate(Allocator, MaxPartyCount * RUNTIME_PARTY_MAX_MEMBER_COUNT);

    MemoryPoolReserve(PartyManager->PartyPool, 0);

    return PartyManager;
}

Void RTPartyManagerDestroy(
    RTPartyManagerRef PartyManager
) {
    MemoryPoolDestroy(PartyManager->PartyPool);
    DictionaryDestroy(PartyManager->CharacterToPartyEntity);
    AllocatorDeallocate(PartyManager->Allocator, PartyManager);
}

Void RTPartyManagerUpdate(
    RTPartyManagerRef PartyManager
) {
    
}

/*
RTEntityID RTPartyManagerCreateParty(
    RTPartyManagerRef PartyManager,
    Index CharacterIndex,
    RTEntityID CharacterID,
    Int32 PartyType
) {
    if (DictionaryLookup(PartyManager->CharacterToPartyEntity, CharacterIndex)) return kEntityIDNull;
    
    Index PartyPoolIndex = 0;
    RTPartyRef Party = (RTPartyRef)MemoryPoolReserveNext(PartyManager->PartyPool, &PartyPoolIndex);
    Party->ID.EntityIndex = (UInt16)PartyPoolIndex;
    Party->ID.EntityType = RUNTIME_ENTITY_TYPE_PARTY;
    Party->LeaderID = CharacterID;
    Party->PartyType = PartyType;
    RTPartyAddMember(Party, CharacterIndex, CharacterID);

    RTEntityID PartyID = { 0 };
    PartyID.EntityIndex = (UInt16)PartyPoolIndex;
    PartyID.EntityType = RUNTIME_ENTITY_TYPE_PARTY;
    DictionaryInsert(PartyManager->CharacterToPartyEntity, &CharacterIndex, &PartyID, sizeof(struct _RTEntityID));

    return PartyID;
}

RTPartyRef RTPartyManagerGetParty(
    RTPartyManagerRef PartyManager,
    RTEntityID PartyID
) {
    assert(PartyID.EntityType == RUNTIME_ENTITY_TYPE_PARTY);

    Index PartyPoolIndex = PartyID.EntityIndex;
    if (!MemoryPoolIsReserved(PartyManager->PartyPool, PartyPoolIndex)) return NULL;

    return (RTPartyRef)MemoryPoolReserveNext(PartyManager->PartyPool, &PartyPoolIndex);
}

Bool RTPartyManagerInviteMember(
    RTPartyManagerRef PartyManager,
    RTPartyRef Party,
    Index CharacterIndex,
    RTEntityID CharacterID
) {
    if (Party->MemberCount >= RUNTIME_PARTY_MAX_MEMBER_COUNT) return false;
    if (DictionaryLookup(PartyManager->CharacterToPartyEntity, &CharacterIndex)) return false;

    return RTPartyAddMember(Party, CharacterIndex, CharacterID);
}

Bool RTPartyManagerExpelMember(
    RTPartyManagerRef PartyManager,
    RTPartyRef Party,
    Index CharacterIndex,
    RTEntityID CharacterID
) {
    return RTPartyRemoveMember(Party, CharacterIndex, CharacterID);
}*/