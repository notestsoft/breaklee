#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Timestamp RequestSyncTimestamp;

Void ServerSyncCharacter(
    ServerRef Server,
    ServerContextRef Context,
    ClientContextRef Client,
    RTCharacterRef Character
);

Void ServerSyncDB(
    ServerRef Server,
    ServerContextRef Context,
    Bool Force
);

ClientContextRef ServerGetClientByAuthKey(
    ServerContextRef Context,
    UInt32 AuthKey,
    UInt16 EntityID
);

ClientContextRef ServerGetClientByEntity(
    ServerContextRef Context,
    RTEntityID Entity
);

ClientContextRef ServerGetClientByIndex(
    ServerContextRef Context,
    UInt32 CharacterIndex,
    CString CharacterName
);

Void ServerRequestCountdownCharacter(
    ServerRef Server,
    ServerContextRef Context,
    RTRuntimeRef Runtime,
    ClientContextRef Client,
    SocketRef Socket,
    RTCharacterRef Character
);

Void ServerRequestCountdown(
    ServerRef Server,
    ServerContextRef Context,
    RTRuntimeRef Runtime
);

Bool ServerAttemptRollByChance(
    float rate
);

EXTERN_C_END
