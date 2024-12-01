#pragma once

#include "Base.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

EXTERN_C_BEGIN

RTScriptManagerRef RTScriptManagerCreate(
    RTRuntimeRef Runtime,
    Int MaxScriptCount
);

Void RTScriptManagerDestroy(
    RTScriptManagerRef ScriptManager
);

RTScriptRef RTScriptManagerLoadScript(
    RTScriptManagerRef ScriptManager,
    CString FilePath
);

Void RTScriptManagerUnloadScript(
    RTScriptManagerRef ScriptManager,
    RTScriptRef Script
);

Void* RTScriptGetState(
    RTScriptRef Script
);

Void RTScriptCreateClass(
    RTScriptRef Script,
    CString Name,
    ...
);

Void RTScriptPushObject(
    RTScriptRef Script,
    CString Name,
    RTRuntimeRef Runtime,
    Void* Object
);

Bool RTScriptCall(
    RTScriptRef Script,
    const Char* Function,
    ...
);

Bool RTScriptCallOnEvent(
    RTScriptRef Script,
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

EXTERN_C_BEGIN
