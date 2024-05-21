#include "PacketLayout.h"

#define PACKET_MANAGER_GLOBAL_USERDATA_NAME "_PacketManager"

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
    Index Index;
    Index Type;
    Index Length;
    Index ChildIndex;
    Index CountIndex;
    Char Name[MAX_PATH];
    Index StackOffset;
};

struct _PacketLayout {
    PacketManagerRef Manager;
    Index Index;
    Char Name[MAX_PATH];
    ArrayRef Fields;
    DictionaryRef NameToField;
};

struct _PacketHandler {
    Index Command;
    Index LayoutIndex;
    Int32 Handler;
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

Bool PacketLayoutParse(
    PacketLayoutRef PacketLayout,
    UInt8* Buffer,
    Int32* OutOffset,
    Int32 Length,
    lua_State* State
);

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
    lua_setglobal(PacketManager->State, PACKET_MANAGER_GLOBAL_USERDATA_NAME);
    
    if (luaL_dostring(PacketManager->State, kScriptPrelude) != LUA_OK) {
        Fatal("Lua: Error %s", lua_tostring(PacketManager->State, -1));
    }
    
    PacketManager->PacketLayouts = ArrayCreateEmpty(Allocator, sizeof(struct _PacketLayout), 8);
    PacketManager->NameToPacketLayout = CStringDictionaryCreate(Allocator, 8);
    PacketManager->CommandToPacketHandler = IndexDictionaryCreate(Allocator, 8);
    PacketManagerRegisterScriptAPI(PacketManager);
    return PacketManager;
}

Void PacketManagerDestroy(
    PacketManagerRef PacketManager
) {
    lua_close(PacketManager->State);

    for (Index Index = 0; Index < ArrayGetElementCount(PacketManager->PacketLayouts); Index += 1) {
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
    
    Index PacketLayoutIndex = ArrayGetElementCount(PacketManager->PacketLayouts);
    PacketLayoutRef PacketLayout = (PacketLayoutRef)ArrayAppendUninitializedElement(PacketManager->PacketLayouts);
    memset(PacketLayout, 0, sizeof(struct _PacketLayout));
    
    PacketLayout->Manager = PacketManager;
    PacketLayout->Index = PacketLayoutIndex;
    strcpy(PacketLayout->Name, Name);
    PacketLayout->Fields = ArrayCreateEmpty(PacketManager->Allocator, sizeof(struct _PacketField), 8);
    PacketLayout->NameToField = CStringDictionaryCreate(PacketManager->Allocator, 8);
    
    DictionaryInsert(PacketManager->NameToPacketLayout, Name, &PacketLayoutIndex, sizeof(Index));
    
    return PacketLayout;
}

Index PacketManagerGetLayoutIndex(
    PacketManagerRef PacketManager,
    CString Name
) {
    Index* PacketLayoutIndex = DictionaryLookup(PacketManager->NameToPacketLayout, Name);
    if (!PacketLayoutIndex) return UINT64_MAX;

    return *PacketLayoutIndex;
}

PacketLayoutRef PacketManagerGetLayoutByIndex(
    PacketManagerRef PacketManager,
    Index Index
) {
    assert(Index < ArrayGetElementCount(PacketManager->PacketLayouts));
    return (PacketLayoutRef)ArrayGetElementAtIndex(PacketManager->PacketLayouts, Index);
}

PacketLayoutRef PacketManagerGetLayout(
    PacketManagerRef PacketManager,
    CString Name
) {
    Index Index = PacketManagerGetLayoutIndex(PacketManager, Name);
    if (Index == UINT64_MAX) return NULL;

    return PacketManagerGetLayoutByIndex(PacketManager, Index);
}

Int32 PacketManagerHandle(
    PacketManagerRef PacketManager,
    Index Command,
    UInt8* Buffer,
    Int32 Length
) {
    PacketHandlerRef PacketHandler = DictionaryLookup(PacketManager->CommandToPacketHandler, &Command);
    if (!PacketHandler) return 0;
    
    PacketLayoutRef PacketLayout = PacketManagerGetLayoutByIndex(PacketManager, PacketHandler->LayoutIndex);
    if (!PacketLayout) return 0;
    
    Int32 StateStack = lua_gettop(PacketManager->State);
    lua_rawgeti(PacketManager->State, LUA_REGISTRYINDEX, PacketHandler->Handler);

    Int32 Offset = 0;
    Bool Success = PacketLayoutParse(PacketLayout, Buffer, &Offset, Length, PacketManager->State);
    if (!Success) {
        lua_settop(PacketManager->State, StateStack);
        return -1;
    }

    Int32 ArgumentCount = 1;
    Int32 ReturnValueCount = 0;
    Int32 Result = lua_pcall(PacketManager->State, ArgumentCount, ReturnValueCount, 0);
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

Index PacketLayoutGetSize(
    PacketLayoutRef PacketLayout
);

Index PacketFieldGetSize(
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

Index PacketLayoutGetSize(
    PacketLayoutRef PacketLayout
) {
    Index Size = 0;
    
    for (Index Index = 0; Index < ArrayGetElementCount(PacketLayout->Fields); Index += 1) {
        PacketFieldRef PacketField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, Index);
        Size += PacketFieldGetSize(PacketLayout, PacketField);
    }
    
    return Size;
}

Void PacketLayoutAddField(
    PacketLayoutRef PacketLayout,
    CString Name,
    Index Type,
    Index Length,
    Index ChildIndex,
    Index CountIndex
) {
    if (DictionaryLookup(PacketLayout->NameToField, Name)) Fatal("Packet field with name '%s' already registered!", Name);
    
    Index PacketFieldIndex = ArrayGetElementCount(PacketLayout->Fields);
    PacketFieldRef PacketField = (PacketFieldRef)ArrayAppendUninitializedElement(PacketLayout->Fields);
    PacketField->Index = PacketFieldIndex;
    PacketField->Type = Type;
    PacketField->Length = Length;
    PacketField->ChildIndex = ChildIndex;
    PacketField->CountIndex = CountIndex;
    strcpy(PacketField->Name, Name);
    PacketField->StackOffset = 0;

    DictionaryInsert(PacketLayout->NameToField, Name, &PacketFieldIndex, sizeof(Index));
}

Void PacketLayoutAddInt8(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_INT8, sizeof(Int8), 0, 0);
}

Void PacketLayoutAddInt16(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_INT16, sizeof(Int16), 0, 0);
}

