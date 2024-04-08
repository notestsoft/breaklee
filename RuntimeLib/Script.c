#include "Script.h"
#include "Runtime.h"

struct _RTScript {
    Index PoolIndex;
    lua_State* State;
};

struct _RTScriptManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    Index MaxScriptCount;
    MemoryPoolRef ScriptPool;
    DictionaryRef ScriptTable;
};

RTScriptManagerRef RTScriptManagerCreate(
    RTRuntimeRef Runtime,
    Index MaxScriptCount
) {
    RTScriptManagerRef ScriptManager = AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTScriptManager));
    if (!ScriptManager) FatalError("Memory allocation failed!");

    ScriptManager->Allocator = Runtime->Allocator;
    ScriptManager->Runtime = Runtime;
    ScriptManager->MaxScriptCount = MaxScriptCount;
    ScriptManager->ScriptPool = MemoryPoolCreate(Runtime->Allocator, sizeof(struct _RTScript), MaxScriptCount);
    ScriptManager->ScriptTable = CStringDictionaryCreate(Runtime->Allocator, MaxScriptCount);

    return ScriptManager;
}

Void RTScriptManagerDestroy(
    RTScriptManagerRef ScriptManager
) {
    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(ScriptManager->ScriptTable);
    while (Iterator.Key) {
        Index MemoryPoolIndex = *(Index*)Iterator.Value;
        RTScriptRef Script = MemoryPoolFetch(ScriptManager->ScriptPool, MemoryPoolIndex);
        lua_close(Script->State);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    DictionaryDestroy(ScriptManager->ScriptTable);
    MemoryPoolDestroy(ScriptManager->ScriptPool);
    AllocatorDeallocate(ScriptManager->Allocator, ScriptManager);
}

RTScriptRef RTScriptManagerLoadScript(
    RTScriptManagerRef ScriptManager,
    CString FilePath
) {
    Index* ScriptPoolIndex = (Index*)DictionaryLookup(ScriptManager->ScriptTable, FilePath);
    if (ScriptPoolIndex) return (RTScriptRef)MemoryPoolFetch(ScriptManager->ScriptPool, *ScriptPoolIndex);

    Index MemoryPoolIndex = 0;
    RTScriptRef Script = (RTScriptRef)MemoryPoolReserveNext(ScriptManager->ScriptPool, &MemoryPoolIndex);
    DictionaryInsert(ScriptManager->ScriptTable, FilePath, &MemoryPoolIndex, sizeof(Index));

    Script->PoolIndex = MemoryPoolIndex;
    Script->State = luaL_newstate();
    if (!Script->State) FatalError("Lua: State creation failed!");

    luaL_openlibs(Script->State);
    
    if (luaL_loadfile(Script->State, FilePath) != LUA_OK) FatalError("Lua: %s", lua_tostring(Script->State, -1));
    if (lua_pcall(Script->State, 0, 0, 0) != LUA_OK) FatalError("Lua: %s", lua_tostring(Script->State, -1));

    return Script;
}

Bool RTScriptCall(
    RTScriptRef Script,
    CString Function,
    ...
) {
    Int32 Result = lua_getglobal(Script->State, Function);
    if (Result != LUA_TFUNCTION) return false;

    va_list ArgumentPointer;
    va_start(ArgumentPointer, Function);

    Void* Argument = NULL;
    Int32 ArgumentCount = 0;
    while (Argument = va_arg(ArgumentPointer, Void*)) {
        lua_pushlightuserdata(Script->State, Argument);
        ArgumentCount += 1;
    }

    va_end(ArgumentPointer);

    Result = lua_pcall(Script->State, ArgumentCount, 0, 0);
    if (Result != LUA_OK) {
        LogMessageFormat(LOG_LEVEL_ERROR, "Lua: %s", lua_tostring(Script->State, -1));
        return false;
    }

    return true;
}
