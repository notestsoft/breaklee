#pragma once

#include "Base.h"
#include "Party.h"

EXTERN_C_BEGIN

struct _RTPartyManager {
    AllocatorRef Allocator;
    MemoryPoolRef PartyPool;
    MemoryPoolRef PartyInvitationPool;
    MemoryPoolRef SoloPartyPool;
    DictionaryRef CharacterToPartyEntity;
    DictionaryRef CharacterToPartyInvite;
};

RTPartyManagerRef RTPartyManagerCreate(
    AllocatorRef Allocator,
    Int MaxPartyCount
);

Void RTPartyManagerDestroy(
    RTPartyManagerRef PartyManager
);

RTPartyRef RTPartyManagerCreateParty(
    RTPartyManagerRef PartyManager,
    RTPartyMemberInfoRef Member,
    Int32 PartyType
);

RTPartyRef RTPartyManagerCreatePartyRemote(
    RTPartyManagerRef PartyManager,
    RTPartyRef RemoteParty
);

Void RTPartyManagerDestroyParty(
    RTPartyManagerRef PartyManager,
    RTPartyRef Party
);

Void RTPartyManagerDestroyPartyRemote(
    RTPartyManagerRef PartyManager,
    RTPartyRef RemoteParty
);

RTPartyRef RTPartyManagerGetPartyByCharacter(
    RTPartyManagerRef PartyManager,
    UInt32 CharacterIndex
);

RTPartyRef RTPartyManagerGetParty(
    RTPartyManagerRef PartyManager,
    RTEntityID PartyID
);

Bool RTPartyManagerAddMember(
    RTPartyManagerRef PartyManager,
    RTPartyRef Party,
    RTPartyMemberInfoRef Member
);

Bool RTPartyManagerRemoveMember(
    RTPartyManagerRef PartyManager,
    UInt32 CharacterIndex
);

EXTERN_C_END
