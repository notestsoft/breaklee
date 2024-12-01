#pragma once

#include "Base.h"
#include "Socket.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

EXTERN_C_BEGIN

typedef struct _PacketField* PacketFieldRef;
typedef struct _PacketLayout* PacketLayoutRef;
typedef struct _PacketHandler* PacketHandlerRef;
typedef struct _PacketManager* PacketManagerRef;

PacketManagerRef PacketManagerCreate(
    AllocatorRef Allocator
);

Void PacketManagerDestroy(
    PacketManagerRef PacketManager
);

Void PacketManagerLoadScript(
    PacketManagerRef PacketManager,
    CString FilePath
);

PacketLayoutRef PacketManagerRegisterLayout(
    PacketManagerRef PacketManager,
    CString Name
);

PacketLayoutRef PacketManagerGetLayout(
    PacketManagerRef PacketManager,
    CString Name
);

Int32 PacketManagerHandle(
    PacketManagerRef PacketManager,
    SocketRef Socket,
    SocketConnectionRef SocketConnection,
    Int Command,
    UInt8* Buffer,
    Int32 Length
);

Void PacketLayoutAddInt8(
    PacketLayoutRef PacketLayout,
    CString Name
);

Void PacketLayoutAddInt16(
    PacketLayoutRef PacketLayout,
    CString Name
);

Void PacketLayoutAddInt32(
    PacketLayoutRef PacketLayout,
    CString Name
);

Void PacketLayoutAddInt64(
    PacketLayoutRef PacketLayout,
    CString Name
);

Void PacketLayoutAddUInt8(
    PacketLayoutRef PacketLayout,
    CString Name
);

Void PacketLayoutAddUInt16(
    PacketLayoutRef PacketLayout,
    CString Name
);

Void PacketLayoutAddUInt32(
    PacketLayoutRef PacketLayout,
    CString Name
);

Void PacketLayoutAddUInt64(
    PacketLayoutRef PacketLayout,
    CString Name
);

Void PacketLayoutAddCString(
    PacketLayoutRef PacketLayout,
    CString Name
);

Void PacketLayoutAddStaticCharacters(
    PacketLayoutRef PacketLayout,
    CString Name,
    Int32 Count
);

Void PacketLayoutAddDynamicCharacters(
    PacketLayoutRef PacketLayout,
    CString Name,
    CString CountName
);

Void PacketLayoutAddStaticArray(
    PacketLayoutRef PacketLayout,
    CString Name,
    CString ChildName,
    Int32 Count
);

Void PacketLayoutAddDynamicArray(
    PacketLayoutRef PacketLayout,
    CString Name,
    CString ChildName,
    CString CountName
);

EXTERN_C_END