Void PacketLayoutAddInt32(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_INT32, sizeof(Int32), 0, 0);
}

Void PacketLayoutAddInt64(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_INT64, sizeof(Int64), 0, 0);
}

Void PacketLayoutAddUInt8(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_UINT8, sizeof(UInt8), 0, 0);
}

Void PacketLayoutAddUInt16(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_UINT16, sizeof(UInt16), 0, 0);
}

Void PacketLayoutAddUInt32(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_UINT32, sizeof(UInt32), 0, 0);
}

Void PacketLayoutAddUInt64(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_UINT64, sizeof(UInt64), 0, 0);
}

Void PacketLayoutAddCString(
    PacketLayoutRef PacketLayout,
    CString Name
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_STRING, sizeof(Char), 0, 0);
}

Void PacketLayoutAddCharacters(
    PacketLayoutRef PacketLayout,
    CString Name,
    Int32 Count
) {
    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_CHARACTERS, sizeof(Char) * Count, 0, 0);
}

Void PacketLayoutAddStaticArray(
    PacketLayoutRef PacketLayout,
    CString Name,
    CString ChildName,
    Int32 Count
) {
    Index ChildIndex = PacketManagerGetLayoutIndex(PacketLayout->Manager, ChildName);
    if (ChildIndex == UINT64_MAX) Fatal("Packet layout named '%s' not found!", ChildName);

    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_STATIC_ARRAY, Count, ChildIndex, 0);
}

