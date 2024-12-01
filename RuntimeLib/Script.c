#include "CharacterAPI.h"
#include "Mob.h"
#include "Runtime.h"
#include "Script.h"
#include "NotificationManager.h"
#include "NotificationProtocol.h"
#include "WorldAPI.h"

static Int32 _DebugWorldSpawnMob(lua_State* State);
static Int32 _DebugWorldDespawnMob(lua_State* State);
static Int32 _DebugWorldSpawnItem(lua_State* State);
static Int32 _DebugMobSetPosition(lua_State* State);

struct _RTScript {
    Int PoolIndex;
    Char FilePath[MAX_PATH];
    FileEventRef FileEvent;
    lua_State* State;
};

struct _RTScriptManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    Int MaxScriptCount;
    MemoryPoolRef ScriptPool;
    DictionaryRef ScriptTable;
};

Void* RTScriptGetState(
    RTScriptRef Script
) {
    return Script->State;
}

#define SCRIPT_STRUCTURE_BINDING(__TYPE__, __ACCESSOR__, __ACCESSOR_TYPE__, __NAME__)     \
static Int32 SCRIPT_PROC_ ## __TYPE__ ## _ ## __NAME__ (lua_State* State) {               \
    struct _RT ## __TYPE__* __TYPE__ = (struct _RT ## __TYPE__*)lua_touserdata(State, 1); \
    lua_push ## __ACCESSOR_TYPE__ (State, __ACCESSOR__);                                  \
    Int32 ReturnValueCount = 1;                                                           \
    return ReturnValueCount;                                                              \
}
#include "MobAPI.h"

Void RTScriptBindMobAPI(
    RTScriptRef Script
) {
#define SCRIPT_STRUCTURE_BINDING(__TYPE__, __ACCESSOR__, __ACCESSOR_TYPE__, __NAME__) \
    lua_pushcfunction(Script->State, SCRIPT_PROC_ ## __TYPE__ ## _ ## __NAME__);      \
    lua_setglobal(Script->State, # __NAME__);
#include "MobAPI.h"
}

RTScriptManagerRef RTScriptManagerCreate(
    RTRuntimeRef Runtime,
    Int MaxScriptCount
) {
    RTScriptManagerRef ScriptManager = AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTScriptManager));
    if (!ScriptManager) Fatal("Memory allocation failed!");

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
        Int MemoryPoolIndex = *(Int*)Iterator.Value;
        RTScriptRef Script = MemoryPoolFetch(ScriptManager->ScriptPool, MemoryPoolIndex);
        if (Script->FileEvent) FileEventDestroy(Script->FileEvent);
        lua_close(Script->State);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    DictionaryDestroy(ScriptManager->ScriptTable);
    MemoryPoolDestroy(ScriptManager->ScriptPool);
    AllocatorDeallocate(ScriptManager->Allocator, ScriptManager);
}

Void _OnScriptFileChanged(
    CString FileName,
    Void* UserData
) {
    RTScriptRef Script = (RTScriptRef)UserData;
    lua_State* OldState = Script->State;
    lua_State* NewState = luaL_newstate();
    if (!NewState) {
        Error("Lua: State creation failed!");
        return;
    }

    Script->State = NewState;

    luaL_openlibs(Script->State);
    RTScriptBindMobAPI(Script);
    RTScriptBindCharacterAPI(Script);
    RTScriptBindWorldAPI(Script);

    lua_pushcfunction(Script->State, _DebugWorldSpawnMob);
    lua_setglobal(Script->State, "world_spawn_mob");

    lua_pushcfunction(Script->State, _DebugWorldDespawnMob);
    lua_setglobal(Script->State, "world_despawn_mob");

    lua_pushcfunction(Script->State, _DebugWorldSpawnItem);
    lua_setglobal(Script->State, "world_spawn_item");

    lua_pushcfunction(Script->State, _DebugMobSetPosition);
    lua_setglobal(Script->State, "mob_set_pos");

    if (luaL_loadfile(Script->State, Script->FilePath) != LUA_OK) {
        Error("Lua: %s", lua_tostring(Script->State, -1));
        lua_close(Script->State);
        Script->State = OldState;
        return;
    }

    if (lua_pcall(Script->State, 0, 0, 0) != LUA_OK) {
        Error("Lua: %s", lua_tostring(Script->State, -1));
        lua_close(Script->State);
        Script->State = OldState;
        return;
    }
}

