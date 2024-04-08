#pragma once

#include "Base.h"

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
    CString Function,
    ...
);

EXTERN_C_BEGIN
