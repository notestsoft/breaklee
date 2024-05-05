#include "PartyManager.h"

RTPartyManagerRef RTPartyManagerCreate(
    AllocatorRef Allocator,
    Index MaxPartyCount
) {
    RTPartyManagerRef PartyManager = AllocatorAllocate(Allocator, sizeof(struct _RTPartyManager));
    if (!PartyManager) Fatal("Memory allocation failed");

    PartyManager->PartyPool = MemoryPoolCreate(Allocator, sizeof(struct _RTParty), MaxPartyCount);
    PartyManager->PartyInvitationPool = MemoryPoolCreate(Allocator, sizeof(struct _RTPartyInvitation), MaxPartyCount);
    PartyManager->CharacterToPartyEntity = IndexDictionaryCreate(Allocator, MaxPartyCount);
    PartyManager->CharacterToPartyInvite = IndexDictionaryCreate(Allocator, MaxPartyCount);

    Index NullIndex = 0;
    MemoryPoolReserve(PartyManager->PartyPool, NullIndex);
    MemoryPoolReserve(PartyManager->PartyInvitationPool, NullIndex);

    return PartyManager;
}

Void RTPartyManagerDestroy(
    RTPartyManagerRef PartyManager
) {
    MemoryPoolDestroy(PartyManager->PartyPool);
    MemoryPoolDestroy(PartyManager->PartyInvitationPool);
    DictionaryDestroy(PartyManager->CharacterToPartyEntity);
    DictionaryDestroy(PartyManager->CharacterToPartyInvite);
    AllocatorDeallocate(PartyManager->Allocator, PartyManager);
}

RTPartyRef RTPartyManagerCreateParty(
    RTPartyManagerRef PartyManager,
    Index CharacterIndex,
    RTEntityID CharacterID,
    Int32 PartyType
) {
    assert(!DictionaryLookup(PartyManager->CharacterToPartyEntity, &CharacterIndex));

    Index PartyPoolIndex = 0;
    RTPartyRef Party = (RTPartyRef)MemoryPoolReserveNext(PartyManager->PartyPool, &PartyPoolIndex);
    Party->ID.EntityIndex = (UInt16)PartyPoolIndex;
    Party->ID.EntityType = RUNTIME_ENTITY_TYPE_PARTY;
    Party->LeaderID = CharacterID;
    Party->LeaderCharacterIndex = CharacterIndex;
    Party->PartyType = PartyType;
    RTPartyAddMember(Party, CharacterIndex, CharacterID);

    RTEntityID PartyID = { 0 };
    PartyID.EntityIndex = (UInt16)PartyPoolIndex;
    PartyID.EntityType = RUNTIME_ENTITY_TYPE_PARTY;
    DictionaryInsert(PartyManager->CharacterToPartyEntity, &CharacterIndex, &PartyID, sizeof(struct _RTEntityID));

    return (RTPartyRef)MemoryPoolFetch(PartyManager->PartyPool, PartyPoolIndex);
}

RTPartyRef RTPartyManagerCreatePartyRemote(
    RTPartyManagerRef PartyManager,
    RTPartyRef RemoteParty
) {
    Index PartyPoolIndex = RemoteParty->ID.EntityIndex;
    RTPartyRef Party = (RTPartyRef)MemoryPoolReserve(PartyManager->PartyPool, &PartyPoolIndex);
    memcpy(Party, RemoteParty, sizeof(struct _RTParty));

    for (Index MemberIndex = 0; MemberIndex < Party->MemberCount; MemberIndex += 1) {
        Index CharacterIndex = RemoteParty->Members[MemberIndex].Info.CharacterIndex;
        DictionaryInsert(PartyManager->CharacterToPartyEntity, &CharacterIndex, &Party->ID, sizeof(struct _RTEntityID));
    }

    return Party;
}

Void RTPartyManagerDestroyParty(
    RTPartyManagerRef PartyManager,
    RTPartyRef Party
) {
    // TODO: Cleanup also all invitations to this party!!!

    for (Index MemberIndex = 0; MemberIndex < Party->MemberCount; MemberIndex += 1) {
        Index CharacterIndex = Party->Members[MemberIndex].Info.CharacterIndex;
        DictionaryRemove(PartyManager->CharacterToPartyEntity, &CharacterIndex);
    }

    Index PartyPoolIndex = Party->ID.EntityIndex;
    MemoryPoolRelease(PartyManager->PartyPool, PartyPoolIndex);
}

RTPartyRef RTPartyManagerGetPartyByCharacter(
    RTPartyManagerRef PartyManager,
    Index CharacterIndex
) {
    RTEntityID* PartyID = (RTEntityID*)DictionaryLookup(PartyManager->CharacterToPartyEntity, &CharacterIndex);
    if (!PartyID) return NULL;

    Index PartyPoolIndex = PartyID->EntityIndex;
    return (RTPartyRef)MemoryPoolFetch(PartyManager->PartyPool, PartyPoolIndex);
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
