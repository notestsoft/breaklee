#include "Character.h"
#include "CharacterAPI.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Int32 lua_RTCharacterSetBattleRank(lua_State* State) {
    lua_getfield(State, 1, "_Runtime");
    RTRuntimeRef Runtime = (RTRuntimeRef)lua_touserdata(State, -1);
    lua_pop(State, 1);

    lua_getfield(State, 1, "_Object");
    RTCharacterRef Character = (RTCharacterRef)lua_touserdata(State, -1);
    lua_pop(State, 1);

    Int32 BattleRank = luaL_checkinteger(State, 2);
    RTCharacterSetBattleRank(Runtime, Character, BattleRank);
    return 0;
}

Int32 lua_RTCharacterSetAllQuestFlags(lua_State* State) {
    lua_getfield(State, 1, "_Runtime");
    RTRuntimeRef Runtime = (RTRuntimeRef)lua_touserdata(State, -1);
    lua_pop(State, 1);

    lua_getfield(State, 1, "_Object");
    RTCharacterRef Character = (RTCharacterRef)lua_touserdata(State, -1);
    lua_pop(State, 1);

    RTCharacterSetAllQuestFlags(Runtime, Character);
    return 0;
}

Int32 lua_RTCharacterDisconnectWorld(lua_State* State) {
    lua_getfield(State, 1, "_Runtime");
    RTRuntimeRef Runtime = (RTRuntimeRef)lua_touserdata(State, -1);
    lua_pop(State, 1);

    lua_getfield(State, 1, "_Object");
    RTCharacterRef Character = (RTCharacterRef)lua_touserdata(State, -1);
    lua_pop(State, 1);

    NOTIFICATION_DATA_ERROR_CODE* Notification = RTNotificationInit(ERROR_CODE);
    Notification->ErrorCode = 38;
    RTNotificationDispatchToCharacter(Notification, Character);

    return 0;
}

Int32 lua_RTCharacterSpawnObject(lua_State* State) {
    lua_getfield(State, 1, "_Runtime");
    RTRuntimeRef Runtime = (RTRuntimeRef)lua_touserdata(State, -1);
    lua_pop(State, 1);

    lua_getfield(State, 1, "_Object");
    RTCharacterRef Character = (RTCharacterRef)lua_touserdata(State, -1);
    lua_pop(State, 1);

    Int32 EntityIndex = luaL_checkinteger(State, 2);
    Int32 ObjectType = luaL_checkinteger(State, 3);
    Int32 EntitySourceIndex = luaL_checkinteger(State, 4);
    Int32 PositionX = luaL_checkinteger(State, 5);
    Int32 PositionY = luaL_checkinteger(State, 6);
    Int32 Hp = luaL_checkinteger(State, 7);
    Int32 Nation = luaL_checkinteger(State, 8);
    Int32 Status = luaL_checkinteger(State, 9);
    Int32 NpcIndex = luaL_checkinteger(State, 10);

    NOTIFICATION_DATA_OBJECTS_SPAWN* Notification = RTNotificationInit(OBJECTS_SPAWN);
    Notification->Count = 1;

    NOTIFICATION_DATA_OBJECTS_SPAWN_INDEX* NotificationObject = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_OBJECTS_SPAWN_INDEX);
    NotificationObject->Entity.EntityIndex = EntityIndex;
    NotificationObject->Entity.WorldIndex = Character->Movement.WorldContext->WorldData->WorldIndex;
    NotificationObject->Entity.EntityType = RUNTIME_ENTITY_TYPE_OBJECT;
    NotificationObject->ObjectType = ObjectType;
    NotificationObject->EntitySourceIndex = EntitySourceIndex;
    NotificationObject->PositionX = PositionX;
    NotificationObject->PositionY = PositionY;
    NotificationObject->CurrentHP = Hp;
    NotificationObject->MaxHP = Hp;
    NotificationObject->Nation = Nation;
    NotificationObject->Status = Status;
    NotificationObject->CharacterIndex = Character->CharacterIndex;
    NotificationObject->NpcIndex = NpcIndex;
    RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

    return 0;
}

Void RTScriptBindCharacterAPI(
	RTScriptRef Script
) {
    RTScriptCreateClass(
        Script,
        "Character",
        "SetBattleRank", lua_RTCharacterSetBattleRank,
        "SetAllQuestFlags", lua_RTCharacterSetAllQuestFlags,
        "DisconnectWorld", lua_RTCharacterDisconnectWorld,
        "SpawnObject", lua_RTCharacterSpawnObject,
        NULL
    );
}
/*
Void RTScriptBindCharacterAPI(
    RTScriptRef Script
) {
    CString MetaName = RTScriptCreateTableBinding(
        Script,
        "Character",
        "SetBattleRank", lua_RTCharacterSetBattleRank,
        "AddExp", lua_RTCharacterAddExp,
        "AddSkillExp", lua_RTCharacterAddSkillExp,
        "AddHonorPoint", lua_RTCharacterAddHonorPoint,
        "AddAbilityExp", lua_RTCharacterAddAbilityExp,
        "AddRage", lua_RTCharacterAddRage,
        "ConsumeRage", lua_RTCharacterConsumeRage,
        "SetHP", lua_RTCharacterSetHP,
        "AddHP", lua_RTCharacterAddHP,
        "SetMP", lua_RTCharacterSetMP,
        "AddMP", lua_RTCharacterAddMP,
        "SetSP", lua_RTCharacterSetSP,
        "AddSP", lua_RTCharacterAddSP,
        "SetBP", lua_RTCharacterSetBP,
        "AddBP", lua_RTCharacterAddBP,
        "ApplyDamage", lua_RTCharacterApplyDamage,
        NULL
    );

    // Lua function to set class rank using lightuserdata
    int lua_Character_SetClassRank(lua_State * L) {
        lua_getfield(L, 1, "ptr");  // Get the lightuserdata (the pointer)
        Character* character = (Character*)lua_touserdata(L, -1);
        lua_pop(L, 1);

        int rank = luaL_checkinteger(L, 2);
        character->SetClassRank(rank);
        return 0;
    }
}
*/