RTScriptRef RTScriptManagerLoadScript(
    RTScriptManagerRef ScriptManager,
    CString FilePath
) {
    Int* ScriptPoolIndex = (Int*)DictionaryLookup(ScriptManager->ScriptTable, FilePath);
    if (ScriptPoolIndex) return (RTScriptRef)MemoryPoolFetch(ScriptManager->ScriptPool, *ScriptPoolIndex);

    Int MemoryPoolIndex = 0;
    RTScriptRef Script = (RTScriptRef)MemoryPoolReserveNext(ScriptManager->ScriptPool, &MemoryPoolIndex);
    DictionaryInsert(ScriptManager->ScriptTable, FilePath, &MemoryPoolIndex, sizeof(Int));

    CStringCopySafe(Script->FilePath, MAX_PATH, FilePath);
    Script->PoolIndex = MemoryPoolIndex;
    Script->State = luaL_newstate();
    if (!Script->State) Fatal("Lua: State creation failed!");

    luaL_openlibs(Script->State);

    RTScriptBindMobAPI(Script);
    RTScriptBindCharacterAPI(Script);
    RTScriptBindWorldAPI(Script);

    lua_pushcfunction(Script->State, _DebugWorldSpawnMob);
    lua_setglobal(Script->State, "world_spawn_mob");

    lua_pushcfunction(Script->State, _DebugWorldDespawnMob);
    lua_setglobal(Script->State, "world_despawn_mob");

    lua_pushcfunction(Script->State, _DebugWorldSpawnItem);
    lua_setglobal(Script->State, "world_spawn_item");

    lua_pushcfunction(Script->State, _DebugMobSetPosition);
    lua_setglobal(Script->State, "mob_set_pos");

    if (luaL_loadfile(Script->State, FilePath) != LUA_OK) Fatal("Lua: %s", lua_tostring(Script->State, -1));
    if (lua_pcall(Script->State, 0, 0, 0) != LUA_OK) Fatal("Lua: %s", lua_tostring(Script->State, -1));

    Script->FileEvent = FileEventCreate(Script->FilePath, _OnScriptFileChanged, Script);

    return Script;
}

Void RTScriptManagerUnloadScript(
    RTScriptManagerRef ScriptManager,
    RTScriptRef Script
) {
    if (Script->FileEvent) FileEventDestroy(Script->FileEvent); 
    lua_close(Script->State);
    DictionaryRemove(ScriptManager->ScriptTable, Script->FilePath);
    MemoryPoolRelease(ScriptManager->ScriptPool, Script->PoolIndex);
}

Void RTScriptCreateClass(
    RTScriptRef Script,
    CString Name,
    ...
) {
    luaL_newmetatable(Script->State, CStringFormat("%sMeta", Name));

    va_list Arguments;
    va_start(Arguments, Name);

    while (true) {
        CString CallbackName = va_arg(Arguments, CString);
        if (!CallbackName) break;

        Void* Callback = va_arg(Arguments, Void*);
        if (!Callback) break;

        lua_pushcfunction(Script->State, Callback);
        lua_setfield(Script->State, -2, CallbackName);
    }

    va_end(Arguments);

    lua_pushvalue(Script->State, -1);
    lua_setfield(Script->State, -2, "__index");

    lua_pop(Script->State, 1);
}

Void RTScriptPushObject(
    RTScriptRef Script,
    CString Name,
    RTRuntimeRef Runtime,
    Void* Object
) {
    lua_newtable(Script->State);
    lua_pushlightuserdata(Script->State, Runtime);
    lua_setfield(Script->State, -2, "_Runtime");
    lua_pushlightuserdata(Script->State, Object);
    lua_setfield(Script->State, -2, "_Object");
    luaL_getmetatable(Script->State, CStringFormat("%sMeta", Name));
    lua_setmetatable(Script->State, -2);
}

