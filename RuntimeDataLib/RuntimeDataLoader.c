#include "RuntimeDataLoader.h"

RTRuntimeDataContextRef RTRuntimeDataContextCreate() {
	RTRuntimeDataContextRef Context = malloc(sizeof(struct _RTRuntimeDataContext));
	if (!Context) FatalError("Memory allocation failed!");

	memset(Context, 0, sizeof(struct _RTRuntimeDataContext));
	return Context;
}

Void RTRuntimeDataContextDestroy(
	RTRuntimeDataContextRef Context
) {
	assert(Context);
	free(Context);
}

Bool RTRuntimeDataContextLoad(
	RTRuntimeDataContextRef Context,
    CString RuntimeDataPath,
    CString ServerDataPath
) {
    assert(Context);

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    ArchiveRef Archive = ArchiveCreateEmpty(Allocator);
    Bool IgnoreErrors = true;
    ArchiveIteratorRef Iterator = NULL;
    Bool Success = false;

#define RUNTIME_DATA_FILE_BEGIN(__NAME__) \
    LogMessageFormat(LOG_LEVEL_INFO, "Loading runtime data: %s", # __NAME__); \
    if (CStringIsEqual(PathGetFileNameExtension(# __NAME__), "xml")) { \
        Success = ArchiveLoadFromFile( \
            Archive, \
            PathCombineNoAlloc(ServerDataPath, # __NAME__), \
            IgnoreErrors \
        ); \
    } \
    else { \
        Success = ArchiveLoadFromFileEncryptedNoAlloc( \
            Archive, \
            PathCombineNoAlloc(RuntimeDataPath, # __NAME__), \
            IgnoreErrors \
        ); \
    } \
    if (!Success) { \
        LogMessageFormat(LOG_LEVEL_ERROR, "Error loading runtime data: %s", # __NAME__); \
        goto error; \
    } 

#define RUNTIME_DATA_FILE_END \
    ArchiveClear(Archive, true);

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__) \
    { \
        LogMessageFormat(LOG_LEVEL_INFO, "Data %s memory size %llu", #__NAME__, sizeof(struct CONCAT(_RTData, __NAME__))); \
        CString Query = __QUERY__; \
        CString PropertyQuery = NULL; \
        Iterator = ArchiveQueryNodeIteratorByPathFirst(Archive, -1, Query); \
        while (Iterator) { \
            assert(Context->CONCAT(__NAME__, Count) < __COUNT__); \
            CONCAT(RTData, __NAME__ ## Ref) Data = &Context->CONCAT(__NAME__, List)[Context->CONCAT(__NAME__, Count)];

#define RUNTIME_DATA_PROPERTY(__TYPE__, __NAME__, __QUERY__) \
            PropertyQuery = __QUERY__; \
            if (!CONCAT(ParseAttribute, __TYPE__)(Archive, Iterator->Index, PropertyQuery, &Data->__NAME__)) { \
                LogMessageFormat(LOG_LEVEL_ERROR, "Couldn't parse attribute %s in %s\n", PropertyQuery, Query); \
                goto error; \
            }

#define RUNTIME_DATA_PROPERTY_ARRAY(__TYPE__, __NAME__, __QUERY__, __COUNT__, __SEPARATOR__) \
            PropertyQuery = __QUERY__; \
            if (!CONCAT(ParseAttribute, __TYPE__ ## Array)(Archive, Iterator->Index, PropertyQuery, Data->__NAME__, __COUNT__, __SEPARATOR__)) { \
                LogMessageFormat(LOG_LEVEL_ERROR, "Couldn't parse attribute %s in %s\n", PropertyQuery, Query); \
                goto error; \
            }

#define RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__, __COUNT__) \
            { \
                CString Query = __QUERY__; \
                ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, Query); \
                while (ChildIterator) { \
                    ArchiveIteratorRef Iterator = ChildIterator; \
                    assert(Data->CONCAT(__NAME__, Count) < __COUNT__); \
                    CONCAT(RTData, __NAME__ ## Ref) ChildData = &Data->CONCAT(__NAME__, List)[Data->CONCAT(__NAME__, Count)]; \
                    { \
                        CONCAT(RTData, __NAME__ ## Ref) Data = ChildData;

#define RUNTIME_DATA_TYPE_END_CHILD(__NAME__, __COUNT__) \
                    } \
                    Data->CONCAT(__NAME__, Count) += 1; \
                    ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator); \
                } \
            }

#define RUNTIME_DATA_TYPE_END(__NAME__) \
           Context->CONCAT(__NAME__, Count) += 1; \
           Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator); \
        } \
    }

#include "Macro.h"

    ArchiveDestroy(Archive);
    return true;

error:
    ArchiveDestroy(Archive);
    return false;
}

#define RUNTIME_DATA_TYPE_INDEX(__NAME__, __TYPE__, __FIELD__) \
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData, __NAME__ ## Get)( \
	RTRuntimeDataContextRef Context, \
	__TYPE__ __FIELD__ \
) { \
    for (Int32 _Index = 0; _Index < Context->CONCAT(__NAME__, Count); _Index++) { \
        CONCAT(RTData, __NAME__ ## Ref) Data = &Context->CONCAT(__NAME__, List)[_Index]; \
        if (Data->__FIELD__ == __FIELD__) { \
            return Data; \
        } \
    } \
 \
    return NULL; \
}

#define RUNTIME_DATA_TYPE_INDEX_CHILD(__PARENT__, __NAME__, __TYPE__, __FIELD__)        \
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData, __NAME__ ## Get)(			        \
	CONCAT(RTData, __PARENT__ ## Ref) Parent,										    \
	__TYPE__ __FIELD__															        \
) {                                                                                     \
    for (Int32 _Index = 0; _Index < Parent->CONCAT(__NAME__, Count); _Index++) {        \
        CONCAT(RTData, __NAME__ ## Ref) Data = &Parent->CONCAT(__NAME__, List)[_Index]; \
        if (Data->__FIELD__ == __FIELD__) {                                             \
            return Data;                                                                \
        }                                                                               \
    }                                                                                   \
                                                                                        \
    return NULL;                                                                        \
}

#define RUNTIME_DATA_TYPE_INDEX_SINGLE(__NAME__)				\
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData, __NAME__ ## Get)( \
	RTRuntimeDataContextRef Context                             \
) { \
    assert(Context->CONCAT(__NAME__, Count) == 1); \
    return &Context->CONCAT(__NAME__, List)[0]; \
}

#define RUNTIME_DATA_TYPE_INDEX_RANGE(__NAME__, __TYPE__, __LOWER_FIELD__, __UPPER_FIELD__) \
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData, __NAME__ ## Get)(						\
	RTRuntimeDataContextRef Context,														\
	__TYPE__ Value																            \
) {                                                                                         \
    for (Int32 _Index = 0; _Index < Context->CONCAT(__NAME__, Count); _Index++) {           \
        CONCAT(RTData, __NAME__ ## Ref) Data = &Context->CONCAT(__NAME__, List)[_Index];    \
        if (Data->__LOWER_FIELD__ <= Value && Value <= Data->__UPPER_FIELD__) {             \
            return Data;                                                                    \
        }                                                                                   \
    }                                                                                       \
                                                                                            \
    return NULL;                                                                            \
}

#include "Macro.h"

Int32 RTRuntimeDataCharacterRankUpConditionGet(
    RTRuntimeDataContextRef Context,
    Int32 Rank,
    Int32 BattleStyleIndex
) {
    for (Int32 RankIndex = 0; RankIndex < Context->CharacterRankUpConditionCount; RankIndex += 1) {
        if (Context->CharacterRankUpConditionList[RankIndex].Rank != Rank) continue;

        assert(Context->CharacterRankUpConditionList[RankIndex].ConditionCount == 1);

        switch (BattleStyleIndex) {
        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].WA;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].BL;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].WI;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].FA;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].FS;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].FB;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].GL;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].FG;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].DM;

        default:
            FatalError("Given BattleStyleIndex doesn't exist!");
            return 0;
        }
    }

    return 0;
}

