#include "PacketLayout.h"

#define PACKET_MANAGER_GLOBAL_INSTANCE_NAME "_PacketManager"
#define PACKET_MANAGER_GLOBAL_SOCKET_NAME "_Socket"
#define PACKET_MANAGER_GLOBAL_SOCKET_CONNECTION_NAME "_SocketConnection"

enum {
    PACKET_FIELD_TYPE_INT8,
    PACKET_FIELD_TYPE_INT16,
    PACKET_FIELD_TYPE_INT32,
    PACKET_FIELD_TYPE_INT64,
    PACKET_FIELD_TYPE_UINT8,
    PACKET_FIELD_TYPE_UINT16,
    PACKET_FIELD_TYPE_UINT32,
    PACKET_FIELD_TYPE_UINT64,
    PACKET_FIELD_TYPE_STRING,
    PACKET_FIELD_TYPE_CHARACTERS,
    PACKET_FIELD_TYPE_STATIC_ARRAY,
    PACKET_FIELD_TYPE_DYNAMIC_ARRAY,
};

enum {
    PACKET_FIELD_SCRIPT_TYPE_INT8           = 0,
    PACKET_FIELD_SCRIPT_TYPE_INT16          = 1,
    PACKET_FIELD_SCRIPT_TYPE_INT32          = 2,
    PACKET_FIELD_SCRIPT_TYPE_INT64          = 3,
    PACKET_FIELD_SCRIPT_TYPE_UINT8          = 4,
    PACKET_FIELD_SCRIPT_TYPE_UINT16         = 5,
    PACKET_FIELD_SCRIPT_TYPE_UINT32         = 6,
    PACKET_FIELD_SCRIPT_TYPE_UINT64         = 7,
    PACKET_FIELD_SCRIPT_TYPE_STRING         = 8,
    PACKET_FIELD_SCRIPT_TYPE_CHARACTERS     = 9,
    PACKET_FIELD_SCRIPT_TYPE_ARRAY          = 10,
};

const CString kScriptPrelude =
    "function i8(name) return { 0, name } end\n"
    "function i16(name) return { 1, name } end\n"
    "function i32(name) return { 2, name } end\n"
    "function i64(name) return { 3, name } end\n"
    "function u8(name) return { 4, name } end\n"
    "function u16(name) return { 5, name } end\n"
    "function u32(name) return { 6, name } end\n"
    "function u64(name) return { 7, name } end\n"
    "function str(name) return { 8, name } end\n"
    "function chars(name, count) return { 9, name, count } end\n"
    "function vec(name, subtype, count) return { 10, name, subtype, count } end\n"
;

struct _PacketField {
    Int Index;
    Int Type;
    Int Length;
    Int ChildIndex;
    Int CountIndex;
    Int StackOffset;
    Int ArrayIndex;
    Char Name[MAX_PATH];
};

struct _PacketLayout {
    PacketManagerRef Manager;
    Int Index;
    Bool IsIntrinsic;
    Char Name[MAX_PATH];
    ArrayRef Fields;
    DictionaryRef NameToField;
};

struct _PacketHandler {
    Int Command;
    Int LayoutIndex;
    Int Handler;
    Char LayoutName[MAX_PATH];
};

struct _PacketManager {
    AllocatorRef Allocator;
    lua_State* State;
    ArrayRef PacketLayouts;
    DictionaryRef NameToPacketLayout;
    DictionaryRef CommandToPacketHandler;
};

Void PacketManagerRegisterScriptAPI(
    PacketManagerRef PacketManager
);

Void PacketLayoutWriteZero(
    PacketLayoutRef PacketLayout,
    PacketBufferRef PacketBuffer
);

Bool PacketLayoutParse(
    PacketLayoutRef PacketLayout,
    UInt8* Buffer,
    Int* OutOffset,
    Int Length,
    lua_State* State
);

Void PacketManagerRegisterIntrinsics(
    PacketManagerRef PacketManager
) {
    typedef Void(*Callback)(PacketLayoutRef, CString);

    struct { CString Name; Callback Callback; } Intrinsics[] = {
        { "i8", PacketLayoutAddInt8 },
        { "i16", PacketLayoutAddInt16 },
        { "i32", PacketLayoutAddInt32 },
        { "i64", PacketLayoutAddInt64 },
        { "u8", PacketLayoutAddUInt8 },
        { "u16", PacketLayoutAddUInt16 },
        { "u32", PacketLayoutAddUInt32 },
        { "u64", PacketLayoutAddUInt64 }
    };

    for (Int Index = 0; Index < sizeof(Intrinsics) / sizeof(Intrinsics[0]); Index++) {
        PacketLayoutRef PacketLayout = PacketManagerRegisterLayout(PacketManager, Intrinsics[Index].Name);
        PacketLayout->IsIntrinsic = true;
        Intrinsics[Index].Callback(PacketLayout, "$0");
    }
}

PacketManagerRef PacketManagerCreate(
    AllocatorRef Allocator
) {
    PacketManagerRef PacketManager = (PacketManagerRef)AllocatorAllocate(Allocator, sizeof(struct _PacketManager));
    if (!PacketManager) Fatal("Memory allocation failed!");
    
    PacketManager->Allocator = Allocator;
    PacketManager->State = luaL_newstate();
    if (!PacketManager->State) Fatal("Lua: State creation failed!");
    luaL_openlibs(PacketManager->State);
    lua_pushlightuserdata(PacketManager->State, PacketManager);
    lua_setglobal(PacketManager->State, PACKET_MANAGER_GLOBAL_INSTANCE_NAME);
    
    if (luaL_dostring(PacketManager->State, kScriptPrelude) != LUA_OK) {
        Fatal("Lua: Error %s", lua_tostring(PacketManager->State, -1));
    }
    
    PacketManager->PacketLayouts = ArrayCreateEmpty(Allocator, sizeof(struct _PacketLayout), 8);
    PacketManager->NameToPacketLayout = CStringDictionaryCreate(Allocator, 8);
    PacketManager->CommandToPacketHandler = IndexDictionaryCreate(Allocator, 8);
    PacketManagerRegisterIntrinsics(PacketManager);
    PacketManagerRegisterScriptAPI(PacketManager);
    return PacketManager;
}

