#pragma once

#include "Context.h"

EXTERN_C_BEGIN

Void LoadMissionDungeonLocales(
    EditorContextRef Context,
    CString FileName
);

Void LoadMissionDungeonFiles(
    EditorContextRef Context,
    CString FileName
);

Void LoadMissionDungeonData(
    EditorContextRef Context,
    ArrayRef MissionDungeons,
    CString FileName
);

EXTERN_C_END
