#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

ClientContextRef ServerGetClientByIndex(
    ServerContextRef Context,
    UInt32 CharacterIndex,
    CString CharacterName,
    Int32 CharacterNameLength
);

RTPartyRef ServerCreateParty(
    ServerContextRef Context,
    Index CharacterIndex,
    RTEntityID CharacterID,
    Int32 PartyType
);

Void ServerDestroyParty(
    ServerContextRef Context,
    RTPartyRef Party
);

RTPartyRef ServerGetPartyByCharacter(
    ServerContextRef Context,
    Index CharacterIndex
);

RTPartyRef ServerGetParty(
    ServerContextRef Context,
    RTEntityID PartyID
);

EXTERN_C_END