Void PacketManagerDestroy(
    PacketManagerRef PacketManager
) {
    lua_close(PacketManager->State);

    for (Int Index = 0; Index < ArrayGetElementCount(PacketManager->PacketLayouts); Index += 1) {
        PacketLayoutRef PacketLayout = (PacketLayoutRef)ArrayGetElementAtIndex(PacketManager->PacketLayouts, Index);
        DictionaryDestroy(PacketLayout->NameToField);
        ArrayDestroy(PacketLayout->Fields);
    }
    
    DictionaryDestroy(PacketManager->CommandToPacketHandler);
    DictionaryDestroy(PacketManager->NameToPacketLayout);
    ArrayDestroy(PacketManager->PacketLayouts);
    AllocatorDeallocate(PacketManager->Allocator, PacketManager);
}

Void PacketManagerLoadScript(
    PacketManagerRef PacketManager,
    CString FilePath
) {
    if (luaL_loadfile(PacketManager->State, FilePath) != LUA_OK) Fatal("Lua: %s", lua_tostring(PacketManager->State, -1));
    if (lua_pcall(PacketManager->State, 0, 0, 0) != LUA_OK) Fatal("Lua: %s", lua_tostring(PacketManager->State, -1));
}

PacketLayoutRef PacketManagerRegisterLayout(
    PacketManagerRef PacketManager,
    CString Name
) {
    if (DictionaryLookup(PacketManager->NameToPacketLayout, Name)) Fatal("Packet layout with name '%s' already registered!", Name);
    
    Int PacketLayoutIndex = ArrayGetElementCount(PacketManager->PacketLayouts);
    PacketLayoutRef PacketLayout = (PacketLayoutRef)ArrayAppendUninitializedElement(PacketManager->PacketLayouts);
    memset(PacketLayout, 0, sizeof(struct _PacketLayout));
    PacketLayout->Manager = PacketManager;
    PacketLayout->Index = PacketLayoutIndex;
    strcpy(PacketLayout->Name, Name);
    PacketLayout->Fields = ArrayCreateEmpty(PacketManager->Allocator, sizeof(struct _PacketField), 8);
    PacketLayout->NameToField = CStringDictionaryCreate(PacketManager->Allocator, 8);
    
    DictionaryInsert(PacketManager->NameToPacketLayout, Name, &PacketLayoutIndex, sizeof(Int));
    
    return PacketLayout;
}

Int PacketManagerGetLayoutIndex(
    PacketManagerRef PacketManager,
    CString Name
) {
    Int* PacketLayoutIndex = DictionaryLookup(PacketManager->NameToPacketLayout, Name);
    if (!PacketLayoutIndex) return UINT64_MAX;

    return *PacketLayoutIndex;
}

PacketLayoutRef PacketManagerGetLayoutByIndex(
    PacketManagerRef PacketManager,
    Int Index
) {
    assert(Index < ArrayGetElementCount(PacketManager->PacketLayouts));
    return (PacketLayoutRef)ArrayGetElementAtIndex(PacketManager->PacketLayouts, Index);
}

PacketLayoutRef PacketManagerGetLayout(
    PacketManagerRef PacketManager,
    CString Name
) {
    Int Index = PacketManagerGetLayoutIndex(PacketManager, Name);
    if (Index == UINT64_MAX) return NULL;

    return PacketManagerGetLayoutByIndex(PacketManager, Index);
}

Int32 PacketManagerHandle(
    PacketManagerRef PacketManager,
    SocketRef Socket,
    SocketConnectionRef SocketConnection,
    Int Command,
    UInt8* Buffer,
    Int32 Length
) {
    PacketHandlerRef PacketHandler = DictionaryLookup(PacketManager->CommandToPacketHandler, &Command);
    if (!PacketHandler) return 0;
    
    PacketLayoutRef PacketLayout = PacketManagerGetLayoutByIndex(PacketManager, PacketHandler->LayoutIndex);
    if (!PacketLayout) return 0;
    
    Int StateStack = lua_gettop(PacketManager->State);

    lua_pushlightuserdata(PacketManager->State, Socket);
    lua_setglobal(PacketManager->State, PACKET_MANAGER_GLOBAL_SOCKET_NAME);

    lua_pushlightuserdata(PacketManager->State, SocketConnection);
    lua_setglobal(PacketManager->State, PACKET_MANAGER_GLOBAL_SOCKET_CONNECTION_NAME);

    lua_rawgeti(PacketManager->State, LUA_REGISTRYINDEX, PacketHandler->Handler);

    Int Offset = 0;
    Bool Success = PacketLayoutParse(PacketLayout, Buffer, &Offset, Length, PacketManager->State);
    if (!Success) {
        lua_settop(PacketManager->State, StateStack);
        return -1;
    }

    Int ArgumentCount = 1;
    Int ReturnValueCount = 0;
    Int Result = lua_pcall(PacketManager->State, ArgumentCount, ReturnValueCount, 0);
    if (Result != LUA_OK) {
        CString Message = (CString)lua_tostring(PacketManager->State, -1);
        Error("Lua error: %s", Message);
        lua_pop(PacketManager->State, 1);
        lua_settop(PacketManager->State, StateStack);
        return -1;
    }
    
    lua_settop(PacketManager->State, StateStack);
    return 1;
}