Void PacketLayoutAddDynamicArray(
    PacketLayoutRef PacketLayout,
    CString Name,
    CString ChildName,
    CString CountName
) {
    Index ChildIndex = PacketManagerGetLayoutIndex(PacketLayout->Manager, ChildName);
    if (ChildIndex == UINT64_MAX) Fatal("Packet layout named '%s' not found!", ChildName);

    Index* CountIndex = DictionaryLookup(PacketLayout->NameToField, CountName);
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

    PacketLayoutAddField(PacketLayout, Name, PACKET_FIELD_TYPE_DYNAMIC_ARRAY, 0, ChildIndex, *CountIndex);
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

Bool PacketLayoutParse(
    PacketLayoutRef PacketLayout,
    UInt8* Buffer,
    Int32* OutOffset,
    Int32 Length,
    lua_State* State
) {
    lua_newtable(State);

    for (Index Index = 0; Index < ArrayGetElementCount(PacketLayout->Fields); Index += 1) {
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
            Char* Value = ((Char*)&Buffer[PacketField->StackOffset]);
            lua_pushstring(State, Value);
        }
        
        if (PacketField->Type == PACKET_FIELD_TYPE_STATIC_ARRAY) {
            PacketLayoutRef Child = PacketManagerGetLayoutByIndex(PacketLayout->Manager, PacketField->ChildIndex);
           
            lua_createtable(State, (Int32)PacketField->Length, 0);
            for (Int32 ChildIndex = 0; ChildIndex < PacketField->Length; ChildIndex += 1) {
                lua_pushinteger(State, ChildIndex + 1);
                if (!PacketLayoutParse(Child, Buffer, OutOffset, Length, State)) return false;

                lua_settable(State, -3);
            }
            
            FieldLength = 0;
        }
        
        if (PacketField->Type == PACKET_FIELD_TYPE_DYNAMIC_ARRAY) {
            PacketLayoutRef Child = PacketManagerGetLayoutByIndex(PacketLayout->Manager, PacketField->ChildIndex);
            PacketFieldRef CountField = (PacketFieldRef)ArrayGetElementAtIndex(PacketLayout->Fields, PacketField->CountIndex);
            Int32 Count = (Int32)PacketFieldReadPrimitive(CountField, Buffer);
            
            lua_createtable(State, Count, 0);
            for (Int32 ChildIndex = 0; ChildIndex < Count; ChildIndex += 1) {
                lua_pushinteger(State, ChildIndex + 1);
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

static Int32 PacketManagerAPI_RegisterPacketLayout(
    lua_State* State
) {
    Int32 StateStack = lua_gettop(State);

    Int32 Result = lua_getglobal(State, PACKET_MANAGER_GLOBAL_USERDATA_NAME);
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
                    Int32 Count = (Int32)lua_tointeger(State, -1);
                    lua_pop(State, 1);

                    PacketLayoutAddCharacters(PacketLayout, FieldName, Count);
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
                    luaL_error(State, "Invalid field type given!");
                    break;
            }

            lua_pop(State, 1);
        }

        lua_pop(State, 1);
    }

    return 0;
}

static Int32 PacketManagerAPI_RegisterPacketHandler(
    lua_State* State
) {
    Int32 StateStack = lua_gettop(State);

    Int32 Result = lua_getglobal(State, PACKET_MANAGER_GLOBAL_USERDATA_NAME);
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

    if (!lua_isinteger(State, 1)) return luaL_error(State, "Invalid argument for command!");
    Index Command = (Index)lua_tointeger(State, 1);

    if (!lua_isstring(State, 2)) return luaL_error(State, "Invalid argument for handler!");
    CString Name = (CString)lua_tostring(State, 2);

    if (!lua_isfunction(State, 3)) return luaL_error(State, "Invalid argument for handler function!");
    lua_pushvalue(State, 3);
    Int32 Handler = luaL_ref(State, LUA_REGISTRYINDEX);

    if (DictionaryLookup(PacketManager->CommandToPacketHandler, &Command)) {
        return luaL_error(State, "Handler for command %d already registered!", Command);
    }
    
    Index LayoutIndex = PacketManagerGetLayoutIndex(PacketManager, Name);
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

Void PacketManagerRegisterScriptAPI(
    PacketManagerRef PacketManager
) {
    lua_pushcfunction(PacketManager->State, PacketManagerAPI_RegisterPacketLayout);
    lua_setglobal(PacketManager->State, "RegisterPacketLayout");
    
    lua_pushcfunction(PacketManager->State, PacketManagerAPI_RegisterPacketHandler);
    lua_setglobal(PacketManager->State, "RegisterPacketHandler");
}
