#include "DataLoader.h"

Void LoadMissionDungeonLocales(
    EditorContextRef Context,
    CString FileName
) {
    CString FilePath = PathCombineAll(Context->Config.Editor.ClientDataPath, "Language", Context->Config.Editor.ClientLanguage, FileName, NULL);
    ArchiveRef Archive = ArchiveCreateEmpty(Context->Allocator);
    if (!ArchiveLoadFromFileEncrypted(Archive, FilePath, true)) {
        ArchiveDestroy(Archive);
        return;
    }

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, -1, "cabal_message.dungeon_msg.msg");
    while (Iterator) {
        Int DungeonIndex = 0;
        Char Name[PLATFORM_PATH_MAX] = { 0 };
        if (!ParseAttributeInt(Archive, Iterator->Index, "id", &DungeonIndex)) return;
        if (!ParseAttributeString(Archive, Iterator->Index, "name", Name, PLATFORM_PATH_MAX)) return;

        DictionaryInsert(Context->MissionDungeonNames, &DungeonIndex, Name, sizeof(Name));

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveDestroy(Archive);
}

Void LoadMissionDungeonFiles(
    EditorContextRef Context,
    CString FileName
) {
    CString FilePath = PathCombineAll(Context->Config.Editor.ClientDataPath, FileName, NULL);
    ArchiveRef Archive = ArchiveCreateEmpty(Context->Allocator);
    if (!ArchiveLoadFromFileEncrypted(Archive, FilePath, true)) {
        ArchiveDestroy(Archive);
        return;
    }

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, -1, "cabal.cabal_mission_dungeon.dungeon");
    while (Iterator) {
        Int DungeonIndex = 0;
        Char Name[PLATFORM_PATH_MAX] = { 0 };
        if (!ParseAttributeInt(Archive, Iterator->Index, "id", &DungeonIndex)) return;
        if (!ParseAttributeString(Archive, Iterator->Index, "file", Name, PLATFORM_PATH_MAX)) return;

        DictionaryInsert(Context->MissionDungeonFiles, &DungeonIndex, Name, sizeof(Name));

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveDestroy(Archive);
}

