#pragma once

#include "Base.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

EXTERN_C_BEGIN

RTScriptManagerRef RTScriptManagerCreate(
    RTRuntimeRef Runtime,
    Index MaxScriptCount
);

Void RTScriptManagerDestroy(
    RTScriptManagerRef ScriptManager
);

RTScriptRef RTScriptManagerLoadScript(
    RTScriptManagerRef ScriptManager,
    CString FilePath
);

Bool RTScriptCall(
    RTScriptRef Script,
    const Char* Function,
    ...
);

EXTERN_C_BEGIN