Bool RTScriptCall(
    RTScriptRef Script,
    const Char* Function,
    ...
) {
    Int32 Result = lua_getglobal(Script->State, Function);
    if (Result != LUA_TFUNCTION) {
        lua_pop(Script->State, 1);
        return false;
    }

    va_list ArgumentPointer;
    va_start(ArgumentPointer, Function);

    Int32 ArgumentCount = 0;
    while (true) {
        Int32 Type = va_arg(ArgumentPointer, Int32);

        if (Type == LUA_TNIL) {
            break;
//            lua_pushnil(Script->State);
        }
        else if (Type == LUA_TBOOLEAN) {
            Bool Value = (Bool)va_arg(ArgumentPointer, Int32);
            lua_pushboolean(Script->State, Value ? 1 : 0);
        }
        else if (Type == LUA_TNUMBER) {
            Int64 Value = va_arg(ArgumentPointer, Int64);
            lua_pushnumber(Script->State, Value);
        }
        else if (Type == LUA_TSTRING) {
            const Char* Value = va_arg(ArgumentPointer, const Char*);
            lua_pushstring(Script->State, Value);
        }
        else if (Type == LUA_TLIGHTUSERDATA) {
            Void* Value = va_arg(ArgumentPointer, Void*);
            lua_pushlightuserdata(Script->State, Value);
        }
        else if (Type == LUA_TUSERDATA) {
            Void* Value = va_arg(ArgumentPointer, Void*);
            lua_pushlightuserdata(Script->State, Value);
        }
        else {
            break;
        }

        ArgumentCount += 1;
    }

    va_end(ArgumentPointer);

    Result = lua_pcall(Script->State, ArgumentCount, 0, 0);
    if (Result != LUA_OK) {
        Error("Lua: %s", lua_tostring(Script->State, -1));
        return false;
    }

    return true;
}

Bool RTScriptCallOnEvent(
    RTScriptRef Script,
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    lua_getglobal(Script->State, "OnEvent");

    RTScriptPushObject(Script, "Character", Runtime, Character);

    if (lua_pcall(Script->State, 1, 0, 0) != LUA_OK) {
        CString Message = (CString)lua_tostring(Script->State, -1);
        Error("Script: %s", Message);
        lua_pop(Script->State, 1);
        return false;
    }

    return true;
}

// TODO: This is just for debugging for now..

static Int32 _DebugWorldSpawnMob(
    lua_State* State
) {
    RTRuntimeRef Runtime = lua_touserdata(State, 1);
    RTWorldContextRef WorldContext = lua_touserdata(State, 2);
    Int32 MobIndex = (Int32)lua_tointeger(State, 3);
    Int32 X = (Int32)lua_tointeger(State, 4);
    Int32 Y = (Int32)lua_tointeger(State, 5);
    
    /*
    Int MemoryPoolIndex = 0;
    RTMobRef Minion = (RTMobRef)MemoryPoolReserveNext(WorldContext->MobPool, &MemoryPoolIndex);
    Minion->ID.EntityIndex = MemoryPoolIndex + 1;
    Minion->ID.WorldIndex = WorldContext->WorldData->WorldIndex;
    Minion->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;
    DictionaryInsert(WorldContext->EntityToMob, &Mob->ID, &MemoryPoolIndex, sizeof(Int));

    Minion->Spawn.MobSpeciesIndex = MobSpeciesIndex;
    Minion->Spawn.AreaX = X;
    Minion->Spawn.AreaY = Y;
    Minion->Spawn.AreaWidth = 1;
    Minion->Spawn.AreaHeight = 1;

    Minion->SpeciesData = &Runtime->MobData[MobSpeciesIndex];
    Minion->IsInfiniteSpawn = true;
    Minion->IsPermanentDeath = false;
    Minion->RemainingSpawnCount = 0;
    RTWorldSpawnMob(Runtime, WorldContext, Minion);
    */

    RTEntityID MobID = { 0 };
    MobID.EntityIndex = MobIndex;
    MobID.WorldIndex = WorldContext->WorldData->WorldIndex;
    MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;
    RTMobRef Minion = RTWorldContextGetMob(WorldContext, MobID);
    if (Minion && !Minion->IsSpawned) {
        Minion->Spawn.AreaX = X;
        Minion->Spawn.AreaY = Y;
        RTWorldSpawnMob(Runtime, WorldContext, Minion);
    }

    return 0;
}