Int PacketLayoutGetSize(
    PacketLayoutRef PacketLayout
);

Int PacketFieldGetSize(
    PacketLayoutRef PacketLayout,
    PacketFieldRef PacketField
) {
    switch (PacketField->Type) {
    case PACKET_FIELD_TYPE_INT8:
    case PACKET_FIELD_TYPE_INT16:
    case PACKET_FIELD_TYPE_INT32:
    case PACKET_FIELD_TYPE_INT64:
    case PACKET_FIELD_TYPE_UINT8:
    case PACKET_FIELD_TYPE_UINT16:
    case PACKET_FIELD_TYPE_UINT32:
    case PACKET_FIELD_TYPE_UINT64:
    case PACKET_FIELD_TYPE_STRING:
    case PACKET_FIELD_TYPE_CHARACTERS:
        return PacketField->Length;
 
    case PACKET_FIELD_TYPE_STATIC_ARRAY: {
        PacketLayoutRef Child = PacketManagerGetLayoutByIndex(PacketLayout->Manager, PacketField->ChildIndex);
        return PacketField->Length * PacketLayoutGetSize(Child);
    }
    
    case PACKET_FIELD_TYPE_DYNAMIC_ARRAY:
        return 0;

    default:
        UNREACHABLE("Unexpected type for packet field!");
    }
}

Int PacketLayoutGetSize(
    PacketLayoutRef PacketLayout
) {
    Int Size = 0;
    
    for (Int Index = 0; Index < ArrayGetElementCount(PacketLayout->Fields); Index += 1) {
        PacketFieldRef PacketField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, Index);
        Size += PacketFieldGetSize(PacketLayout, PacketField);
    }
    
    return Size;
}

PacketFieldRef PacketLayoutAddField(
    PacketLayoutRef PacketLayout,
    CString Name,
    Int Type,
    Int Length,
    Int ChildIndex,
    Int64 CountIndex
) {
    if (DictionaryLookup(PacketLayout->NameToField, Name)) {
        Fatal("Packet field with name '%s' already registered!", Name);
        return NULL;
    }
    
    Int PacketFieldIndex = ArrayGetElementCount(PacketLayout->Fields);
    PacketFieldRef PacketField = (PacketFieldRef)ArrayAppendUninitializedElement(PacketLayout->Fields);
    memset(PacketField, 0, sizeof(struct _PacketField));
    PacketField->Index = PacketFieldIndex;
    PacketField->Type = Type;
    PacketField->Length = Length;
    PacketField->ChildIndex = ChildIndex;
    PacketField->CountIndex = CountIndex;
    strcpy(PacketField->Name, Name);
    PacketField->StackOffset = 0;
    PacketField->ArrayIndex = -1;

    DictionaryInsert(PacketLayout->NameToField, Name, &PacketFieldIndex, sizeof(Int));
    return PacketField;
}

PacketFieldRef PacketLayoutGetField(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    return (PacketFieldRef)DictionaryLookup(PacketLayout->NameToField, Name);
}

Void PacketLayoutAddInt8(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_INT8, sizeof(Int8), 0, -1);
}

Void PacketLayoutAddInt16(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_INT16, sizeof(Int16), 0, -1);
}

Void PacketLayoutAddInt32(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_INT32, sizeof(Int32), 0, -1);
}

Void PacketLayoutAddInt64(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_INT64, sizeof(Int64), 0, -1);
}

Void PacketLayoutAddUInt8(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_UINT8, sizeof(UInt8), 0, -1);
}

Void PacketLayoutAddUInt16(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_UINT16, sizeof(UInt16), 0, -1);
}

Void PacketLayoutAddUInt32(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_UINT32, sizeof(UInt32), 0, -1);
}

Void PacketLayoutAddUInt64(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_UINT64, sizeof(UInt64), 0, -1);
}

Void PacketLayoutAddCString(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_STRING, sizeof(Char), 0, -1);
}

Void PacketLayoutAddStaticCharacters(
    PacketLayoutRef PacketLayout,
    CString Name,
    Int32 Count
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_CHARACTERS, sizeof(Char) * Count, 0, -1);
}

Void PacketLayoutAddDynamicCharacters(
    PacketLayoutRef PacketLayout,
    CString Name,
    CString CountName
) {
    Int* CountIndex = DictionaryLookup(PacketLayout->NameToField, CountName);
    if (!CountIndex) Fatal("Packet field named '%s' not found!", CountName);

    PacketFieldRef CountField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, *CountIndex);
    Bool CountFieldIsPrimitive = (
        CountField->Type == PACKET_FIELD_TYPE_INT8 ||
        CountField->Type == PACKET_FIELD_TYPE_INT16 ||
        CountField->Type == PACKET_FIELD_TYPE_INT32 ||
        CountField->Type == PACKET_FIELD_TYPE_INT64 ||
        CountField->Type == PACKET_FIELD_TYPE_UINT8 ||
        CountField->Type == PACKET_FIELD_TYPE_UINT16 ||
        CountField->Type == PACKET_FIELD_TYPE_UINT32 ||
        CountField->Type == PACKET_FIELD_TYPE_UINT64
    );

    if (!CountFieldIsPrimitive) Fatal("Packet field named '%s' uses non-primitive count '%s'!", Name, CountName);

    PacketFieldRef PacketField = PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_CHARACTERS, 0, 0, *CountIndex);
    CountField->ArrayIndex = PacketField->Index;
}

Void PacketLayoutAddStaticArray(
    PacketLayoutRef PacketLayout,
    CString Name,
    CString ChildName,
    Int32 Count
) {
    Int ChildIndex = PacketManagerGetLayoutIndex(PacketLayout->Manager, ChildName);
    if (ChildIndex == UINT64_MAX) Fatal("Packet layout named '%s' not found!", ChildName);

    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_STATIC_ARRAY, Count, ChildIndex, 0);
}

