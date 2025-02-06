
#include <RuntimeLib/RuntimeLib.h>

#include "Constants.h"
#include "IPCProtocol.h"

#ifndef CLIENT_PROTOCOL_ENUM
#define CLIENT_PROTOCOL_ENUM(...)
#endif

#ifndef CLIENT_PROTOCOL_STRUCT
#define CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef CLIENT_PROTOCOL
#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __SIGNATURE__, __COMMAND__, __BODY__)
#endif

CLIENT_PROTOCOL(S2C, NFY_ERROR, DEFAULT, 7,
    UInt16 ErrorCommand;
    UInt16 ErrorSubCommand;
    UInt16 ErrorCode;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(C2S, CONNECT, DEFAULT, 401,
    UInt32 AuthKey;
)

CLIENT_PROTOCOL(S2C, CONNECT, DEFAULT, 401,
    UInt32 XorKey;
    UInt32 AuthKey;
    UInt16 ConnectionID;
    UInt16 XorKeyIndex;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(C2S, AUTH_ACCOUNT, DEFAULT, 402,
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(S2C, AUTH_ACCOUNT, DEFAULT, 402,
    UInt32 CharacterIndex;
    UInt32 GuildIndex;
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, LOGIN, DEFAULT, 403,
    UInt8 WorldServerIndex;
)

CLIENT_PROTOCOL(S2C, LOGIN, DEFAULT, 403,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, AUCTION_ITEM_SOLD, DEFAULT, 501,
    UInt8 Unknown1;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 ItemDuration;
    Int16 ItemCount;
    Int64 ItemPrice;
    UInt8 Unknown2[83];
)

#undef CLIENT_PROTOCOL_ENUM
#undef CLIENT_PROTOCOL_STRUCT
#undef CLIENT_PROTOCOL
