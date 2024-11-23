#pragma once

#include "Context.h"
#include "Constants.h"

EXTERN_C_BEGIN

Void ServerLoadRuntimeData(
    ServerConfig Config,
    ServerContextRef Context
);

Void ServerLoadScriptData(
    ServerConfig Config,
    ServerContextRef Context
);

Bool ServerLoadCharacterTemplateData(
    ServerContextRef Context,
    ArchiveRef RankArchive
);

Bool ServerLoadBattleStyleFormulaData(
    RTRuntimeRef Runtime,
    ArchiveRef Archive
);

Bool ServerLoadQuestData(
    RTRuntimeRef Runtime,
    ArchiveRef Archive
);

Bool ServerLoadItemData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory
);

Bool ServerLoadMobData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory
);

Bool ServerLoadTerrainData(
    RTRuntimeRef Runtime,
    CString RuntimeDirectory,
    CString ServerDirectory,
    CString ScriptDirectory,
    ArchiveRef TerrainArchive
);

Bool ServerLoadWorldData(
    RTRuntimeRef Runtime,
    CString RuntimeDirectory,
    CString ServerDirectory,
    CString ScriptDirectory,
    ArchiveRef MainArchive,
    ArchiveRef TempArchive
);

Bool ServerLoadSkillData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef SkillArchive
);

Bool ServerLoadQuestDungeonData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef Archive
);

Bool ServerLoadMissionDungeonData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef Archive
);

Bool ServerLoadWorldDropData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef TempArchive
);

Bool ServerLoadMobPatrolData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory
);

Bool ServerLoadMobPatternData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    CString ScriptDirectory
);

Bool ServerLoadOptionPoolData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory
);

EXTERN_C_END