Void PacketLayoutAddDynamicArray(
    PacketLayoutRef PacketLayout,
    CString Name,
    CString ChildName,
    CString CountName
) {
    Int ChildIndex = PacketManagerGetLayoutIndex(PacketLayout->Manager, ChildName);
    if (ChildIndex == UINT64_MAX) Fatal("Packet layout named '%s' not found!", ChildName);

    Int* CountIndex = DictionaryLookup(PacketLayout->NameToField, CountName);
    if (!CountIndex) Fatal("Packet field named '%s' not found!", ChildName);
    
    PacketFieldRef CountField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, *CountIndex);
    Bool CountFieldIsPrimitive = (
        CountField->Type == PACKET_FIELD_TYPE_INT8 ||
        CountField->Type == PACKET_FIELD_TYPE_INT16 ||
        CountField->Type == PACKET_FIELD_TYPE_INT32 ||
        CountField->Type == PACKET_FIELD_TYPE_INT64 ||
        CountField->Type == PACKET_FIELD_TYPE_UINT8 ||
        CountField->Type == PACKET_FIELD_TYPE_UINT16 ||
        CountField->Type == PACKET_FIELD_TYPE_UINT32 ||
        CountField->Type == PACKET_FIELD_TYPE_UINT64
    );
    
    if (!CountFieldIsPrimitive) Fatal("Packet field named '%s' uses non-primitive count '%s'!", ChildName, CountName);

    PacketFieldRef PacketField = PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_DYNAMIC_ARRAY, 0, ChildIndex, *CountIndex);
    CountField->ArrayIndex = PacketField->Index;
}

lua_Integer PacketFieldReadPrimitive(
    PacketFieldRef PacketField,
    UInt8* Buffer
) {
    lua_Integer Result = 0;
    if (PacketField->Type == PACKET_FIELD_TYPE_INT8) {
        Result = *((Int8*)&Buffer[PacketField->StackOffset]);
    }
    
    if (PacketField->Type == PACKET_FIELD_TYPE_INT16) {
        Result = *((Int16*)&Buffer[PacketField->StackOffset]);
    }
    
    if (PacketField->Type == PACKET_FIELD_TYPE_INT32) {
        Result = *((Int32*)&Buffer[PacketField->StackOffset]);
    }
    
    if (PacketField->Type == PACKET_FIELD_TYPE_INT64) {
        Result = *((Int64*)&Buffer[PacketField->StackOffset]);
    }
    
    if (PacketField->Type == PACKET_FIELD_TYPE_UINT8) {
        Result = *((UInt8*)&Buffer[PacketField->StackOffset]);
    }
    
    if (PacketField->Type == PACKET_FIELD_TYPE_UINT16) {
        Result = *((UInt16*)&Buffer[PacketField->StackOffset]);
    }
    
    if (PacketField->Type == PACKET_FIELD_TYPE_UINT32) {
        Result = *((UInt32*)&Buffer[PacketField->StackOffset]);
    }
    
    if (PacketField->Type == PACKET_FIELD_TYPE_UINT64) {
        Result = *((UInt64*)&Buffer[PacketField->StackOffset]);
    }
    
    return Result;
}

Void PacketFieldWritePrimitive(
    PacketFieldRef PacketField,
    PacketBufferRef PacketBuffer,
    lua_Integer Value
) {
    if (PacketField->Type == PACKET_FIELD_TYPE_INT8) {
        PacketBufferAppendValue(PacketBuffer, Int8, (Int8)Value);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_INT16) {
        PacketBufferAppendValue(PacketBuffer, Int16, (Int16)Value);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_INT32) {
        PacketBufferAppendValue(PacketBuffer, Int32, (Int32)Value);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_INT64) {
        PacketBufferAppendValue(PacketBuffer, Int64, (Int64)Value);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_UINT8) {
        PacketBufferAppendValue(PacketBuffer, UInt8, (UInt8)Value);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_UINT16) {
        PacketBufferAppendValue(PacketBuffer, UInt16, (UInt16)Value);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_UINT32) {
        PacketBufferAppendValue(PacketBuffer, UInt32, (UInt32)Value);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_UINT64) {
        PacketBufferAppendValue(PacketBuffer, UInt64, (UInt64)Value);
    }
}

Void PacketFieldWriteZero(
    PacketLayoutRef PacketLayout,
    PacketFieldRef PacketField,
    PacketBufferRef PacketBuffer
) {
    if (PacketField->Type == PACKET_FIELD_TYPE_INT8) {
        PacketBufferAppendValue(PacketBuffer, Int8, 0);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_INT16) {
        PacketBufferAppendValue(PacketBuffer, Int16, 0);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_INT32) {
        PacketBufferAppendValue(PacketBuffer, Int32, 0);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_INT64) {
        PacketBufferAppendValue(PacketBuffer, Int64, 0);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_UINT8) {
        PacketBufferAppendValue(PacketBuffer, UInt8, 0);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_UINT16) {
        PacketBufferAppendValue(PacketBuffer, UInt16, 0);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_UINT32) {
        PacketBufferAppendValue(PacketBuffer, UInt32, 0);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_UINT64) {
        PacketBufferAppendValue(PacketBuffer, UInt64, 0);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_STRING) {
        PacketBufferAppendValue(PacketBuffer, Char, 0);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_CHARACTERS) {
        PacketBufferAppend(PacketBuffer, PacketField->Length);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_STATIC_ARRAY) {
        PacketLayoutRef ChildLayout = (PacketLayoutRef)ArrayGetElementAtIndex(PacketLayout->Fields, PacketField->ChildIndex);
        Int ChildSize = PacketLayoutGetSize(ChildLayout);
        PacketBufferAppend(PacketBuffer, PacketField->Length * ChildSize);
    }

    if (PacketField->Type == PACKET_FIELD_TYPE_DYNAMIC_ARRAY) {
    }
}