RTDataUpgradeItemGradeRef RTRuntimeDataUpgradeItemGradeGet(
    RTDataUpgradeItemRef UpgradeItem,
    Int32 Grade
) {
    for (Int32 Index = 0; Index < UpgradeItem->UpgradeItemGradeCount; Index += 1) {
        RTDataUpgradeItemGradeRef UpgradeItemGrade = &UpgradeItem->UpgradeItemGradeList[Index];
        if (UpgradeItemGrade->MinGrade <= Grade && Grade <= UpgradeItemGrade->MaxGrade) {
            return UpgradeItemGrade;
        }
    }

    return NULL;
}

RTDataUpgradeItemBasicGradeRef RTRuntimeDataUpgradeItemBasicGradeGet(
    RTDataUpgradeItemBasicRef UpgradeItem,
    Int32 Grade
) {
    for (Int32 Index = 0; Index < UpgradeItem->UpgradeItemBasicGradeCount; Index += 1) {
        RTDataUpgradeItemBasicGradeRef UpgradeItemGrade = &UpgradeItem->UpgradeItemBasicGradeList[Index];
        if (UpgradeItemGrade->MinGrade <= Grade && Grade <= UpgradeItemGrade->MaxGrade) {
            return UpgradeItemGrade;
        }
    }

    return NULL;
}

Bool ParseAttributeRTDataItemType(
    ArchiveRef Object,
    Int64 NodeIndex,
    CString Name,
    RTDataItemType* Result
) {
    Int64 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (strcmp(Data->Data, "MG") == 0) {
        *Result = RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    }
    else if (strcmp(Data->Data, "1H") == 0) {
        *Result = RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND;
    }
    else if (strcmp(Data->Data, "2H") == 0) {
        *Result = RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND;
    }
    else if (strcmp(Data->Data, "SUIT") == 0) {
        *Result = RUNTIME_ITEM_TYPE_SUIT;
    }
    else if (strcmp(Data->Data, "GLOVE") == 0) {
        *Result = RUNTIME_ITEM_TYPE_GLOVES;
    }
    else if (strcmp(Data->Data, "BOOT") == 0) {
        *Result = RUNTIME_ITEM_TYPE_BOOTS;
    }
    else if (strcmp(Data->Data, "HELM") == 0) {
        *Result = RUNTIME_ITEM_TYPE_HELMED2; // TODO: What should we do with RUNTIME_ITEM_TYPE_HELMED1?
    }
    else if (strcmp(Data->Data, "BIKE") == 0) {
        *Result = RUNTIME_ITEM_TYPE_VEHICLE_BIKE;
    }
    else if (strcmp(Data->Data, "CHAKRAM") == 0) {
        *Result = RUNTIME_ITEM_TYPE_CHAKRAM;
    }
    else {
        goto error;
    }

    return true;

error:
    return false;
}
