#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

ClientContextRef ServerGetClientByIndex(
    ServerContextRef Context,
    UInt32 CharacterIndex,
    CString CharacterName,
    Int32 CharacterNameLength
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client && Client->CharacterIndex == CharacterIndex && memcmp(Client->CharacterName, CharacterName, CharacterNameLength) == 0) {
            return Client;
        }
    }

    return NULL;
}

RTPartyRef ServerCreateParty(
    ServerContextRef Context,
    Index CharacterIndex,
    RTEntityID CharacterID,
    Int32 PartyType
) {
    assert(!DictionaryLookup(Context->CharacterToPartyEntity, &CharacterIndex));

    Index PartyPoolIndex = 0;
    RTPartyRef Party = (RTPartyRef)MemoryPoolReserveNext(Context->PartyPool, &PartyPoolIndex);
    Party->ID.EntityIndex = (UInt16)PartyPoolIndex;
    Party->ID.EntityType = RUNTIME_ENTITY_TYPE_PARTY;
    Party->LeaderID = CharacterID;
    Party->LeaderCharacterIndex = CharacterIndex;
    Party->PartyType = PartyType;
    RTPartyAddMember(Party, CharacterIndex, CharacterID);

    RTEntityID PartyID = { 0 };
    PartyID.EntityIndex = (UInt16)PartyPoolIndex;
    PartyID.EntityType = RUNTIME_ENTITY_TYPE_PARTY;
    DictionaryInsert(Context->CharacterToPartyEntity, &CharacterIndex, &PartyID, sizeof(struct _RTEntityID));

    return (RTPartyRef)MemoryPoolFetch(Context->PartyPool, PartyPoolIndex);
}

Void ServerDestroyParty(
    ServerContextRef Context,
    RTPartyRef Party
) {
    // TODO: Cleanup also all invitations to this party!!!

    for (Index MemberIndex = 0; MemberIndex < Party->MemberCount; MemberIndex += 1) {
        Index CharacterIndex = Party->Members[MemberIndex].CharacterIndex;
        DictionaryRemove(Context->CharacterToPartyEntity, &CharacterIndex);
    }

    Index PartyPoolIndex = Party->ID.EntityIndex;
    MemoryPoolRelease(Context->PartyPool, PartyPoolIndex);
}

RTPartyRef ServerGetPartyByCharacter(
    ServerContextRef Context,
    Index CharacterIndex
) {
    RTEntityID* PartyID = (RTEntityID*)DictionaryLookup(Context->CharacterToPartyEntity, &CharacterIndex);
    if (!PartyID) return NULL;

    Index PartyPoolIndex = PartyID->EntityIndex;
    return (RTPartyRef)MemoryPoolFetch(Context->PartyPool, PartyPoolIndex);
}

RTPartyRef ServerGetParty(
    ServerContextRef Context,
    RTEntityID PartyID
) {
    assert(PartyID.EntityType == RUNTIME_ENTITY_TYPE_PARTY);

    Index PartyPoolIndex = PartyID.EntityIndex;
    if (!MemoryPoolIsReserved(Context->PartyPool, PartyPoolIndex)) return NULL;

    return (RTPartyRef)MemoryPoolReserveNext(Context->PartyPool, &PartyPoolIndex);
}