Void PacketLayoutWriteZero(
    PacketLayoutRef PacketLayout,
    PacketBufferRef PacketBuffer
) {
    for (Int Index = 0; Index < ArrayGetElementCount(PacketLayout->Fields); Index += 1) {
        PacketFieldRef PacketField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, Index);
        PacketFieldWriteZero(PacketLayout, PacketField, PacketBuffer);
    }
}

Bool PacketLayoutParse(
    PacketLayoutRef PacketLayout,
    UInt8* Buffer,
    Int* OutOffset,
    Int Length,
    lua_State* State
) {
    lua_newtable(State);

    for (Int Index = 0; Index < ArrayGetElementCount(PacketLayout->Fields); Index += 1) {
        PacketFieldRef PacketField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, Index);
        PacketField->StackOffset = *OutOffset;
        
        Int32 FieldLength = (Int32)PacketField->Length;
        if (PacketField->StackOffset + FieldLength > Length) {
            Error("Received misaligned packet for layout '%s'", PacketLayout->Name);
            return false;
        }
        
        lua_pushstring(State, PacketField->Name);

        if (
            PacketField->Type == PACKET_FIELD_TYPE_INT8 ||
            PacketField->Type == PACKET_FIELD_TYPE_INT16 ||
            PacketField->Type == PACKET_FIELD_TYPE_INT32 ||
            PacketField->Type == PACKET_FIELD_TYPE_INT64 ||
            PacketField->Type == PACKET_FIELD_TYPE_UINT8 ||
            PacketField->Type == PACKET_FIELD_TYPE_UINT16 ||
            PacketField->Type == PACKET_FIELD_TYPE_UINT32 ||
            PacketField->Type == PACKET_FIELD_TYPE_UINT64
        ) {
            lua_Integer Value = PacketFieldReadPrimitive(PacketField, Buffer);
            lua_pushinteger(State, (lua_Integer)Value);
        }
        
        if (PacketField->Type == PACKET_FIELD_TYPE_STRING) {
            Char* Value = (Char*)&Buffer[PacketField->StackOffset];
            FieldLength = (Int32)strlen(Value);
            if (PacketField->StackOffset + FieldLength > Length) {
                Error("Received misaligned packet for layout '%s'", PacketLayout->Name);
                return false;
            }

            lua_pushstring(State, Value);
        }
        
        if (PacketField->Type == PACKET_FIELD_TYPE_CHARACTERS) {
            // TODO: We should do a memcpy here and append a trailing zero manually based on the length
            Char* Value = ((Char*)&Buffer[PacketField->StackOffset]);
            lua_pushstring(State, Value);

            if (PacketField->CountIndex >= 0) {
                PacketField->Length = strlen(Value);

                PacketFieldRef CountField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, PacketField->CountIndex);

                lua_settable(State, -3);
                lua_pushstring(State, CountField->Name);
                assert(
                    PacketField->Type == PACKET_FIELD_TYPE_INT8 ||
                    PacketField->Type == PACKET_FIELD_TYPE_INT16 ||
                    PacketField->Type == PACKET_FIELD_TYPE_INT32 ||
                    PacketField->Type == PACKET_FIELD_TYPE_INT64 ||
                    PacketField->Type == PACKET_FIELD_TYPE_UINT8 ||
                    PacketField->Type == PACKET_FIELD_TYPE_UINT16 ||
                    PacketField->Type == PACKET_FIELD_TYPE_UINT32 ||
                    PacketField->Type == PACKET_FIELD_TYPE_UINT64
                );
                lua_pushinteger(State, (lua_Integer)PacketField->Length);
            }
        }
        
        if (PacketField->Type == PACKET_FIELD_TYPE_STATIC_ARRAY) {
            PacketLayoutRef Child = PacketManagerGetLayoutByIndex(PacketLayout->Manager, PacketField->ChildIndex);
           
            lua_createtable(State, (Int32)PacketField->Length, 0);
            for (Int ChildIndex = 0; ChildIndex < PacketField->Length; ChildIndex += 1) {
                lua_pushinteger(State, (lua_Integer)ChildIndex + 1);
                if (!PacketLayoutParse(Child, Buffer, OutOffset, Length, State)) return false;

                lua_settable(State, -3);
            }
            
            FieldLength = 0;
        }
        
        if (PacketField->Type == PACKET_FIELD_TYPE_DYNAMIC_ARRAY) {
            // TODO: Add support for automatic counting!
            PacketLayoutRef Child = PacketManagerGetLayoutByIndex(PacketLayout->Manager, PacketField->ChildIndex);
            PacketFieldRef CountField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, PacketField->CountIndex);
            Int32 Count = (Int32)PacketFieldReadPrimitive(CountField, Buffer);
            
            lua_createtable(State, Count, 0);
            for (Int ChildIndex = 0; ChildIndex < Count; ChildIndex += 1) {
                lua_pushinteger(State, (lua_Integer)ChildIndex + 1);
                if (!PacketLayoutParse(Child, Buffer, OutOffset, Length, State)) return false;

                lua_settable(State, -3);
            }
            
            FieldLength = 0;
        }
        
        lua_settable(State, -3);
        *OutOffset += FieldLength;
    }
    
    return true;
}