static Int32 _DebugWorldDespawnMob(
    lua_State* State
) {
    RTRuntimeRef Runtime = lua_touserdata(State, 1);
    RTWorldContextRef WorldContext = lua_touserdata(State, 2);
    Int32 MobIndex = (Int32)lua_tointeger(State, 3);
    Int32 X = (Int32)lua_tointeger(State, 4);
    Int32 Y = (Int32)lua_tointeger(State, 5);

    RTEntityID MobID = { 0 };
    MobID.EntityIndex = MobIndex;
    MobID.WorldIndex = WorldContext->WorldData->WorldIndex;
    MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;
    RTMobRef Minion = RTWorldContextGetMob(WorldContext, MobID);
    if (Minion && Minion->IsSpawned) {
        RTWorldDespawnMob(Runtime, WorldContext, Minion);
    }

    return 0;
}

// TODO: This is just for debugging for now..

static Int32 _DebugWorldSpawnItem(
    lua_State* State
) {
    RTRuntimeRef Runtime = lua_touserdata(State, 1);
    RTWorldContextRef WorldContext = lua_touserdata(State, 2);
    UInt32 EntityID = (UInt32)lua_tointeger(State, 3);
    Int64 ItemID = (Int64)lua_tointeger(State, 4);
    Int64 ItemOptions = (Int64)lua_tointeger(State, 5);
    Int32 X = (Int32)lua_tointeger(State, 6);
    Int32 Y = (Int32)lua_tointeger(State, 7);

    RTDropResult Drop = { 0 };
    Drop.ItemID.Serial = ItemID;
    Drop.ItemOptions = ItemOptions;
    RTWorldSpawnItem(Runtime, WorldContext, *(RTEntityID*)(&EntityID), X, Y, Drop);

    return 0;
}

static Int32 _DebugMobSetPosition(
    lua_State* State
) {
    RTRuntimeRef Runtime = lua_touserdata(State, 1);
    RTMobRef Mob = lua_touserdata(State, 2);
    Int32 X = (Int32)lua_tointeger(State, 3);
    Int32 Y = (Int32)lua_tointeger(State, 4);

    if (Mob->Movement.IsMoving) {
        RTMovementEndDeadReckoning(Runtime, &Mob->Movement);

        if (Mob->IsChasing) {
            NOTIFICATION_DATA_MOB_CHASE_END* Notification = RTNotificationInit(MOB_CHASE_END);
            Notification->Entity = Mob->ID;
            Notification->PositionCurrentX = Mob->Movement.PositionCurrent.X;
            Notification->PositionCurrentY = Mob->Movement.PositionCurrent.Y;
            RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
        }
        else {
            NOTIFICATION_DATA_MOB_MOVE_END* Notification = RTNotificationInit(MOB_MOVE_END);
            Notification->Entity = Mob->ID;
            Notification->PositionCurrentX = Mob->Movement.PositionCurrent.X;
            Notification->PositionCurrentY = Mob->Movement.PositionCurrent.Y;
            RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
        }
    }

    RTMovementSetSpeed(Runtime, &Mob->Movement, (Int32)Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED]);
    Mob->Movement.PositionEnd.X = X;
    Mob->Movement.PositionEnd.Y = Y;

    Bool PathFound = RTMovementFindPath(
        Runtime,
        Mob->Movement.WorldContext,
        &Mob->Movement
    );

    if (!PathFound) {
        Mob->Movement.PositionEnd.X = Mob->Movement.Waypoints[1].X;
        Mob->Movement.PositionEnd.Y = Mob->Movement.Waypoints[1].Y;
    }

    RTMovementStartDeadReckoning(Runtime, &Mob->Movement);

    return 0;
}
