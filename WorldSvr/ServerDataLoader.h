#pragma once

#include "Context.h"
#include "Constants.h"
#include "GameProtocol.h"

EXTERN_C_BEGIN

Void ServerLoadRuntimeData(
    ServerConfig Config,
    ServerContextRef Context
);

Bool ServerLoadCharacterTemplateData(
    ServerContextRef Context,
    ArchiveRef RankArchive,
    ArchiveRef CharacterInitArchive
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

Bool ServerLoadShopData(
    RTRuntimeRef Runtime,
    CString RuntimeDirectory,
    CString ServerDirectory
);

Bool ServerLoadWorldData(
    RTRuntimeRef Runtime,
    CString RuntimeDirectory,
    CString ServerDirectory,
    CString ScriptDirectory,
    ArchiveRef TerrainArchive,
    ArchiveRef MainArchive,
    Bool LoadShops
);

Bool ServerLoadSkillData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef SkillArchive
);

Bool ServerLoadDungeonData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef Cont1Archive,
    ArchiveRef Cont2Archive,
    ArchiveRef Cont3Archive
);

EXTERN_C_END