Bool PacketLayoutEncode(
    PacketLayoutRef PacketLayout,
    PacketBufferRef PacketBuffer,
    lua_State* State
) {
    if (PacketLayout->IsIntrinsic) {
        assert(ArrayGetElementCount(PacketLayout->Fields) == 1);

        PacketFieldRef PacketField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, 0);
        assert(CStringIsEqual(PacketField->Name, "$0"));

        if (!lua_isinteger(State, -1)) {
            Error("Invalid type '%s' given for field '%s' in layout '%s'", lua_typename(State, -1), PacketField->Name, PacketLayout->Name);
            lua_pop(State, 1);
            return false;
        }

        lua_Integer Value = lua_tointeger(State, -1);
        PacketFieldWritePrimitive(PacketField, PacketBuffer, Value);
        lua_pop(State, 1);
        return true;
    }

    if (!lua_istable(State, -1)) {
        return false;
    }

    for (Int FieldIndex = 0; FieldIndex < ArrayGetElementCount(PacketLayout->Fields); FieldIndex += 1) {
        PacketFieldRef PacketField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, FieldIndex);

        if (PacketField->ArrayIndex >= 0) {
            PacketFieldRef ArrayField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, PacketField->ArrayIndex);
            
            lua_pushstring(State, ArrayField->Name);
            lua_gettable(State, -2);
            if (ArrayField->Type == PACKET_FIELD_TYPE_DYNAMIC_ARRAY && lua_istable(State, -1)) {
                PacketFieldWritePrimitive(PacketField, PacketBuffer, luaL_len(State, -1));
            }
            else if (ArrayField->Type == PACKET_FIELD_TYPE_CHARACTERS && lua_isstring(State, -1)) {
                CString Value = (CString)lua_tostring(State, -1);
                ArrayField->Length = strlen(Value);
                PacketFieldWritePrimitive(PacketField, PacketBuffer, ArrayField->Length);
            }
            else {
                PacketFieldWritePrimitive(PacketField, PacketBuffer, 0);
            }

            lua_pop(State, 1);
            continue;
        }

        lua_pushstring(State, PacketField->Name);
        lua_gettable(State, -2);

        if (lua_isnil(State, -1)) {
            PacketFieldWriteZero(PacketLayout, PacketField, PacketBuffer);
            lua_pop(State, 1);
            continue;
        }

        if (
            PacketField->Type == PACKET_FIELD_TYPE_INT8 ||
            PacketField->Type == PACKET_FIELD_TYPE_INT16 ||
            PacketField->Type == PACKET_FIELD_TYPE_INT32 ||
            PacketField->Type == PACKET_FIELD_TYPE_INT64 ||
            PacketField->Type == PACKET_FIELD_TYPE_UINT8 ||
            PacketField->Type == PACKET_FIELD_TYPE_UINT16 ||
            PacketField->Type == PACKET_FIELD_TYPE_UINT32 ||
            PacketField->Type == PACKET_FIELD_TYPE_UINT64
        ) {
            if (!lua_isinteger(State, -1)) {
                Error("Invalid type '%s' given for field '%s' in layout '%s'", lua_typename(State, -1), PacketField->Name, PacketLayout->Name);
                lua_pop(State, 1);
                return false;
            }

            lua_Integer Value = lua_tointeger(State, -1);
            PacketFieldWritePrimitive(PacketField, PacketBuffer, Value);
            lua_pop(State, 1);
            continue;
        }

        if (PacketField->Type == PACKET_FIELD_TYPE_STRING) {
            if (!lua_isstring(State, -1)) {
                lua_pop(State, 1);
                Error("Invalid type given for field '%s' in layout '%s'", PacketField->Name, PacketLayout->Name);
                return false;
            }

            CString Value = (CString)lua_tostring(State, -1);
            PacketBufferAppendCString(PacketBuffer, Value);
            lua_pop(State, 1);
            continue;
        }

        if (PacketField->Type == PACKET_FIELD_TYPE_CHARACTERS) {
            if (!lua_isstring(State, -1)) {
                lua_pop(State, 1);
                Error("Invalid type given for field '%s' in layout '%s'", PacketField->Name, PacketLayout->Name);
                return false;
            }

            CString Value = (CString)lua_tostring(State, -1);
            CString Memory = (CString)PacketBufferAppend(PacketBuffer, PacketField->Length);
            memcpy(Memory, Value, MIN(PacketField->Length, strlen(Value)));
            lua_pop(State, 1);
            continue;
        }

        if (PacketField->Type == PACKET_FIELD_TYPE_STATIC_ARRAY) {
            if (!lua_istable(State, -1)) {
                lua_pop(State, 1);
                Error("Invalid type given for field '%s' in layout '%s'", PacketField->Name, PacketLayout->Name);
                return false;
            }

            PacketLayoutRef ChildLayout = PacketManagerGetLayoutByIndex(PacketLayout->Manager, PacketField->ChildIndex);
            Int Count = PacketField->Length;

            lua_pushnil(State);

            Int Offset = 0;
            while (lua_next(State, -2) != 0) {
                if (Offset >= PacketField->Length) {
                    lua_pop(State, 2);
                    Warn("Invalid count of elements given for field '%s' in layout '%s'", PacketField->Name, PacketLayout->Name);
                    break;
                }

                if (!PacketLayoutEncode(ChildLayout, PacketBuffer, State)) {
                    lua_pop(State, 2);
                    return false;
                }

                Offset += 1;
            }

            for (Int Index = Offset; Index < Count; Index += 1) {
                PacketLayoutWriteZero(ChildLayout, PacketBuffer);
            }

            lua_pop(State, 1);
            continue;
        }

        if (PacketField->Type == PACKET_FIELD_TYPE_DYNAMIC_ARRAY) {
            if (!lua_istable(State, -1)) {
                lua_pop(State, 1);
                Error("Invalid type given for field '%s' in layout '%s'", PacketField->Name, PacketLayout->Name);
                return false;
            }

            PacketLayoutRef ChildLayout = PacketManagerGetLayoutByIndex(PacketLayout->Manager, PacketField->ChildIndex);
            lua_Integer Count = luaL_len(State, -1);

            lua_pushnil(State);

            Int Offset = 0;
            while (lua_next(State, -2) != 0) {
                if (!PacketLayoutEncode(ChildLayout, PacketBuffer, State)) {
                    lua_pop(State, 2);
                    return false;
                }

                Offset += 1;
            }

            lua_pop(State, 1);
            continue;
        }

        lua_pop(State, 1);
    }

    lua_pop(State, 1);
    return true;
}