Void LoadMissionDungeonData(
    EditorContextRef Context,
    ArrayRef MissionDungeons,
    CString FileName
) {
    CString FilePath = PathCombineNoAlloc(Context->Config.Editor.ServerDataPath, FileName);
    CLTableSetRef TableSet = CLTableSetCreateEmpty(Context->Allocator);
    if (!CLTableSetLoadFromFile(TableSet, FilePath)) Fatal("Loading file '%s' failed!", FileName);

    CLTableRef Table = CLTableSetGetTable(TableSet, "MD");
    if (!Table) Fatal("Table '%s' not found in file '%s'!", "MD", FileName);

    for (Int RowIndex = 0; RowIndex < CLTableGetRowCount(Table); RowIndex += 1) {
        MissionDungeonDataRef Data = (MissionDungeonDataRef)ArrayAppendUninitializedElement(MissionDungeons);
        if (!CLTableGetValueInt32(Table, "MD", RowIndex, &Data->RowIndex)) goto error;
        if (!CLTableGetValueInt32(Table, "QDungeonIdx", RowIndex, &Data->QDungeonIdx)) goto error;
        if (!CLTableGetValueInt32(Table, "InstanceLimit", RowIndex, &Data->InstanceLimit)) goto error;
        if (!CLTableGetValueInt32(Table, "Level", RowIndex, &Data->Level)) goto error;
        if (!CLTableGetValueInt32(Table, "MaxUser", RowIndex, &Data->MaxUser)) goto error;
        if (!CLTableGetValueInt32(Table, "MissionTimeout", RowIndex, &Data->MissionTimeout)) goto error;
        //if (!CLTableGetValueInt32(Table, "OpenItem", RowIndex, &Data->OpenItem)) goto error;
        if (!CLTableGetValueInt32(Table, "PPLink", RowIndex, &Data->PPLink)) goto error;
        if (!CLTableGetValueInt32(Table, "Penalty", RowIndex, &Data->Penalty)) goto error;
        //if (!CLTableGetValueInt32(Table, "Reward", RowIndex, &Data->Reward)) goto error;
        if (!CLTableGetValueInt32(Table, "WarpIdx", RowIndex, &Data->WarpIdx)) goto error;
        if (!CLTableGetValueInt32(Table, "WarpIdxForSucess", RowIndex, &Data->WarpIdxForSucess)) goto error;
        if (!CLTableGetValueInt32(Table, "WarpIdxForFail", RowIndex, &Data->WarpIdxForFail)) goto error;
        if (!CLTableGetValueInt32(Table, "WarpIndexForDead", RowIndex, &Data->WarpIndexForDead)) goto error;
        if (!CLTableGetValueInt32(Table, "WorldIdx", RowIndex, &Data->WorldIdx)) goto error;
        if (!CLTableGetValueInt32(Table, "bAddRange", RowIndex, &Data->bAddRange)) goto error;
        if (!CLTableGetValueInt32(Table, "NextQDIdxForSuccess", RowIndex, &Data->NextQDIdxForSuccess)) goto error;
        if (!CLTableGetValueInt32(Table, "WarpNPC_Set", RowIndex, &Data->WarpNPC_Set)) goto error;
        if (!CLTableGetValueInt32(Table, "UseTerrain", RowIndex, &Data->UseTerrain)) goto error;
        //if (!CLTableGetValueInt32(Table, "BattleStyle", RowIndex, &Data->BattleStyle)) goto error;
        if (!CLTableGetValueInt32(Table, "UseOddCircle_Count", RowIndex, &Data->UseOddCircle_Count)) goto error;
        if (!CLTableGetValueInt32(Table, "Party_Type", RowIndex, &Data->Party_Type)) goto error;
        if (!CLTableGetValueInt32(Table, "RemoveItem", RowIndex, &Data->RemoveItem)) goto error;
        if (!CLTableGetValueInt32(Table, "DBWrite", RowIndex, &Data->DBWrite)) goto error;
        if (!CLTableGetValueInt32(Table, "DungeonType", RowIndex, &Data->DungeonType)) goto error;
        if (!CLTableGetValueInt32(Table, "UseVeradrix_Count", RowIndex, &Data->UseVeradrix_Count)) goto error;
        if (!CLTableGetValueInt32(Table, "Cool_Time", RowIndex, &Data->Cool_Time)) goto error;
        if (!CLTableGetValueInt32(Table, "Reentry", RowIndex, &Data->Reentry)) goto error;
    }
    /*
    struct _PatternPartData {
        [PatternPart] PPIdx	MissionMobs	MissionNPC
    };

    struct _TimeControlData {
        [TimeControl] Dungeon_Index	MMAP_Index	Event	Time_Value
    };

    struct _ArenaDefenceData {
        [ArenaDefence] Dungeon_ID	MobIdx	Goal_X	Goal_Y	SpeedType
    };

    struct _DungeonPointRewardData {
        [DP_Table] Dungeon_Index	DP
    };

    struct _MobMapData {
        [MMap] MobIdx	PPIdx	SpeciesIdx	PosX	PosY	Width	Height	SpwnInterval	SpwnCount	SpawnDefault	EvtProperty	EvtMobs	EvtInterval	Grade	Lv	MissionGate	PerfectDrop	TrgIdxSpawn	TrgIdxKill	Type	Min	Max	Authority	Server_Mob	Loot_Delay	Enemies
    };

    struct _TriggerData {
        [Trg2Evt] QDungeonIdx	TrgIdx	Order	TrgType	LiveStateMMapIdx	DeadStateMMapIdx	TrgNpcIdx	EvtActGroupIdx
    };

    struct _ActionGroupData {
        [ActGroup] QDungeonIdx	EvtActGroupIdx	Order	TgtMMapIdx	TgtAction	EvtDelay	TgtSpawnInterval	TgtSpawnCount
    };

    struct _MissionMobMapData {
        [MissionMMAP] QDungeonIdx	MMapidx	Cells
    };

    struct _ClearConditionData {
        [ClearCondition] QDungeonIdx	MobIdx
    };

    struct _StartKillData {
        [StartKill] QDungeonIdx	MobIdx
    };

    struct _MissionDungeonTimerData {
        [MD_Timer] Dungeon_Index	MMAP_Event	MMAP_Index	Condition	Time_Value
    };

    struct _MissionDungeonFlowCheckData {
        [MD_FlowCheck] Dungeon_Index	Dead_MMAP	Kill_MMAP
    };

    struct _MissionDungeonTimerValueData {
        [MD_Timer_Value] Dungeon_Index	Item_Index	Mmap_Index
    };

    struct _ClientImmuneData {
        [ClientImmune] QDungeonIdx	MobIdx	ImmuneMobIdx
    };
    */
    return;

error:
    if (TableSet) CLTableSetDestroy(TableSet);
}
