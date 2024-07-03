#include "RuntimeDataLoader.h"

RTRuntimeDataContextRef RTRuntimeDataContextCreate() {
	RTRuntimeDataContextRef Context = malloc(sizeof(struct _RTRuntimeDataContext));
	if (!Context) Fatal("Memory allocation failed!");

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
    Info("Loading runtime data: %s", # __NAME__); \
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
        Error("Error loading runtime data: %s", # __NAME__); \
        goto error; \
    } 

#define RUNTIME_DATA_FILE_END \
    ArchiveClear(Archive, true);

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__) \
    { \
        Info("Data %s memory size %llu", #__NAME__, sizeof(struct CONCAT(_RTData, __NAME__))); \
        CString Query = __QUERY__; \
        CString PropertyQuery = NULL; \
        Iterator = ArchiveQueryNodeIteratorByPathFirst(Archive, -1, Query); \
        while (Iterator) { \
            assert(Context->CONCAT(__NAME__, Count) < __COUNT__); \
            CONCAT(RTData, __NAME__ ## Ref) Data = &Context->CONCAT(__NAME__, List)[Context->CONCAT(__NAME__, Count)];

#define RUNTIME_DATA_PROPERTY(__TYPE__, __NAME__, __QUERY__) \
            PropertyQuery = __QUERY__; \
            if (!CONCAT(ParseAttribute, __TYPE__)(Archive, Iterator->Index, PropertyQuery, &Data->__NAME__)) { \
                Error("Couldn't parse attribute %s in %s\n", PropertyQuery, Query); \
            }

#define RUNTIME_DATA_PROPERTY_PRECONDITION(__TYPE__, __NAME__, __QUERY__, __VALUE__) \
            PropertyQuery = __QUERY__; \
            if (!CONCAT(ParseAttribute, __TYPE__ ## Equal)(Archive, Iterator->Index, PropertyQuery, __VALUE__)) { \
                Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator); \
                continue; \
            }

#define RUNTIME_DATA_PROPERTY_ARRAY(__TYPE__, __NAME__, __QUERY__, __COUNT__, __SEPARATOR__) \
            PropertyQuery = __QUERY__; \
            Data->CONCAT(__NAME__, Count) = CONCAT(ParseAttribute, __TYPE__ ## ArrayCounted)(Archive, Iterator->Index, PropertyQuery, Data->__NAME__, __COUNT__, __SEPARATOR__);

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

#define RUNTIME_DATA_TYPE_INDEX_SUFFIXED(__NAME__, __SUFFIX__, __TYPE__, __FIELD__) \
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData ## __NAME__, Get ## __SUFFIX__)( \
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
            Fatal("Given BattleStyleIndex doesn't exist!");
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

RTDataNewbieSupportCategoryRewardRef RTRuntimeDataNewbieSupportCategoryRewardGet(
    RTDataNewbieSupportCategoryRef Category,
    Int32 ConditionValue1,
    Int32 ConditionValue2
) {
    for (Int32 Index = 0; Index < Category->NewbieSupportCategoryRewardCount; Index += 1) {
        RTDataNewbieSupportCategoryRewardRef Reward = &Category->NewbieSupportCategoryRewardList[Index];
        if (Reward->ConditionValue1 == ConditionValue1 && Reward->ConditionValue2 == ConditionValue2) {
            return Reward;
        }
    }

    return NULL;
}

RTDataDivineUpgradeMainRef RTRuntimeDataDivineUpgradeMainGet(
    RTRuntimeDataContextRef Context,
    Int32 ItemGrade,
    Int32 ItemType
) {
    for (Int32 Index = 0; Index < Context->DivineUpgradeMainCount; Index += 1) {
        RTDataDivineUpgradeMainRef Main = &Context->DivineUpgradeMainList[Index];
        if ((Main->ItemGrade == ItemGrade || Main->ItemGrade < 0) && Main->ItemType == ItemType) return Main;
    }

    return NULL;
}

RTDataDummySkillRef RTRuntimeDataDummySkillGet(
    RTRuntimeDataContextRef Context,
    Int32 Case,
    Int32 Property
) {
    for (Int32 Index = 0; Index < Context->DummySkillCount; Index += 1) {
        RTDataDummySkillRef DummySkill = &Context->DummySkillList[Index];
        if (DummySkill->Case != Case) continue;
        if (DummySkill->Property != Property) continue;

        return DummySkill;
    }

    return NULL;
}

RTDataForceCoreBaseRef RTRuntimeDataForceCoreBaseGet(
    RTRuntimeDataContextRef Context,
    Int32 ItemGrade,
    Int32 ItemType
) {
    for (Int32 Index = 0; Index < Context->ForceCoreBaseCount; Index += 1) {
        RTDataForceCoreBaseRef Main = &Context->ForceCoreBaseList[Index];
        if ((Main->ItemGrade == ItemGrade || Main->ItemGrade < 0) && Main->ItemType == ItemType) return Main;
    }

    return NULL;
}

RTDataForceCodeCostRef RTRuntimeDataForceCodeCostGet(
    RTRuntimeDataContextRef Context,
    Int32 CostGrade,
    Int32 FilledSlotCount
) {
    for (Int32 Index = 0; Index < Context->ForceCodeCostCount; Index += 1) {
        RTDataForceCodeCostRef Cost = &Context->ForceCodeCostList[Index];
        if (Cost->CostGrade == CostGrade && Cost->FilledSlotCount == FilledSlotCount) return Cost;
    }

    return NULL;
}

RTDataForceCodeRateRef RTRuntimeDataForceCodeRateGet(
    RTRuntimeDataContextRef Context,
    Int32 ItemType,
    Int32 FilledSlotCount,
    Int32 ItemUpgradeLevel
) {
    for (Int32 Index = 0; Index < Context->ForceCodeRateCount; Index += 1) {
        RTDataForceCodeRateRef Rate = &Context->ForceCodeRateList[Index];
        if (Rate->ItemType == ItemType && Rate->FilledSlotCount == FilledSlotCount && Rate->ItemUpgradeLevel == ItemUpgradeLevel) return Rate;
    }

    return NULL;
}

RTDataUpgradeGradeChangeRef RTRuntimeDataUpgradeGradeChangeGet(
    RTRuntimeDataContextRef Context,
    Int32 ItemType,
    Int32 ItemGrade
) {
    for (Int32 Index = 0; Index < Context->UpgradeGradeChangeCount; Index += 1) {
        RTDataUpgradeGradeChangeRef Change = &Context->UpgradeGradeChangeList[Index];
        if (Change->ItemType == ItemType && Change->ItemGrade == ItemGrade) return Change;
    }

    return NULL;
}

// TODO: "MG" is not inside of forcecore_option so force controller weapons will not work!
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

    struct { Char Key[MAX_PATH]; RTDataItemType Value; } Dictionary[] = {
        { "MG", RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER },
        { "1H", RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND },
        { "2H", RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND },
        { "SUIT", RUNTIME_ITEM_TYPE_SUIT },
        { "GLOVE", RUNTIME_ITEM_TYPE_GLOVES },
        { "BOOT", RUNTIME_ITEM_TYPE_BOOTS },
        { "HELM", RUNTIME_ITEM_TYPE_HELMED1 },
        { "BIKE", RUNTIME_ITEM_TYPE_VEHICLE_BIKE },
        { "CHAKRAM", RUNTIME_ITEM_TYPE_CHAKRAM },
        { "AMULET", RUNTIME_ITEM_TYPE_AMULET },
        { "BELT", RUNTIME_ITEM_TYPE_BELT },
        { "BIKE_5", RUNTIME_ITEM_TYPE_VEHICLE_BIKE },
        { "BIKE_8", RUNTIME_ITEM_TYPE_VEHICLE_BIKE },
        { "BIKE_11", RUNTIME_ITEM_TYPE_VEHICLE_BIKE },
        { "BIKE_14", RUNTIME_ITEM_TYPE_VEHICLE_BIKE },
        { "BOARD", RUNTIME_ITEM_TYPE_VEHICLE_BOARD },
        { "BRACELET_1", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_2", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_3", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_4", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_5", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_6", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_7", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_8", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_9", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_10", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_11", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_12", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_13", RUNTIME_ITEM_TYPE_BRACELET },
        { "BRACELET_14", RUNTIME_ITEM_TYPE_BRACELET },
        { "BROOCH", RUNTIME_ITEM_TYPE_BROOCH },
        { "CHARM", RUNTIME_ITEM_TYPE_CHARM },
        { "EMBLEM_1", RUNTIME_ITEM_TYPE_CREST },
        { "EMBLEM_2", RUNTIME_ITEM_TYPE_CREST },
        { "EMBLEM_3", RUNTIME_ITEM_TYPE_CREST },
        { "EARRING_1", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_2", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_3", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_4", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_5", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_6", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_7", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_8", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_9", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_10", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_11", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_12", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_13", RUNTIME_ITEM_TYPE_EARRING },
        { "EARRING_14", RUNTIME_ITEM_TYPE_EARRING },
        { "EPULET", RUNTIME_ITEM_TYPE_EPAULET },
        { "RING", RUNTIME_ITEM_TYPE_RING },
        { "DivineToken", RUNTIME_ITEM_TYPE_DIVINE_TOKEN },
        { "UNIQUEEMBLEM", RUNTIME_ITEM_TYPE_ARTIFACT },
    };

    Int32 Count = sizeof(Dictionary) / sizeof(Dictionary[0]);
    for (Int32 Index = 0; Index < Count; Index += 1) {
        if (strcmp(Data->Data, Dictionary[Index].Key) == 0) {
            *Result = Dictionary[Index].Value;
            return true;
        }
    }
    
error:
    return false;
}

Bool ParseAttributeRTDataItemTypeGrade(
    ArchiveRef Object,
    Int64 NodeIndex,
    CString Name,
    RTDataItemTypeGrade* Result
) {
    Int64 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    struct { Char Key[MAX_PATH]; RTDataItemTypeGrade Value; } Dictionary[] = {
        { "MG", -1 },
        { "1H", -1 },
        { "2H", -1 },
        { "SUIT", -1 },
        { "GLOVE", -1 },
        { "BOOT", -1 },
        { "HELM", -1 },
        { "BIKE", -1 },
        { "CHAKRAM", -1 },
        { "AMULET", -1 },
        { "BELT", -1 },
        { "BIKE_5", 5 },
        { "BIKE_8", 8 },
        { "BIKE_11", 11 },
        { "BIKE_14", 14 },
        { "BOARD", -1 },
        { "BRACELET_1", 1 },
        { "BRACELET_2", 2 },
        { "BRACELET_3", 3 },
        { "BRACELET_4", 4 },
        { "BRACELET_5", 5 },
        { "BRACELET_6", 6 },
        { "BRACELET_7", 7 },
        { "BRACELET_8", 8 },
        { "BRACELET_9", 9 },
        { "BRACELET_10", 10 },
        { "BRACELET_11", 11 },
        { "BRACELET_12", 12 },
        { "BRACELET_13", 13 },
        { "BRACELET_14", 14 },
        { "BROOCH", -1 },
        { "CHARM", -1 },
        { "EMBLEM_1", 1 },
        { "EMBLEM_2", 2 },
        { "EMBLEM_3", 3 },
        { "EARRING_1", 1 },
        { "EARRING_2", 2 },
        { "EARRING_3", 3 },
        { "EARRING_4", 4 },
        { "EARRING_5", 5 },
        { "EARRING_6", 6 },
        { "EARRING_7", 7 },
        { "EARRING_8", 8 },
        { "EARRING_9", 9 },
        { "EARRING_10", 10 },
        { "EARRING_11", 11 },
        { "EARRING_12", 12 },
        { "EARRING_13", 13 },
        { "EARRING_14", 14 },
        { "EPULET", -1 },
        { "RING", -1 },
        { "DivineToken", -1 },
        { "UNIQUEEMBLEM", -1 },
    };

    Int32 Count = sizeof(Dictionary) / sizeof(Dictionary[0]);
    for (Int32 Index = 0; Index < Count; Index += 1) {
        if (strcmp(Data->Data, Dictionary[Index].Key) == 0) {
            *Result = Dictionary[Index].Value;
            return true;
        }
    }

error:
    return false;
}

Bool ParseAttributeRTDataItemCategory(
    ArchiveRef Object,
    Int64 NodeIndex,
    CString Name,
    RTDataItemCategory* Result
) {
    Int64 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (strcmp(Data->Data, "MG") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_WEAPON_FORCE_CONTROLLER;
    }
    else if (strcmp(Data->Data, "1H") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_WEAPON_ONE_HAND;
    }
    else if (strcmp(Data->Data, "2H") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_WEAPON_TWO_HAND;
    }
    else if (strcmp(Data->Data, "SUIT") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_SUIT;
    }
    else if (strcmp(Data->Data, "GLOVE") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_GLOVES;
    }
    else if (strcmp(Data->Data, "BOOT") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_BOOTS;
    }
    else if (strcmp(Data->Data, "HELM") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_HELMED;
    }
    else if (strcmp(Data->Data, "BIKE") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_VEHICLE_BIKE;
    }
    else if (strcmp(Data->Data, "CHAKRAM") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_CHAKRAM;
    }
    else if (strcmp(Data->Data, "DivineToken") == 0) {
        *Result = RUNTIME_ITEM_CATEGORY_TOKEN;
    }
    else {
        goto error;
    }

    return true;

error:
    return false;
}

Bool ParseAttributeRTDataItemGroup(
    ArchiveRef Object,
    Int64 NodeIndex,
    CString Name,
    RTDataItemGroup* Result
) {
    Int64 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (strcmp(Data->Data, "MG") == 0) {
        *Result = RUNTIME_ITEM_GROUP_WEAPON_ONE_HAND;
    }
    else if (strcmp(Data->Data, "1H") == 0) {
        *Result = RUNTIME_ITEM_GROUP_WEAPON_ONE_HAND;
    }
    else if (strcmp(Data->Data, "2H") == 0) {
        *Result = RUNTIME_ITEM_GROUP_WEAPON_TWO_HAND;
    }
    else if (strcmp(Data->Data, "SUIT") == 0) {
        *Result = RUNTIME_ITEM_GROUP_ARMOR;
    }
    else if (strcmp(Data->Data, "GLOVE") == 0) {
        *Result = RUNTIME_ITEM_GROUP_ARMOR;
    }
    else if (strcmp(Data->Data, "BOOT") == 0) {
        *Result = RUNTIME_ITEM_GROUP_ARMOR;
    }
    else if (strcmp(Data->Data, "HELM") == 0) {
        *Result = RUNTIME_ITEM_GROUP_ARMOR;
    }
    else if (strcmp(Data->Data, "BIKE") == 0) {
        *Result = RUNTIME_ITEM_GROUP_VEHICLE;
    }
    else if (strcmp(Data->Data, "CHAKRAM") == 0) {
        *Result = RUNTIME_ITEM_GROUP_WEAPON_ONE_HAND;
    }
    else {
        goto error;
    }

    return true;

error:
    return false;
}

RTDataForceCodeFormula RTRuntimeDataForceCodeFormulaGet(
    RTRuntimeDataContextRef Context,
    RTDataItemType ItemType,
    Int32 ItemGrade,
    Int32 ItemUniqueGrade,
    Int32 ForceIndex,
    Int32 ForceLevel,
    Bool IsExtended,
    Bool IsEpic
) {
    assert(ItemGrade < 16);

    RTDataForceCodeFormula Formula = { 0 };

    Bool IsOneHandedWeapon = (
        ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND ||
        ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER ||
        ItemType == RUNTIME_ITEM_TYPE_CHAKRAM
    );
    Int32 ForceCoreItemType = IsOneHandedWeapon ? RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND : ItemType;

    RTDataForceCoreBaseRef ForceCoreBase = RTRuntimeDataForceCoreBaseGet(Context, ItemGrade, ForceCoreItemType);
    if (!ForceCoreBase) return Formula;

    Int32 ForceEffectIndex = 0;
    for (Int32 Index = 0; Index < ForceCoreBase->ForceCoreBaseCodeCount; Index += 1) {
        if (ForceCoreBase->ForceCoreBaseCodeList[Index].ForceIndex != ForceIndex) continue;

        ForceEffectIndex = ForceCoreBase->ForceCoreBaseCodeList[Index].ForceEffectIndex;
        break;
    }

    if (ForceEffectIndex < 1) return Formula;

    Bool IsEquipment = (
        ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER ||
        ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND ||
        ItemType == RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND ||
        ItemType == RUNTIME_ITEM_TYPE_SUIT ||
        ItemType == RUNTIME_ITEM_TYPE_GLOVES ||
        ItemType == RUNTIME_ITEM_TYPE_BOOTS ||
        ItemType == RUNTIME_ITEM_TYPE_HELMED1 ||
        ItemType == RUNTIME_ITEM_TYPE_HELMED2 ||
        ItemType == RUNTIME_ITEM_TYPE_CHAKRAM ||
        // TODO: Check which table itemtype RUNTIME_ITEM_TYPE_VEHICLE_BOARD refers to.
        ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BOARD
    );

    if (IsEquipment && IsEpic) {
        for (Int32 Index = 0; Index < Context->EpicCodeEquipmentCount; Index += 1) {
            RTDataEpicCodeEquipmentRef EpicCode = &Context->EpicCodeEquipmentList[Index];
            if (EpicCode->ForceEffectIndex != ForceEffectIndex) continue;
            if (EpicCode->ForceLevel != ForceLevel) continue;

            Formula.ForceEffectIndex = ForceEffectIndex;
            Formula.ForceLevel = ForceLevel;

            if (ItemUniqueGrade > 0) {
                assert(ItemUniqueGrade < 3);

                Int32 ForceValues[] = {
                    0,
                    EpicCode->UniqueForceValueGrade1,
                    EpicCode->UniqueForceValueGrade2,
                };

                Formula.ForceValue = ForceValues[ItemUniqueGrade];
            }
            else {
                Int32 ForceValues[] = {
                    0,
                    1,
                    2,
                    3,
                    EpicCode->ForceValueGrade4,
                    EpicCode->ForceValueGrade5,
                    EpicCode->ForceValueGrade6,
                    EpicCode->ForceValueGrade7,
                    EpicCode->ForceValueGrade8,
                    EpicCode->ForceValueGrade9,
                    EpicCode->ForceValueGrade10,
                    EpicCode->ForceValueGrade11,
                    EpicCode->ForceValueGrade12,
                    EpicCode->ForceValueGrade13,
                    EpicCode->ForceValueGrade14,
                    EpicCode->ForceValueGrade15
                };

                Formula.ForceValue = ForceValues[ItemGrade];
            }

            return Formula;
        }

        return Formula;
    }

    if (IsEquipment && IsExtended) {
        for (Int32 Index = 0; Index < Context->ExtendedForceCodeEquipmentCount; Index += 1) {
            RTDataExtendedForceCodeEquipmentRef ForceCode = &Context->ExtendedForceCodeEquipmentList[Index];
            if (ForceCode->ForceEffectIndex != ForceEffectIndex) continue;
            if (ForceCode->ForceLevel != ForceLevel) continue;

            Formula.ForceEffectIndex = ForceEffectIndex;
            Formula.ForceLevel = ForceLevel;

            Int32 ForceValues[] = {
                0,
                ForceCode->ForceValueGrade1,
                ForceCode->ForceValueGrade2,
                ForceCode->ForceValueGrade3,
                ForceCode->ForceValueGrade4,
                ForceCode->ForceValueGrade5,
                ForceCode->ForceValueGrade6,
                ForceCode->ForceValueGrade7,
                ForceCode->ForceValueGrade8,
                ForceCode->ForceValueGrade9,
                ForceCode->ForceValueGrade10,
                ForceCode->ForceValueGrade11,
                ForceCode->ForceValueGrade12,
                ForceCode->ForceValueGrade13,
                ForceCode->ForceValueGrade14,
                ForceCode->ForceValueGrade15
            };

            Formula.ForceValue = ForceValues[ItemGrade];

            return Formula;
        }

        return Formula;
    }

    if (IsEquipment) {
        for (Int32 Index = 0; Index < Context->ForceCodeEquipmentCount; Index += 1) {
            RTDataExtendedForceCodeEquipmentRef ForceCode = &Context->ForceCodeEquipmentList[Index];
            if (ForceCode->ForceEffectIndex != ForceEffectIndex) continue;
            if (ForceCode->ForceLevel != ForceLevel) continue;

            Formula.ForceEffectIndex = ForceEffectIndex;
            Formula.ForceLevel = ForceLevel;

            Int32 ForceValues[] = {
                0,
                ForceCode->ForceValueGrade1,
                ForceCode->ForceValueGrade2,
                ForceCode->ForceValueGrade3,
                ForceCode->ForceValueGrade4,
                ForceCode->ForceValueGrade5,
                ForceCode->ForceValueGrade6,
                ForceCode->ForceValueGrade7,
                ForceCode->ForceValueGrade8,
                ForceCode->ForceValueGrade9,
                ForceCode->ForceValueGrade10,
                ForceCode->ForceValueGrade11,
                ForceCode->ForceValueGrade12,
                ForceCode->ForceValueGrade13,
                ForceCode->ForceValueGrade14,
                ForceCode->ForceValueGrade15
            };

            Formula.ForceValue = ForceValues[ItemGrade];

            return Formula;
        }

        return Formula;
    }

    Bool IsAccessory = !IsEquipment;
    if (IsAccessory && IsEpic) {
        for (Int32 Index = 0; Index < Context->EpicCodeAccessoryCount; Index += 1) {
            RTDataEpicCodeAccessoryRef EpicCode = &Context->EpicCodeAccessoryList[Index];
            if (EpicCode->ForceEffectIndex != ForceEffectIndex) continue;
            if (EpicCode->ForceLevel != ForceLevel) continue;

            Formula.ForceEffectIndex = ForceEffectIndex;
            Formula.ForceLevel = ForceLevel;

            if (ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BIKE) {
                Int32 ForceValues[] = {
                    0,
                    1,
                    2,
                    3,
                    4,
                    EpicCode->VehicleForceValueGrade5,
                    0,
                    0,
                    EpicCode->VehicleForceValueGrade8,
                    0,
                    0,
                    EpicCode->VehicleForceValueGrade11,
                    0,
                    0,
                    EpicCode->VehicleForceValueGrade14,
                    0
                };

                Formula.ForceValue = ForceValues[ItemGrade];
            }

            if (ItemType == RUNTIME_ITEM_TYPE_AMULET ||
                ItemType == RUNTIME_ITEM_TYPE_BELT ||
                ItemType == RUNTIME_ITEM_TYPE_BRACELET ||
                ItemType == RUNTIME_ITEM_TYPE_BROOCH ||
                ItemType == RUNTIME_ITEM_TYPE_CHARM ||
                ItemType == RUNTIME_ITEM_TYPE_EARRING ||
                ItemType == RUNTIME_ITEM_TYPE_RING) {
                assert(0 < ItemGrade && ItemGrade < 15);
                if (ItemUniqueGrade) {
                    Formula.ForceValue = EpicCode->UniqueAccessoryForceValueGrades[ItemGrade - 1];
                }
                else {
                    Formula.ForceValue = EpicCode->AccessoryForceValueGrades[ItemGrade - 1];
                }
            }

            if (ItemType == RUNTIME_ITEM_TYPE_CREST) {
                assert(ItemGrade < 4);
                Int32 ForceValues[] = {
                    0,
                    EpicCode->EmblemForceValueGrade1,
                    EpicCode->EmblemForceValueGrade2,
                    EpicCode->EmblemForceValueGrade3,
                };
                Formula.ForceValue = ForceValues[ItemGrade];
            }

            if (ItemType == RUNTIME_ITEM_TYPE_ARTIFACT) {
                Formula.ForceValue = EpicCode->UniqueEmblemForceValue;
            }

            return Formula;
        }

        return Formula;
    }

    if (IsAccessory && IsExtended) {
        for (Int32 Index = 0; Index < Context->ExtendedForceCodeAccessoryCount; Index += 1) {
            RTDataExtendedForceCodeAccessoryRef ForceCode = &Context->ExtendedForceCodeAccessoryList[Index];
            if (ForceCode->ForceEffectIndex != ForceEffectIndex) continue;
            if (ForceCode->ForceLevel != ForceLevel) continue;

            Formula.ForceEffectIndex = ForceEffectIndex;
            Formula.ForceLevel = ForceLevel;

            if (ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BIKE) {
                Int32 ForceValues[] = {
                    0,
                    1,
                    2,
                    3,
                    4,
                    ForceCode->VehicleForceValueGrade5,
                    0,
                    0,
                    ForceCode->VehicleForceValueGrade8,
                    0,
                    0,
                    ForceCode->VehicleForceValueGrade11,
                    0,
                    0,
                    ForceCode->VehicleForceValueGrade14,
                    0
                };

                Formula.ForceValue = ForceValues[ItemGrade];
            }

            if (ItemType == RUNTIME_ITEM_TYPE_EPAULET ||
                ItemType == RUNTIME_ITEM_TYPE_EPAULET2) {
                Formula.ForceValue = ForceCode->EpauletForceValue;
            }

            if (ItemType == RUNTIME_ITEM_TYPE_AMULET ||
                ItemType == RUNTIME_ITEM_TYPE_BELT ||
                ItemType == RUNTIME_ITEM_TYPE_BRACELET ||
                ItemType == RUNTIME_ITEM_TYPE_BROOCH ||
                ItemType == RUNTIME_ITEM_TYPE_CHARM ||
                ItemType == RUNTIME_ITEM_TYPE_EARRING ||
                ItemType == RUNTIME_ITEM_TYPE_RING) {
                assert(false && "Not supported itemtype!");
            }

            if (ItemType == RUNTIME_ITEM_TYPE_CREST) {
                assert(ItemGrade < 4);
                Int32 ForceValues[] = {
                    0,
                    ForceCode->EmblemForceValueGrade1,
                    ForceCode->EmblemForceValueGrade2,
                    ForceCode->EmblemForceValueGrade3,
                };
                Formula.ForceValue = ForceValues[ItemGrade];
            }

            if (ItemType == RUNTIME_ITEM_TYPE_ARTIFACT) {
                Formula.ForceValue = ForceCode->UniqueEmblemForceValue;
            }

            return Formula;
        }

        return Formula;
    }

    if (IsAccessory) {
        for (Int32 Index = 0; Index < Context->ForceCodeAccessoryCount; Index += 1) {
            RTDataForceCodeAccessoryRef ForceCode = &Context->ForceCodeAccessoryList[Index];
            if (ForceCode->ForceEffectIndex != ForceEffectIndex) continue;
            if (ForceCode->ForceLevel != ForceLevel) continue;

            Formula.ForceEffectIndex = ForceEffectIndex;
            Formula.ForceLevel = ForceLevel;

            if (ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BIKE) {
                Int32 ForceValues[] = {
                    0,
                    1,
                    2,
                    3,
                    4,
                    ForceCode->VehicleForceValueGrade5,
                    0,
                    0,
                    ForceCode->VehicleForceValueGrade8,
                    0,
                    0,
                    ForceCode->VehicleForceValueGrade11,
                    0,
                    0,
                    ForceCode->VehicleForceValueGrade14,
                    0
                };

                Formula.ForceValue = ForceValues[ItemGrade];
            }

            if (ItemType == RUNTIME_ITEM_TYPE_EPAULET ||
                ItemType == RUNTIME_ITEM_TYPE_EPAULET2) {
                Formula.ForceValue = ForceCode->EpauletForceValue;
            }

            if (ItemType == RUNTIME_ITEM_TYPE_AMULET ||
                ItemType == RUNTIME_ITEM_TYPE_BELT ||
                ItemType == RUNTIME_ITEM_TYPE_BRACELET ||
                ItemType == RUNTIME_ITEM_TYPE_BROOCH ||
                ItemType == RUNTIME_ITEM_TYPE_CHARM ||
                ItemType == RUNTIME_ITEM_TYPE_EARRING ||
                ItemType == RUNTIME_ITEM_TYPE_RING) {
                assert(0 < ItemGrade && ItemGrade < 15);
                Formula.ForceValue = ForceCode->AccessoryForceValueGrades[ItemGrade - 1];
            }

            if (ItemType == RUNTIME_ITEM_TYPE_CREST) {
                assert(ItemGrade < 4);
                Int32 ForceValues[] = {
                    0,
                    ForceCode->EmblemForceValueGrade1,
                    ForceCode->EmblemForceValueGrade2,
                    ForceCode->EmblemForceValueGrade3,
                };
                Formula.ForceValue = ForceValues[ItemGrade];
            }

            if (ItemType == RUNTIME_ITEM_TYPE_ARTIFACT) {
                Formula.ForceValue = ForceCode->UniqueEmblemForceValue;
            }

            return Formula;
        }

        return Formula;
    }

    return Formula;
}