static Int32 PacketManagerAPI_RegisterPacketLayout(
    lua_State* State
) {
    Int32 StateStack = lua_gettop(State);

    Int32 Result = lua_getglobal(State, PACKET_MANAGER_GLOBAL_INSTANCE_NAME);
    if (Result != LUA_TLIGHTUSERDATA) {
        lua_pop(State, 1);
        lua_settop(State, StateStack);
        return luaL_error(State, "Corrupted stack!");
    }
        
    PacketManagerRef PacketManager = lua_touserdata(State, -1);
    if (!PacketManager) {
        lua_pop(State, 1);
        lua_settop(State, StateStack);
        return luaL_error(State, "Corrupted stack!");
    }
    lua_pop(State, 1);
    
    CString Name = (CString)lua_tostring(State, 1);
    PacketLayoutRef PacketLayout = PacketManagerRegisterLayout(PacketManager, Name);
    if (!PacketLayout) {
        lua_pop(State, 1);
        lua_settop(State, StateStack);
        return luaL_error(State, "Packet layout registration failed '%s'", Name);
    }
    
    if (!lua_istable(State, 2)) {
        return luaL_error(State, "Invalid argument given for RegisterPacketLayout!");
    }
    
    lua_pushnil(State);
    while (lua_next(State, -2) != 0) {
        if (lua_istable(State, -1)) {
            lua_pushnil(State);

            lua_next(State, -2);
            Int32 FieldType = (Int32)lua_tointeger(State, -1);
            lua_pop(State, 1);

            lua_next(State, -2);
            CString FieldName = (CString)lua_tostring(State, -1);
            lua_pop(State, 1);

            switch (FieldType) {
                case PACKET_FIELD_SCRIPT_TYPE_INT8:
                    PacketLayoutAddInt8(PacketLayout, FieldName);
                    break;
                    
                case PACKET_FIELD_SCRIPT_TYPE_INT16:
                    PacketLayoutAddInt16(PacketLayout, FieldName);
                    break;
                    
                case PACKET_FIELD_SCRIPT_TYPE_INT32:
                    PacketLayoutAddInt32(PacketLayout, FieldName);
                    break;
                    
                case PACKET_FIELD_SCRIPT_TYPE_INT64:
                    PacketLayoutAddInt64(PacketLayout, FieldName);
                    break;
                    
                case PACKET_FIELD_SCRIPT_TYPE_UINT8:
                    PacketLayoutAddUInt8(PacketLayout, FieldName);
                    break;
                    
                case PACKET_FIELD_SCRIPT_TYPE_UINT16:
                    PacketLayoutAddUInt16(PacketLayout, FieldName);
                    break;
                    
                case PACKET_FIELD_SCRIPT_TYPE_UINT32:
                    PacketLayoutAddUInt32(PacketLayout, FieldName);
                    break;
                    
                case PACKET_FIELD_SCRIPT_TYPE_UINT64:
                    PacketLayoutAddUInt64(PacketLayout, FieldName);
                    break;
                    
                case PACKET_FIELD_SCRIPT_TYPE_STRING:
                    PacketLayoutAddCString(PacketLayout, FieldName);
                    break;
                    
                case PACKET_FIELD_SCRIPT_TYPE_CHARACTERS: {
                    lua_next(State, -2);
                    if (lua_isstring(State, -1)) {
                        CString CountName = (CString)lua_tostring(State, -1);
                        lua_pop(State, 1);

                        PacketLayoutAddDynamicCharacters(PacketLayout, FieldName, CountName);
                    }
                    else if (lua_isinteger(State, -1)) {
                        Int32 Count = (Int32)lua_tointeger(State, -1);
                        lua_pop(State, 1);

                        PacketLayoutAddStaticCharacters(PacketLayout, FieldName, Count);
                    }
                    else {
                        lua_pop(State, 1);
                        return luaL_error(State, "Invalid field value given!");
                    }

                    break;
                }
                
                case PACKET_FIELD_SCRIPT_TYPE_ARRAY: {
                    lua_next(State, -2);
                    CString ChildName = (CString)lua_tostring(State, -1);
                    lua_pop(State, 1);
                    
                    lua_next(State, -2);
                    if (lua_isinteger(State, -1)) {
                        Int32 Count = (Int32)lua_tointeger(State, -1);
                        lua_pop(State, 1);

                        PacketLayoutAddStaticArray(PacketLayout, FieldName, ChildName, Count);
                    } else {
                        CString CountName = (CString)lua_tostring(State, -1);
                        lua_pop(State, 1);

                        PacketLayoutAddDynamicArray(PacketLayout, FieldName, ChildName, CountName);
                    }

                    break;
                }
                
                default:
                    return luaL_error(State, "Invalid field type given!");
            }

            lua_pop(State, 1);
        }

        lua_pop(State, 1);
    }

    return 0;
}

PacketManagerRef PacketManagerStateGetGlobalInstance(
    lua_State* State
) {
    Int32 StateStack = lua_gettop(State);
    Int32 Result = lua_getglobal(State, PACKET_MANAGER_GLOBAL_INSTANCE_NAME);
    if (Result != LUA_TLIGHTUSERDATA) {
        lua_pop(State, 1);
        lua_settop(State, StateStack);
        luaL_error(State, "Corrupted stack!");
        return NULL;
    }

    PacketManagerRef PacketManager = lua_touserdata(State, -1);
    if (!PacketManager) {
        lua_pop(State, 1);
        lua_settop(State, StateStack);
        luaL_error(State, "Corrupted stack!");
        return NULL;
    }
    lua_pop(State, 1);

    return PacketManager;
}

SocketRef PacketManagerStateGetGlobalSocket(
    lua_State* State
) {
    Int32 StateStack = lua_gettop(State);
    Int32 Result = lua_getglobal(State, PACKET_MANAGER_GLOBAL_SOCKET_NAME);
    if (Result != LUA_TLIGHTUSERDATA) {
        lua_pop(State, 1);
        lua_settop(State, StateStack);
        luaL_error(State, "Corrupted stack!");
        return NULL;
    }

    SocketRef Socket = lua_touserdata(State, -1);
    if (!Socket) {
        lua_pop(State, 1);
        lua_settop(State, StateStack);
        luaL_error(State, "Corrupted stack!");
        return NULL;
    }
    lua_pop(State, 1);

    return Socket;
}

SocketConnectionRef PacketManagerStateGetGlobalSocketConnection(
    lua_State* State
) {
    Int32 StateStack = lua_gettop(State);
    Int32 Result = lua_getglobal(State, PACKET_MANAGER_GLOBAL_SOCKET_CONNECTION_NAME);
    if (Result != LUA_TLIGHTUSERDATA) {
        lua_pop(State, 1);
        lua_settop(State, StateStack);
        luaL_error(State, "Corrupted stack!");
        return NULL;
    }

    SocketConnectionRef SocketConnection = lua_touserdata(State, -1);
    if (!SocketConnection) {
        lua_pop(State, 1);
        lua_settop(State, StateStack);
        luaL_error(State, "Corrupted stack!");
        return NULL;
    }
    lua_pop(State, 1);

    return SocketConnection;
}

static Int32 PacketManagerAPI_RegisterPacketHandler(
    lua_State* State
) {
    Int32 StateStack = lua_gettop(State);

    PacketManagerRef PacketManager = PacketManagerStateGetGlobalInstance(State);
    if (!PacketManager) return 0;

    if (!lua_isinteger(State, 1)) return luaL_error(State, "Invalid argument for command!");
    Int Command = (Int)lua_tointeger(State, 1);

    if (!lua_isstring(State, 2)) return luaL_error(State, "Invalid argument for handler!");
    CString Name = (CString)lua_tostring(State, 2);

    if (!lua_isfunction(State, 3)) return luaL_error(State, "Invalid argument for handler function!");
    lua_pushvalue(State, 3);
    Int32 Handler = luaL_ref(State, LUA_REGISTRYINDEX);

    if (DictionaryLookup(PacketManager->CommandToPacketHandler, &Command)) {
        return luaL_error(State, "Handler for command %d already registered!", Command);
    }
    
    Int LayoutIndex = PacketManagerGetLayoutIndex(PacketManager, Name);
    if (LayoutIndex == UINT64_MAX) {
        return luaL_error(State, "Layout named %s not found!", Name);
    }

    struct _PacketHandler PacketHandler = { 0 };
    PacketHandler.Command = Command;
    PacketHandler.LayoutIndex = PacketManagerGetLayoutIndex(PacketManager, Name);
    PacketHandler.Handler = Handler;
    strcpy(PacketHandler.LayoutName, Name);

    DictionaryInsert(PacketManager->CommandToPacketHandler, &Command, &PacketHandler, sizeof(struct _PacketHandler));

    lua_settop(State, StateStack);
    return 0;
}

static Int32 PacketManagerAPI_Unicast(
    lua_State* State
) {
    Int32 StateStack = lua_gettop(State);

    PacketManagerRef PacketManager = PacketManagerStateGetGlobalInstance(State);
    if (!PacketManager) return 0;

    SocketRef Socket = PacketManagerStateGetGlobalSocket(State);
    if (!Socket) return 0;

    SocketConnectionRef SocketConnection = PacketManagerStateGetGlobalSocketConnection(State);
    if (!SocketConnection) return 0;

    if (!lua_isinteger(State, 1)) return luaL_error(State, "Invalid argument for command!");
    Int Command = (Int)lua_tointeger(State, 1);

    if (!lua_isstring(State, 2)) return luaL_error(State, "Invalid argument for layout!");
    CString Name = (CString)lua_tostring(State, 2);

    PacketLayoutRef PacketLayout = PacketManagerGetLayout(PacketManager, Name);
    if (!PacketLayout) return luaL_error(State, "Packet layout not found for name: '%s'", Name);

    if (!lua_istable(State, 3)) return luaL_error(State, "Invalid argument for payload!");
    lua_pushvalue(State, 3);
    
    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
    Void* Packet = _PacketBufferInit(PacketBuffer, false, 6, Command);

    if (!PacketLayoutEncode(PacketLayout, PacketBuffer, State)) {
        lua_settop(State, StateStack);
        return luaL_error(State, "Failed to encode packet layout!");
    }

    lua_settop(State, StateStack);

    SocketSend(Socket, SocketConnection, Packet);
    return 0;
}

Void PacketManagerRegisterScriptAPI(
    PacketManagerRef PacketManager
) {
    lua_pushcfunction(PacketManager->State, PacketManagerAPI_RegisterPacketLayout);
    lua_setglobal(PacketManager->State, "RegisterPacketLayout");
    
    lua_pushcfunction(PacketManager->State, PacketManagerAPI_RegisterPacketHandler);
    lua_setglobal(PacketManager->State, "RegisterPacketHandler");

    lua_pushcfunction(PacketManager->State, PacketManagerAPI_Unicast);
    lua_setglobal(PacketManager->State, "Unicast");
}
