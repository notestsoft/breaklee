#include "ServerDataLoader.h"

#pragma pack(push, 1)

struct _ArchiveItemData {
    UInt32 ItemID;
    Char VisualID[60];
    Char ItemName[60];
    Char ItemDescription[60];
    UInt8 Padding1[20];
    UInt32 ItemType;
    Int32 DisplayStyle;
    Int32 DisplayLevel;
    Float32 InventoryPosition123[3];
    Float32 InventoryPosition456[3];
    Float32 InventoryRotation123[3];
    Float32 InventoryRotation456[3];
    Float32 InventoryScale1;
    Float32 InventoryScale2;
    Int32 MinLevel;
    Int32 MinOverlordLevel;
    Int32 LimitClass;
    Int32 MinHonorRank;
    UInt8 Unknown1[20];
    Float32 InventoryRotation1;
    UInt8 UnknownB;
    Int32 BuyPrice;
    Int32 SellPrice;
    UInt32 SlotSize; // 2bit height // 2bit width
    Int32 Level;
    Int32 Options[22];
    UInt32 Property;
    UInt8 EnchantCodeLink;
    UInt8 Unknown3;
    UInt8 Grade;
    UInt8 Unknown4;
    UInt8 BattleStyleIndices[3];
    UInt8 DurationType;
    UInt8 DurationUse;
    UInt8 UniqueGrade;
    Int32 MaxHonorRank;
    UInt8 MeisterGrade;
    Int16 CooltimeID;
    Int16 MaxStackSize;
    Int32 ExtendedOptions[6];
    Int32 Unknown6;
};

struct _ArchiveMobData {
    Int32 Level;
    Float64 HP;
    Int32 HPRecharge;
    Int32 AttackRate;
    Int32 DefenseRate;
    Int32 Defense;
    Int32 DefaultSkillPhysicalAttackMin;
    Int32 DefaultSkillPhysicalAttackMax;
    Int32 DefaultSkillReach;
    Int32 DefaultSkillRange;
    Float32 DefaultSkillInterval;
    Int32 SpecialSkillPhysicalAttackMin;
    Int32 SpecialSkillPhysicalAttackMax;
    Int32 SpecialSkillReach;
    Int32 SpecialSkillRange;
    Float32 SpecialSkillInterval;
    Int32 AlertRange;
    Int32 ChaseRange;
    Int32 FindCount;
    Float32 FindInterval;
    Int32 AttackPattern;
    UInt32 Aggressive;
    UInt32 Cooperative;
    UInt64 Exp;
    Int32 DefaultSkillStance;
    Int32 SpecialSkillStance;
    Float32 MovementSpeed;
    Float32 ChaseSpeed;
    Int32 Scale;
    Int32 DefaultSkillGroup;
    Int32 SpecialSkillGroup;
    UInt32 Property;
    Int32 D[4];
    Int32 DamageDiscount;
    Int32 MoveInterval;
    Int32 Accuracy;
    Int32 Penetration;
    Int32 ResistCriticalRate;
    Int32 AggroPattern;
    Int32 AttackCountAmp;
    Int32 LimitRangeB;
    Int32 IgnoreAccuracy;
    Int32 IgnoreDamageReduction;
    Int32 IgnorePenetration;
    Int32 AbsoluteDamage;
    Int32 AttackSignalValue;
    Int32 ResistSkillAmp;
    Int32 ResistCriticalDamage;
    Int32 ResistSuppression;
    Int32 ResistSilence;
    Int32 ProportionalHPDmg;
    Int32 O[1];
    Int32 IsWorldBoss;
    UInt8 K[3];
    Int32 L[6];
};

#pragma pack(pop)

static const CString kBattleStyleIndexKeyWA = "2hand";
static const CString kBattleStyleIndexKeyBL = "dual";
static const CString kBattleStyleIndexKeyWI = "magic";
static const CString kBattleStyleIndexKeyFA = "magic_arrow";
static const CString kBattleStyleIndexKeyFS = "sword_shield";
static const CString kBattleStyleIndexKeyFB = "magic_sword";
static const CString kBattleStyleIndexKeyGL = "chakram";
static const CString kBattleStyleIndexKeyFG = "magic_gun";
static const CString kBattleStyleIndexKeyDM = "magic_skull";

static const CString kBattleStyleIndexKeyAltWA = "twohand";
static const CString kBattleStyleIndexKeyAltBL = "dual";
static const CString kBattleStyleIndexKeyAltWI = "magic";
static const CString kBattleStyleIndexKeyAltFA = "magic_arrow";
static const CString kBattleStyleIndexKeyAltFS = "sword_shield";
static const CString kBattleStyleIndexKeyAltFB = "magic_sword";
static const CString kBattleStyleIndexKeyAltGL = "chakram";
static const CString kBattleStyleIndexKeyAltFG = "magic_gun";
static const CString kBattleStyleIndexKeyAltDM = "magic_skull";

CString GetBattleStyleIndexKey(Int32 BattleStyleIndex) {
    switch (BattleStyleIndex) {
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA: return kBattleStyleIndexKeyWA;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL: return kBattleStyleIndexKeyBL;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI: return kBattleStyleIndexKeyWI;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA: return kBattleStyleIndexKeyFA;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS: return kBattleStyleIndexKeyFS;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB: return kBattleStyleIndexKeyFB;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL: return kBattleStyleIndexKeyGL;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG: return kBattleStyleIndexKeyFG;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM: return kBattleStyleIndexKeyDM;
    default: return NULL;
    }
}

CString GetBattleStyleIndexKeyAlt(Int32 BattleStyleIndex) {
    switch (BattleStyleIndex) {
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA: return kBattleStyleIndexKeyAltWA;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL: return kBattleStyleIndexKeyAltBL;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI: return kBattleStyleIndexKeyAltWI;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA: return kBattleStyleIndexKeyAltFA;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS: return kBattleStyleIndexKeyAltFS;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB: return kBattleStyleIndexKeyAltFB;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL: return kBattleStyleIndexKeyAltGL;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG: return kBattleStyleIndexKeyAltFG;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM: return kBattleStyleIndexKeyAltDM;
    default: return NULL;
    }
}

Bool ParseBattleStyleString(
    ArchiveRef Object,
    Int64 NodeIndex,
    CString Name,
    Int32* Result
) {
    Char Buffer[20];

    if (!ParseAttributeString(Object, NodeIndex, Name, Buffer, 20)) return false;

    if (strcmp(Buffer, kBattleStyleIndexKeyWA) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyBL) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyWI) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyFA) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyFS) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyFB) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyGL) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyFG) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyDM) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM;
    }
    else {
        return false;
    }

    return true;
}

Bool ServerLoadCharacterTemplateData(
    ServerContextRef Context,
    ArchiveRef RankArchive
) {
    ArchiveRef Archive = RankArchive;
    Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.char_init");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "init");
    while (Iterator) {
        Int32 BattleStyleIndex = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "style", &BattleStyleIndex)) goto error;

        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        RTBattleStyleLevelFormulaDataRef FormulaData = &Context->Runtime->BattleStyleLevelFormulaData[BattleStyleIndex - 1];
        FormulaData->BattleStyleIndex = BattleStyleIndex;
        
        if (!ParseAttributeInt32(Archive, Iterator->Index, "hp", &FormulaData->BaseHP)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mp", &FormulaData->BaseMP)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "delta_hp", &FormulaData->DeltaHP)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "delta_mp", &FormulaData->DeltaMP)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.char_newhp");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "newhp");
    while (Iterator) {
        Int32 BattleStyleIndex = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "bs", &BattleStyleIndex)) goto error;

        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        RTBattleStyleLevelFormulaDataRef FormulaData = &Context->Runtime->BattleStyleLevelFormulaData[BattleStyleIndex - 1];
        FormulaData->BattleStyleIndex = BattleStyleIndex;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "bdelta", &FormulaData->DeltaHP2)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadBattleStyleFormulaData(
    RTRuntimeRef Runtime,
    ArchiveRef RankArchive
) {
    Int64 ParentIndex = ArchiveNodeGetChildByPath(RankArchive, -1, "cabal.rankup");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(RankArchive, ParentIndex, "class_formula");
    while (Iterator) {
        Int32 BattleStyleIndex = 0;
        if (!ParseBattleStyleString(RankArchive, Iterator->Index, "class", &BattleStyleIndex)) goto error;
        
        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        RTBattleStyleClassFormulaDataRef FormulaData = &Runtime->BattleStyleClassFormulaData[BattleStyleIndex - 1];
        FormulaData->BattleStyleIndex = BattleStyleIndex;

        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_atk", FormulaData->Attack, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_matk", FormulaData->MagicAttack, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_def", FormulaData->Defense, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_ar", FormulaData->AttackRate, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_dr", FormulaData->DefenseRate, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_dmgdsc", FormulaData->DamageReduction, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_hit", FormulaData->Accuracy, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_penet", FormulaData->Penetration, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_hitpenel", FormulaData->AccuracyPenalty, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "class_penetpenel", FormulaData->PenetrationPenalty, 2, ',')) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(RankArchive, Iterator);
    }

    for (Int32 BattleStyleIndex = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN; BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX; BattleStyleIndex += 1) {
        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        RTBattleStyleSkillFormulaDataRef FormulaData = &Runtime->BattleStyleSkillFormulaData[BattleStyleIndex - 1];
        FormulaData->BattleStyleIndex = BattleStyleIndex;
        FormulaData->SkillRankCount = 0;
    }

    Iterator = ArchiveQueryNodeIteratorFirst(RankArchive, ParentIndex, "exp_for_point");
    while (Iterator) {
        Int32 SkillRank = 0;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "rank", &SkillRank)) goto error;

        ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(RankArchive, Iterator->Index, "condition");
        while (ChildIterator) {
            Int32 BattleStyleIndex = 0;
            if (!ParseBattleStyleString(RankArchive, ChildIterator->Index, "class", &BattleStyleIndex)) goto error;

            assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

            RTBattleStyleSkillFormulaDataRef FormulaData = &Runtime->BattleStyleSkillFormulaData[BattleStyleIndex - 1];
            assert(FormulaData->BattleStyleIndex == BattleStyleIndex);
            assert(FormulaData->SkillRankCount < RUNTIME_MEMORY_MAX_SKILL_RANK_DATA_COUNT);

            RTBattleStyleSkillRankDataRef RankData = &FormulaData->SkillRanks[FormulaData->SkillRankCount];
            RankData->SkillRank = SkillRank;

            if (!ParseAttributeInt32(RankArchive, ChildIterator->Index, "exp", &RankData->SkillLevelExp)) goto error;

            FormulaData->SkillRankCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(RankArchive, ChildIterator);
        }

        Iterator = ArchiveQueryNodeIteratorNext(RankArchive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(RankArchive, ParentIndex, "mastery_levelup");
    while (Iterator) {
        Int32 Level = 0;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "level", &Level)) goto error;

        ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(RankArchive, Iterator->Index, "condition");
        while (ChildIterator) {
            Int32 BattleStyleIndex;
            if (!ParseBattleStyleString(RankArchive, ChildIterator->Index, "class", &BattleStyleIndex)) goto error;
            
            assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

            RTBattleStyleRankFormulaDataRef FormulaData = &Runtime->BattleStyleRankFormulaData[BattleStyleIndex - 1];
            FormulaData->BattleStyleIndex = BattleStyleIndex;
            assert(FormulaData->RankCount < RUNTIME_MEMORY_MAX_BATTLE_RANK_DATA_COUNT);

            RTBattleStyleRankDataRef RankData = &FormulaData->Ranks[FormulaData->RankCount];
            RankData->Level = Level;

            if (!ParseAttributeInt32(RankArchive, ChildIterator->Index, "str", &RankData->ConditionSTR)) goto error;
            if (!ParseAttributeInt32(RankArchive, ChildIterator->Index, "dex", &RankData->ConditionDEX)) goto error;
            if (!ParseAttributeInt32(RankArchive, ChildIterator->Index, "int", &RankData->ConditionINT)) goto error;

            if (!ParseAttributeInt32(RankArchive, ChildIterator->Index, "skill1idx", &RankData->SkillIndex[0]) ||
                !ParseAttributeInt32(RankArchive, ChildIterator->Index, "skill1slotidx", &RankData->SkillSlot[0])) {
                RankData->SkillIndex[0] = 0;
                RankData->SkillSlot[0] = 0;
            }

            if (!ParseAttributeInt32(RankArchive, ChildIterator->Index, "skill2idx", &RankData->SkillIndex[1]) ||
                !ParseAttributeInt32(RankArchive, ChildIterator->Index, "skill2slotidx", &RankData->SkillSlot[1])) {
                RankData->SkillIndex[1] = 0;
                RankData->SkillSlot[1] = 0;
            }

            FormulaData->RankCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(RankArchive, ChildIterator);
        }

        Iterator = ArchiveQueryNodeIteratorNext(RankArchive, Iterator);
    }

    ParentIndex = ArchiveNodeGetChildByPath(RankArchive, -1, "cabal.rankup.stat_formula");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(RankArchive, ParentIndex, "flag");
    while (Iterator) {
        Int32 BattleStyleIndex = 0;
        if (!ParseBattleStyleString(RankArchive, Iterator->Index, "class", &BattleStyleIndex)) goto error;

        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        RTBattleStyleStatsFormulaDataRef FormulaData = &Runtime->BattleStyleStatsFormulaData[BattleStyleIndex - 1];
        FormulaData->BattleStyleIndex = BattleStyleIndex;

        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "atk_flag", FormulaData->Attack, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "matk_flag", FormulaData->MagicAttack, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "dmgdcs_flag", FormulaData->DamageReduction, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "flee_flag", FormulaData->Evasion, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "ar_flag", FormulaData->AttackRate, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "dr_flag", FormulaData->DefenseRate, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "crpreg_flag", FormulaData->ResistCriticalRate, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "samreg_flag", FormulaData->ResistSkillAmp, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "crdreg_flag", FormulaData->ResistCriticalDamage, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "movreg_flag", FormulaData->ResistUnmovable, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "stnreg_flag", FormulaData->ResistStun, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "dwnreg_flag", FormulaData->ResistDown, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "nubreg_flag", FormulaData->ResistKnockback, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "hp_flag", FormulaData->HP, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "mp_flag", FormulaData->MP, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(RankArchive, Iterator->Index, "ignorepenet_flag", FormulaData->IgnorePenetration, 3, ',')) goto error;

        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "atk_code", &FormulaData->AttackSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "matk_code", &FormulaData->MagicAttackSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "dmgdcs_code", &FormulaData->DamageReductionSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "flee_code", &FormulaData->EvasionSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "ar_code", &FormulaData->AttackRateSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "dr_code", &FormulaData->DefenseRateSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "crpreg_code", &FormulaData->ResistCriticalRateSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "samreg_code", &FormulaData->ResistSkillAmpSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "crdreg_code", &FormulaData->ResistCriticalDamageSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "movreg_code", &FormulaData->ResistUnmovableSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "stnreg_code", &FormulaData->ResistStunSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "dwnreg_code", &FormulaData->ResistDownSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "nubreg_code", &FormulaData->ResistKnockbackSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "hp_code", &FormulaData->HPSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "mp_code", &FormulaData->MPSlopeID)) goto error;
        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "ignorepenet_code", &FormulaData->IgnorePenetrationSlopeID)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(RankArchive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(RankArchive, ParentIndex, "parameter");
    while (Iterator) {
        assert(Runtime->SlopeFormulaDataCount < RUNTIME_MEMORY_MAX_SLOPE_FORMULA_COUNT);

        RTBattleStyleSlopeFormulaDataRef FormulaData = &Runtime->BattleStyleSlopeFormulaData[Runtime->SlopeFormulaDataCount];
        FormulaData->SlopeCount = 0;

        if (!ParseAttributeInt32(RankArchive, Iterator->Index, "code", &FormulaData->SlopeID)) goto error;

        ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(RankArchive, Iterator->Index, "condition");
        while (ChildIterator) {
            assert(FormulaData->SlopeCount < RUNTIME_MEMORY_MAX_SLOPE_DATA_COUNT);

            RTBattleStyleSlopeDataRef SlopeData = &FormulaData->Slopes[FormulaData->SlopeCount];

            if (!ParseAttributeInt32(RankArchive, ChildIterator->Index, "penalty_stat", &SlopeData->Penalty)) goto error;
            if (!ParseAttributeInt32(RankArchive, ChildIterator->Index, "slope", &SlopeData->Slope)) goto error;
            if (!ParseAttributeInt32(RankArchive, ChildIterator->Index, "intercept", &SlopeData->Intercept)) goto error;

            FormulaData->SlopeCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(RankArchive, ChildIterator);
        }

        Runtime->SlopeFormulaDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(RankArchive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadQuestData(
    RTRuntimeRef Runtime,
    ArchiveRef Archive
) {
    Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest.cabal_quest");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "quest");
    while (Iterator) {
        assert(Runtime->QuestDataCount < RUNTIME_MEMORY_MAX_QUEST_DATA_COUNT);

        RTQuestDataRef Quest = &Runtime->QuestData[Runtime->QuestDataCount];
        Quest->Index = Runtime->QuestDataCount;
        Quest->QuestBeginNpcIndex = -1;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "quest_id", &Quest->ID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "type", &Quest->Type)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "party", &Quest->Party)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_order", &Quest->MissionOrder)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_set", &Quest->MissionSet)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "delete_type", &Quest->DeleteType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dailycount", &Quest->DailyCount)) goto error;

        ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "condition");
        while (ChildIterator) {
            RTQuestConditionDataRef Condition = &Quest->Condition;

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "level", &Condition->Level)) goto error;
            if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "class", Condition->Class, 2, ':')) goto error;
            if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "item", Condition->Item, 3, ':')) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "maxlv", &Condition->MaxLevel)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "rank", &Condition->Rank)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "rank_type", &Condition->RankType)) goto error;
            if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "skill", Condition->Skill, 2, ':')) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "min_reputation_class", &Condition->MinHonorRank)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "max_reputation_class", &Condition->MaxHonorRank)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "related_qid", &Condition->RelatedQuestID)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "nation_type", &Condition->NationType)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MinOLv", &Condition->MinOverlordLevel)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MaxOLv", &Condition->MaxOverlordLevel)) goto error;

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            assert(!ChildIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "mission");
        while (ChildIterator) {
            Int32 Type;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "type", &Type)) goto error;

            if (Type == 0) {
                assert(Quest->MissionMobCount < RUNTIME_MAX_QUEST_COUNTER_COUNT);

                RTQuestMissionDataRef Mission = &Quest->MissionMobs[Quest->MissionMobCount];
                memset(Mission, 0, sizeof(struct _RTQuestMissionData));

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "order", &Mission->Order)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "notify_order", &Mission->NotifyOrder)) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "value", &Mission->Value[0], 2, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where1", Mission->Location1, 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where2", Mission->Location2, 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where3", Mission->Location3, 3, ':')) goto error;

                Quest->MissionMobCount += 1;
            }
            else if (Type == 1) {
                assert(Quest->MissionItemCount < RUNTIME_MAX_QUEST_COUNTER_COUNT);

                RTQuestMissionDataRef Mission = &Quest->MissionItems[Quest->MissionItemCount];

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "order", &Mission->Order)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "notify_order", &Mission->NotifyOrder)) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "value", &Mission->Value[0], 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where1", Mission->Location1, 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where2", Mission->Location2, 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where3", Mission->Location3, 3, ':')) goto error;

                Quest->MissionItemCount += 1;
            }
            else if (Type == 2) {
                assert(Quest->MissionDungeonCount < RUNTIME_MAX_QUEST_COUNTER_COUNT);

                RTQuestMissionDataRef Mission = &Quest->MissionDungeons[Quest->MissionDungeonCount];

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "order", &Mission->Order)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "notify_order", &Mission->NotifyOrder)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "value", &Mission->Value[0])) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where1", Mission->Location1, 0, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where2", Mission->Location2, 0, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where3", Mission->Location3, 0, ':')) goto error;

                Quest->MissionDungeonCount += 1;
            }
            else {
                assert(0);
            }
            
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "param");
        while (ChildIterator) {
            if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "reward", Quest->Reward, 9, ',')) goto error;
            
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            assert(!ChildIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "npcs");
        while (ChildIterator) {
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "npcset_id", &Quest->NpcSet.ID)) goto error;

            ArchiveIteratorRef NpcIterator = ArchiveQueryNodeIteratorFirst(Archive, ChildIterator->Index, "quest_npc");
            while (NpcIterator) {
                assert(Quest->NpcSet.Count < RUNTIME_MEMORY_MAX_QUEST_NPC_COUNT);

                RTQuestNpcDataRef Npc = &Quest->NpcSet.Npcs[Quest->NpcSet.Count];
                Int32 NpcID[2];

                if (!ParseAttributeInt32(Archive, NpcIterator->Index, "unique_order", &Npc->Index)) goto error;
                if (!ParseAttributeInt32Array(Archive, NpcIterator->Index, "npc_id", NpcID, 2, ',')) goto error;
                
                Npc->WorldID = NpcID[0];
                Npc->NpcID = NpcID[1];

                if (!ParseAttributeInt32(Archive, NpcIterator->Index, "npc_action_order", &Npc->NpcActionOrder)) goto error;
                if (!ParseAttributeInt32(Archive, NpcIterator->Index, "action_type", &Npc->ActionType)) goto error;
                if (!ParseAttributeInt32Array(Archive, NpcIterator->Index, "value", Npc->Value, 3, ',')) goto error;

                ParseAttributeInt32Array(Archive, NpcIterator->Index, "notify", Npc->Notification, 2, ':');

                if (Npc->ActionType == RUNTIME_QUEST_ACTION_TYPE_BEGIN) {
                    Quest->QuestBeginNpcIndex = Quest->NpcSet.Count;
                }

                Quest->NpcSet.Count += 1;
                NpcIterator = ArchiveQueryNodeIteratorNext(Archive, NpcIterator);
            }

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            assert(!ChildIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "dungeon");
        while (ChildIterator) {
            assert(Quest->DungeonIndexCount < RUNTIME_MAX_QUEST_DUNGEON_INDEX_COUNT);

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "id", &Quest->DungeonIndex[Quest->DungeonIndexCount])) goto error;

            Quest->DungeonIndexCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        Runtime->QuestDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest.cabal_reward");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "reward_item_set");
    while (Iterator) {
        assert(Runtime->QuestRewardItemSetDataCount < RUNTIME_MEMORY_MAX_QUEST_REWARD_ITEM_SET_DATA_COUNT);

        RTQuestRewardItemSetDataRef ItemSet = &Runtime->QuestRewardItemSetData[Runtime->QuestRewardItemSetDataCount];
        ItemSet->Count = 0;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &ItemSet->ID)) goto error;
        
        ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "reward_item");
        while (ChildIterator) {
            assert(ItemSet->Count < RUNTIME_MEMORY_MAX_QUEST_REWARD_ITEM_DATA_COUNT);

            RTQuestRewardItemDataRef Item = &ItemSet->Items[ItemSet->Count];
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "class", &Item->BattleStyleIndex)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "order", &Item->Index)) goto error;
            if (!ParseAttributeUInt32Array(Archive, ChildIterator->Index, "item_id", Item->ItemID, 2)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "duration", &Item->ItemDuration)) goto error;

            ItemSet->Count += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        Runtime->QuestRewardItemSetDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadWarpData(
    RTRuntimeRef Runtime,
    ArchiveRef Archive
) {
    Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "World.warp_npc");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef WorldIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "world");
    while (WorldIterator) {
        Int32 WorldID;
        if (!ParseAttributeInt32(Archive, WorldIterator->Index, "id", &WorldID)) goto error;

        ArchiveIteratorRef NpcIterator = ArchiveQueryNodeIteratorFirst(Archive, WorldIterator->Index, "npc");
        while (NpcIterator) {
            Int32 NpcID;
            if (!ParseAttributeInt32(Archive, NpcIterator->Index, "id", &NpcID)) goto error;

            ArchiveIteratorRef WarpIterator = ArchiveQueryNodeIteratorFirst(Archive, NpcIterator->Index, "warp_list");
            while (WarpIterator) {
                assert(Runtime->WarpCount < RUNTIME_MEMORY_MAX_WARP_COUNT);

                RTWarpRef Warp = &Runtime->Warps[Runtime->WarpCount];
                Warp->WorldID = WorldID;
                Warp->NpcID = NpcID;

                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "order", &Warp->Index)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "type", &Warp->Type)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "target_id", &Warp->TargetID)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "level", &Warp->Level)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "Fee", &Warp->Fee)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "quest_id", &Warp->QuestID)) goto error;
                if (!ParseAttributeUInt32Array(Archive, WarpIterator->Index, "warp_item", &Warp->WarpItemID, 2)) goto error;
    
                Runtime->WarpCount += 1;
                WarpIterator = ArchiveQueryNodeIteratorNext(Archive, WarpIterator);
            }

            NpcIterator = ArchiveQueryNodeIteratorNext(Archive, NpcIterator);
        }

        WorldIterator = ArchiveQueryNodeIteratorNext(Archive, WorldIterator);
    }

    return true;

error:
    return false;
}

CString GetItemDescription(
    ArchiveRef Messages,
    CString Name
) {
    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorByPathFirst(Messages, -1, "cabal_message.item_msg.msg");
    while (Iterator) {
        Int64 AttributeIndex = ArchiveNodeGetAttributeByName(Messages, Iterator->Index, "id");
        if (AttributeIndex >= 0) {
            ArchiveStringRef String = ArchiveAttributeGetData(Messages, AttributeIndex);
            if (memcmp(String->Data, Name, String->Length) == 0) {
                AttributeIndex = ArchiveNodeGetAttributeByName(Messages, Iterator->Index, "cont");
                if (AttributeIndex >= 0) {
                    ArchiveStringRef String = ArchiveAttributeGetData(Messages, AttributeIndex);
                    return String->Data;
                }
            }
        }

        Iterator = ArchiveQueryNodeIteratorNext(Messages, Iterator);
    }

    return "";
}

Bool ServerLoadItemData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    RTRuntimeRef Runtime = Context->Runtime;

    CString FileNames[] = {
        "item.enc",
        "item2.enc",
        "item3.enc",
        "item4.enc"
    };

    for (Int32 FileIndex = 0; FileIndex < 4; FileIndex += 1) {
        CString FilePath = PathCombineNoAlloc(RuntimeDirectory, FileNames[FileIndex]);
        UInt8* Buffer = NULL;
        Int32 BufferLength = 0;

        if (!EncryptionDecryptFile(FilePath, &Buffer, &BufferLength)) {
            Fatal("Error loading file: %s", FilePath);
            goto error;
        }

        Int32 ItemCount = BufferLength / sizeof(struct _ArchiveItemData);
        for (Int32 Index = 0; Index < ItemCount; Index += 1) {
            assert(Runtime->ItemDataCount < RUNTIME_MEMORY_MAX_ITEM_DATA_COUNT);

            struct _ArchiveItemData* ArchiveItemData = (struct _ArchiveItemData*)(Buffer + sizeof(struct _ArchiveItemData) * Index);
            RTItemDataRef ItemData = &Runtime->ItemData[Runtime->ItemDataCount];
            ItemData->ItemID = ArchiveItemData->ItemID;
            ItemData->ItemType = ArchiveItemData->ItemType;
            ItemData->ItemGrade = ArchiveItemData->Grade;
            ItemData->SellPrice = ArchiveItemData->SellPrice;
            ItemData->BuyPrice = 0;
            ItemData->MinLevel = ArchiveItemData->MinLevel;
            memcpy(ItemData->Options, ArchiveItemData->Options, sizeof(ArchiveItemData->Options));
            ItemData->MaxStackSize = ArchiveItemData->MaxStackSize;
            ItemData->UniqueGrade = ArchiveItemData->UniqueGrade;
            ItemData->MasterGrade = ArchiveItemData->MeisterGrade;
            ItemData->EnchantCodeLink = ArchiveItemData->EnchantCodeLink;
            memcpy(ItemData->ExtendedOptions, ArchiveItemData->ExtendedOptions, sizeof(ArchiveItemData->ExtendedOptions));
            Runtime->ItemDataCount += 1;
        }

        free(Buffer);
    }

    return true;

error:
    return false;
}

Bool ServerLoadMobData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    RTRuntimeRef Runtime = Context->Runtime;

    CString FilePath = PathCombineNoAlloc(RuntimeDirectory, "mobserver.dat");
    UInt8* Memory = NULL;
    Int32 MemoryLength;
    FileRef File = FileOpen(FilePath);
    if (!File) return false;

    FileRead(File, &Memory, &MemoryLength);

    struct _ArchiveMobData* ArchiveMobMemory = (struct _ArchiveMobData*)Memory;

    Index MobCount = MemoryLength / sizeof(struct _ArchiveMobData);
    for (Int32 Index = 0; Index < MobCount; Index += 1) {
        struct _ArchiveMobData* ArchiveMobData = &ArchiveMobMemory[Index];

        assert(Index < RUNTIME_MEMORY_MAX_MOB_DATA_COUNT);
        RTMobSpeciesDataRef MobData = &Runtime->MobData[Index];
        Runtime->MobDataCount = MAX(Runtime->MobDataCount, Index + 1);

        MobData->MobSpeciesIndex = Index;
        MobData->Level = ArchiveMobData->Level;
        MobData->MoveSpeed = ArchiveMobData->MovementSpeed / 100;
        MobData->ChaseSpeed = ArchiveMobData->ChaseSpeed / 100;
        MobData->Radius = 0; // TODO: Load radius from mobs data
        MobData->Property = ArchiveMobData->Property;
        MobData->AttackPattern = ArchiveMobData->AttackPattern;
        MobData->AggressiveType = ArchiveMobData->Aggressive; // TODO: Check if this maps correctly
        MobData->Cooperate = ArchiveMobData->Cooperative;
        MobData->Escape = 0; // TODO: Property is missing
        MobData->AttackType = 0; // TODO: Property is missing
        MobData->Scale = ArchiveMobData->Scale;
        MobData->FindCount = ArchiveMobData->FindCount;
        MobData->FindInterval = ArchiveMobData->FindInterval * 1000;
        MobData->MoveInterval = ArchiveMobData->MoveInterval;
        MobData->AlertRange = ArchiveMobData->AlertRange;
        MobData->ChaseRange = ArchiveMobData->ChaseRange;
        MobData->LimitRangeB = ArchiveMobData->LimitRangeB;
        MobData->ChaseInterval = 100;
        MobData->HP = ArchiveMobData->HP;
        MobData->Defense = ArchiveMobData->Defense;
        MobData->AttackRate = ArchiveMobData->AttackRate;
        MobData->DefenseRate = ArchiveMobData->DefenseRate;
        MobData->HPRecharge = ArchiveMobData->HPRecharge;
        MobData->DamageReduction = ArchiveMobData->DamageDiscount;
        MobData->Accuracy = ArchiveMobData->Accuracy;
        MobData->Penetration = ArchiveMobData->Penetration;
        MobData->ResistCriticalRate = ArchiveMobData->ResistCriticalRate;
        MobData->IgnoreAccuracy = ArchiveMobData->IgnoreAccuracy;
        MobData->IgnoreDamageReduction = ArchiveMobData->IgnoreDamageReduction;
        MobData->IgnorePenetration = ArchiveMobData->IgnorePenetration;
        MobData->AbsoluteDamage = ArchiveMobData->AbsoluteDamage;
        MobData->ResistSkillAmp = ArchiveMobData->ResistSkillAmp;
        MobData->ResistCriticalDamage = ArchiveMobData->ResistCriticalDamage;
        MobData->ResistSuppression = ArchiveMobData->ResistSuppression;
        MobData->ResistSilence = ArchiveMobData->ResistSilence;
        MobData->ReflectDamage = ArchiveMobData->ProportionalHPDmg;
        MobData->MoveInterval = ArchiveMobData->MoveInterval;
        MobData->Accuracy = ArchiveMobData->Accuracy;
        MobData->DefaultSkill.PhysicalAttackMin = ArchiveMobData->DefaultSkillPhysicalAttackMin;
        MobData->DefaultSkill.PhysicalAttackMax = ArchiveMobData->DefaultSkillPhysicalAttackMax;
        MobData->DefaultSkill.Reach = ArchiveMobData->DefaultSkillReach;
        MobData->DefaultSkill.Range = ArchiveMobData->DefaultSkillRange;
        MobData->DefaultSkill.Stance = ArchiveMobData->DefaultSkillStance;
        MobData->DefaultSkill.SkillGroup = ArchiveMobData->DefaultSkillGroup;
        MobData->DefaultSkill.Interval = ArchiveMobData->DefaultSkillInterval * 1000;
        MobData->SpecialSkill.PhysicalAttackMin = ArchiveMobData->SpecialSkillPhysicalAttackMin;
        MobData->SpecialSkill.PhysicalAttackMax = ArchiveMobData->SpecialSkillPhysicalAttackMax;
        MobData->SpecialSkill.Reach = ArchiveMobData->SpecialSkillReach;
        MobData->SpecialSkill.Range = ArchiveMobData->SpecialSkillRange;
        MobData->SpecialSkill.Stance = ArchiveMobData->SpecialSkillStance;
        MobData->SpecialSkill.SkillGroup = ArchiveMobData->SpecialSkillGroup;
        MobData->SpecialSkill.Interval = ArchiveMobData->SpecialSkillInterval * 1000;
        MobData->AttackSignal = ArchiveMobData->AttackSignalValue;
        MobData->IsWorldBoss = ArchiveMobData->IsWorldBoss;
        MobData->Exp = ArchiveMobData->Exp;

        Int32 Radius = (MobData->Scale + 1) >> 1;
        MobData->DefaultSkill.Distance = (MobData->Scale >> 1) + MobData->DefaultSkill.Reach;
        MobData->DefaultSkill.Offset = Radius + ((MobData->DefaultSkill.Reach - Radius) >> 1);
        if (MobData->DefaultSkill.Offset < Radius) {
            MobData->DefaultSkill.Offset = Radius;
        }

        MobData->SpecialSkill.Distance = (MobData->Scale >> 1) + MobData->SpecialSkill.Reach;
        MobData->SpecialSkill.Offset = Radius + ((MobData->SpecialSkill.Reach - Radius) >> 1);
        if (MobData->SpecialSkill.Offset < Radius) {
            MobData->SpecialSkill.Offset = Radius;
        }

        // TODO: Replace this with other computations based on attack types and patterns...
        MobData->CanAttack = 1;
        MobData->CanMove = (MobData->FindCount >= RUNTIME_MOB_FIND_COUNT_UNMOVABLE) ? 0 : 1;
    }

    Int32 SkillGroupUsage[50] = { 0 };
    Int32 AttackPatternUsage[50] = { 0 };

    for (Int32 Index = 0; Index < MobCount; Index += 1) {
        struct _ArchiveMobData* ArchiveMobData = &ArchiveMobMemory[Index];

        SkillGroupUsage[ArchiveMobData->DefaultSkillGroup] += 1;
        SkillGroupUsage[ArchiveMobData->SpecialSkillGroup] += 1;
        AttackPatternUsage[ArchiveMobData->AttackPattern] += 1;

        if (ArchiveMobData->D[0]) {
            Warn("Mob (%d) D[0]: %d", Index, ArchiveMobData->D[0]);
        }

        if (ArchiveMobData->D[1]) {
            Warn("Mob (%d) D[1]: %d", Index, ArchiveMobData->D[1]);
        }

        if (ArchiveMobData->D[2]) {
            Warn("Mob (%d) D[2]: %d", Index, ArchiveMobData->D[2]);
        }

        if (ArchiveMobData->O[0]) {
            Warn("Mob (%d) O[0]: %d", Index, ArchiveMobData->O[0]);
        }

        if (ArchiveMobData->K[0]) {
            Warn("Mob (%d) K[0]: %d", Index, ArchiveMobData->K[0]);
        }

        if (ArchiveMobData->K[1]) {
            Warn("Mob (%d) K[1]: %d", Index, ArchiveMobData->K[1]);
        }

        if (ArchiveMobData->K[2]) {
            Warn("Mob (%d) K[2]: %d", Index, ArchiveMobData->K[2]);
        }

        if (ArchiveMobData->L[0]) {
            Warn("Mob (%d) L[0]: %d", Index, ArchiveMobData->L[0]);
        }

        if (ArchiveMobData->L[1]) {
            Warn("Mob (%d) L[1]: %d", Index, ArchiveMobData->L[1]);
        }

        if (ArchiveMobData->L[2]) {
            Warn("Mob (%d) L[2]: %d", Index, ArchiveMobData->L[2]);
        }

        if (ArchiveMobData->L[3]) {
            Warn("Mob (%d) L[3]: %d", Index, ArchiveMobData->L[3]);
        }

        if (ArchiveMobData->L[4]) {
            Warn("Mob (%d) L[4]: %d", Index, ArchiveMobData->L[4]);
        }

        if (ArchiveMobData->L[5]) {
            Warn("Mob (%d) L[5]: %d", Index, ArchiveMobData->L[5]);
        }
    }

    for (Int32 Index = 0; Index < 50; Index += 1) {
        if (SkillGroupUsage[Index] > 0)
            Warn("Mob Skill Group: %d", Index);
    }

    for (Int32 Index = 0; Index < 50; Index += 1) {
        if (AttackPatternUsage[Index] > 0)
            Warn("Mob Attack Pattern: %d", Index);
    }

    FileClose(File);
    free(Memory);
    return true;
}

Bool ServerLoadWarpData(
    RTRuntimeRef Runtime,
    ArchiveRef Archive
);

Bool ServerLoadWorldData(
    RTRuntimeRef Runtime,
    CString RuntimeDirectory,
    CString ServerDirectory,
    CString ScriptDirectory,
    ArchiveRef TerrainArchive,
    ArchiveRef MainArchive,
    Bool LoadShops
) {
    ArchiveRef Archive = ArchiveCreateEmpty(AllocatorGetSystemDefault());

    Int64 ParentIndex = ArchiveNodeGetChildByPath(TerrainArchive, -1, "Terrain.map");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(TerrainArchive, ParentIndex, "map_index");
    while (Iterator) {
        Index WorldIndex = 0;
        if (!ParseAttributeIndex(TerrainArchive, Iterator->Index, "world_id", &WorldIndex)) goto error;

        RTWorldDataRef World = RTWorldDataCreate(Runtime->WorldManager, WorldIndex);
        World->WorldIndex = WorldIndex;
        World->MobTable = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMob), 8);
        World->DropTable.WorldDropPool = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTDropItem), 8);
        World->DropTable.MobDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);
        World->DropTable.QuestDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);

        Char WorldFileName[MAX_PATH];
        if (!ParseAttributeInt32(TerrainArchive, Iterator->Index, "dead_warp", &World->DeadWarpIndex)) goto error;
        if (!ParseAttributeString(TerrainArchive, Iterator->Index, "name", WorldFileName, MAX_PATH)) goto error;
        if (!ParseAttributeInt32(TerrainArchive, Iterator->Index, "return_warp", &World->ReturnWarpIndex)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(TerrainArchive, Iterator);
    }

    Int32 HasMapCode = 0;
    Int32 GpsOrder = 0;
    Int32 WarAllowed = 0;
    Int32 WarControl = 0;
    Char MapFileName[MAX_PATH] = { 0 };

    for (Index WorldIndex = 0; WorldIndex < Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!RTWorldDataExists(Runtime->WorldManager, WorldIndex)) continue;
        
        RTWorldDataRef World = RTWorldDataGet(Runtime->WorldManager, WorldIndex);

        Char WorldFilePath[MAX_PATH] = { 0 };
        Char WorldFileDirectory[MAX_PATH] = { 0 };
        Char WorldFileName[MAX_PATH] = { 0 };

        sprintf(WorldFileName, "world%zu.enc", World->WorldIndex);
        PathCombine(RuntimeDirectory, "World", WorldFileDirectory);
        PathCombine(WorldFileDirectory, WorldFileName, WorldFilePath);
        ArchiveClear(Archive, true);

        if (!ArchiveLoadFromFileEncryptedNoAlloc(Archive, WorldFilePath, false)) {
            Error("Loading world file %s failed!", WorldFilePath);
            continue;
        }

        if (!ServerLoadWarpData(Runtime, Archive)) goto error;

        Int64 NodeIndex = ArchiveQueryNodeWithAttribute(
            Archive,
            -1,
            "World.cabal_world.world",
            "id",
            UInt64ToStringNoAlloc(World->WorldIndex)
        );

        if (NodeIndex < 0) goto error;

        Index WorldIndex = 0;
        if (!ParseAttributeIndex(Archive, NodeIndex, "id", &WorldIndex)) goto error;
        assert(WorldIndex == World->WorldIndex);

        if (!ParseAttributeInt32(Archive, NodeIndex, "type", &World->Type)) goto error;
        if (!ParseAttributeString(Archive, NodeIndex, "map_file", MapFileName, MAX_PATH)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIndex, "m_code", &HasMapCode)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIndex, "gpsorder", &World->MapCodeIndex)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIndex, "allowedwar", &WarAllowed)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIndex, "warcontrol", &WarControl)) goto error;

        if (HasMapCode) World->Flags |= RUNTIME_WORLD_FLAGS_HAS_MAPCODE;
        if (WarAllowed) World->Flags |= RUNTIME_WORLD_FLAGS_WAR_ALLOWED;
        if (WarControl) World->Flags |= RUNTIME_WORLD_FLAGS_WAR_CONTROL;

        if (strlen(MapFileName) > 0) {
            Char MapFilePath[MAX_PATH] = { 0 };
            Char MapFileDirectory[MAX_PATH] = { 0 };

            PathCombine(RuntimeDirectory, "Map", MapFileDirectory);
            PathCombine(MapFileDirectory, MapFileName, MapFilePath);
            Info("Loading map file: %s", MapFilePath);

            FileRef MapFile = FileOpen(MapFilePath);
            if (!MapFile) goto error;

            UInt8* MapData = NULL;
            Int32 MapDataLength = 0;

            if (!FileRead(MapFile, &MapData, &MapDataLength)) {
                FileClose(MapFile);
                goto error;
            }

            FileClose(MapFile);

            UInt64 MapDataOffset = 0;
            UInt32 MapFileMagic = *((UInt32*)&MapData[MapDataOffset]);
            MapDataOffset += 132;
            if (MapFileMagic == 1006) {
                MapDataOffset += 4;
            }
            else if (MapFileMagic == 1007) {
                MapDataOffset += 4;
            }

            Int32 EffectCount = *((Int32*)&MapData[MapDataOffset]);
            MapDataOffset += 4;

            for (Int32 Index = 0; Index < EffectCount; Index++) {
                UInt16 TextLength = *((UInt16*)&MapData[MapDataOffset]);
                MapDataOffset += 18;
                MapDataOffset += TextLength;
            }

            Int32 TextureCount = *((Int32*)&MapData[MapDataOffset]);
            MapDataOffset += 4;

            for (Int32 Index = 0; Index < TextureCount; Index++) {
                UInt32 Size = *((UInt32*)&MapData[MapDataOffset]);
                MapDataOffset += 4;
                MapDataOffset += Size;
            }

            MapDataOffset += 12;
            MapDataOffset += (RUNTIME_WORLD_SIZE + 1) * (RUNTIME_WORLD_SIZE + 1) * sizeof(UInt32);
            UInt32* TileData = (UInt32*)&MapData[MapDataOffset];
            memcpy(World->Tiles, TileData, sizeof(UInt32) * RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE);
            free(MapData);
        }

        ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, NodeIndex, "world_npc");
        while (ChildIterator) {
            assert(Runtime->NpcCount < RUNTIME_MEMORY_MAX_NPC_COUNT);

            RTNpcRef Npc = &Runtime->Npcs[Runtime->NpcCount];

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "id", &Npc->ID)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "x", &Npc->X)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "y", &Npc->Y)) goto error;

            Npc->WorldID = World->WorldIndex;

            Runtime->NpcCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        // Load Trainer Data
        // TODO: Move trainer data to server data
        {
            ArchiveIteratorRef SkillIterator = ArchiveQueryNodeIteratorFirst(Archive, NodeIndex, "skill");
            if (SkillIterator) {
                assert(Runtime->TrainerDataCount < RUNTIME_MEMORY_MAX_TRAINER_DATA_COUNT);

                RTTrainerDataRef TrainerData = &Runtime->TrainerData[Runtime->TrainerDataCount];
                TrainerData->Index = Runtime->TrainerDataCount;
                TrainerData->WorldID = World->WorldIndex;
                TrainerData->NpcID = 2;

                while (SkillIterator) {
                    assert(TrainerData->SkillCount < RUNTIME_TRAINER_MAX_SKILL_COUNT);

                    RTTrainerSkillDataRef SkillData = &TrainerData->Skills[TrainerData->SkillCount];

                    if (!ParseAttributeInt32(Archive, SkillIterator->Index, "id", &SkillData->ID)) goto error;
                    if (!ParseAttributeInt32(Archive, SkillIterator->Index, "slot_id", &SkillData->SlotID)) goto error;
                    if (!ParseAttributeInt32(Archive, SkillIterator->Index, "level", &SkillData->Level)) goto error;
                    if (!ParseAttributeInt32(Archive, SkillIterator->Index, "skill_book", &SkillData->SkillBookID)) goto error;

                    TrainerData->SkillCount += 1;
                    SkillIterator = ArchiveQueryNodeIteratorNext(Archive, SkillIterator);
                }

                Runtime->TrainerDataCount += 1;
            }
        }

        Char MobFilePath[MAX_PATH];
        Char MobFileName[MAX_PATH];
        Char MobScriptFileName[MAX_PATH];
        Char MobScriptFilePath[MAX_PATH];
        sprintf(MobFileName, "world%zu-mmap.xml", World->WorldIndex);
        PathCombine(ServerDirectory, MobFileName, MobFilePath);
        Info("Loading mmap file: %s", MobFilePath);
        if (FileExists(MobFilePath)) {
            if (!ArchiveLoadFromFile(Archive, MobFilePath, false)) goto error;

            Int64 MobParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "world");
            if (MobParentIndex < 0) goto error;

            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, MobParentIndex, "spawn");
            while (ChildIterator) {

                RTMobRef Mob = (RTMobRef)ArrayAppendUninitializedElement(World->MobTable);
                memset(Mob, 0, sizeof(struct _RTMob));

                //Mob->ID.EntityIndex = ArrayGetElementCount(World->MobTable);
                if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "order", &Mob->ID.EntityIndex)) goto error;
                Mob->ID.WorldIndex = World->WorldIndex;
                Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

                if (!ParseAttributeIndex(Archive, ChildIterator->Index, "SpeciesIdx", &Mob->Spawn.MobSpeciesIndex)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "PosX", &Mob->Spawn.AreaX)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "PosY", &Mob->Spawn.AreaY)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Width", &Mob->Spawn.AreaWidth)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Height", &Mob->Spawn.AreaHeight)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpwnInterval", &Mob->Spawn.SpawnInterval)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpawnDefault", &Mob->Spawn.SpawnDefault)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MissionGate", &Mob->Spawn.IsMissionGate)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "PerfectDrop", &Mob->Spawn.PerfectDrop)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Server_Mob", &Mob->Spawn.ServerMobIndex)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Loot_Delay", &Mob->Spawn.LootDelay)) goto error;

                if (ParseAttributeString(Archive, ChildIterator->Index, "Script", MobScriptFileName, MAX_PATH)) {
                    PathCombine(ScriptDirectory, MobScriptFileName, MobScriptFilePath);
                    Mob->Script = RTScriptManagerLoadScript(Runtime->ScriptManager, MobScriptFilePath);
                }

                Mob->SpeciesData = &Runtime->MobData[Mob->Spawn.MobSpeciesIndex];
                Mob->Spawn.Level = Mob->SpeciesData->Level;
                Mob->IsInfiniteSpawn = true;
                Mob->IsPermanentDeath = false;
                Mob->RemainingSpawnCount = 0;

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveClear(Archive, true);
        }

        Char DropWorldFilePath[MAX_PATH];
        Char DropWorldFileName[MAX_PATH];

        sprintf(DropWorldFileName, "world%zu-terrain-world.xml", World->WorldIndex);
        PathCombine(ServerDirectory, DropWorldFileName, DropWorldFilePath);
        Info("Loading terrain file: %s", DropWorldFilePath);
        if (FileExists(DropWorldFilePath)) {
            if (!ArchiveLoadFromFile(Archive, DropWorldFilePath, false)) goto error;

            Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "drops");
            if (ParentIndex < 0) goto error;

            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "item");
            while (ChildIterator) {
                RTDropItemRef DropItem = (RTDropItemRef)ArrayAppendUninitializedElement(World->DropTable.WorldDropPool);
                memset(DropItem, 0, sizeof(struct _RTDropItem));

                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "ItemKind", &DropItem->ItemID.ID)) goto error;
                if (!ParseAttributeUInt64(Archive, ChildIterator->Index, "ItemOpt", &DropItem->ItemOptions)) goto error;

                Float64 ScalarDropRate = 0.0;
                if (!ParseAttributeFloat64(Archive, ChildIterator->Index, "DropRate", &ScalarDropRate)) goto error;
                DropItem->DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MinLv", &DropItem->MinMobLevel)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MaxLv", &DropItem->MaxMobLevel)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "OptPoolIdx", &DropItem->OptionPoolIndex)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "DurationIdx", &DropItem->DurationIndex)) goto error;

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveClear(Archive, true);
        }

        Char DropMobFilePath[MAX_PATH];
        Char DropMobFileName[MAX_PATH]; 

        sprintf(DropMobFileName, "world%zu-terrain-mob.xml", World->WorldIndex);
        PathCombine(ServerDirectory, DropMobFileName, DropMobFilePath);
        Info("Loading terrain mob file: %s", DropMobFilePath);
        if (FileExists(DropMobFilePath)) {
            if (!ArchiveLoadFromFile(Archive, DropMobFilePath, false)) goto error;

            Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "drops");
            if (ParentIndex < 0) goto error;

            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "item");
            while (ChildIterator) {
                Index DropPoolIndex = 0;
                if (!ParseAttributeIndex(Archive, ChildIterator->Index, "SpeciesIdx", &DropPoolIndex)) goto error;
                ArrayRef DropPool = (ArrayRef)DictionaryLookup(World->DropTable.MobDropPool, &DropPoolIndex);
                if (!DropPool) {
                    struct _Array TempArray = { 0 };
                    DictionaryInsert(World->DropTable.MobDropPool, &DropPoolIndex, &TempArray, sizeof(struct _Array));
                    DropPool = DictionaryLookup(World->DropTable.MobDropPool, &DropPoolIndex);
                    ArrayInitializeEmpty(DropPool, Runtime->Allocator, sizeof(struct _RTDropItem), 8);
                }

                RTDropItemRef DropItem = (RTDropItemRef)ArrayAppendUninitializedElement(DropPool);
                memset(DropItem, 0, sizeof(struct _RTDropItem));

                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "ItemKind", &DropItem->ItemID.ID)) goto error;
                if (!ParseAttributeUInt64(Archive, ChildIterator->Index, "ItemOpt", &DropItem->ItemOptions)) goto error;

                Float64 ScalarDropRate = 0.0;
                if (!ParseAttributeFloat64(Archive, ChildIterator->Index, "DropRate", &ScalarDropRate)) goto error;
                DropItem->DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "OptPoolIdx", &DropItem->OptionPoolIndex)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "DurationIdx", &DropItem->DurationIndex)) goto error;

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveClear(Archive, true);
        }

        Char DropQuestFilePath[MAX_PATH] = { 0 };
        Char DropQuestFileDirectory[MAX_PATH] = { 0 };
        Char DropQuestFileName[MAX_PATH] = { 0 };

        sprintf(DropQuestFileName, "World_%zu.xml", World->WorldIndex);
        PathCombine(ServerDirectory, "World", DropQuestFileDirectory);
        PathCombine(DropQuestFileDirectory, DropQuestFileName, DropQuestFilePath);

        Info("Loading terrain quest file: %s", DropQuestFilePath);
        if (FileExists(DropQuestFilePath)) {
            if (!ArchiveLoadFromFile(Archive, DropQuestFilePath, false)) goto error;

            Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "World");
            if (ParentIndex < 0) goto error;

            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "QuestDropPool");
            while (ChildIterator) {
                Index DropPoolIndex = 0;
                if (!ParseAttributeIndex(Archive, ChildIterator->Index, "MobSpeciesIndex", &DropPoolIndex)) goto error;

                ArrayRef DropPool = (ArrayRef)DictionaryLookup(World->DropTable.QuestDropPool, &DropPoolIndex);
                if (!DropPool) {
                    struct _Array TempArray = { 0 };
                    DictionaryInsert(World->DropTable.QuestDropPool, &DropPoolIndex, &TempArray, sizeof(struct _Array));
                    DropPool = DictionaryLookup(World->DropTable.QuestDropPool, &DropPoolIndex);
                    ArrayInitializeEmpty(DropPool, Runtime->Allocator, sizeof(struct _RTDropItem), 8);
                }

                ArchiveIteratorRef ItemIterator = ArchiveQueryNodeIteratorFirst(Archive, ChildIterator->Index, "Item");
                while (ItemIterator) {
                    RTDropItemRef DropItem = (RTDropItemRef)ArrayAppendUninitializedElement(DropPool);
                    memset(DropItem, 0, sizeof(struct _RTDropItem));

                    if (!ParseAttributeUInt64(Archive, ItemIterator->Index, "ItemID", &DropItem->ItemID.Serial)) goto error;
                    if (!ParseAttributeUInt64(Archive, ItemIterator->Index, "ItemOption", &DropItem->ItemOptions)) goto error;
                    if (!ParseAttributeUInt32(Archive, ItemIterator->Index, "ItemDuration", &DropItem->DurationIndex)) goto error;
                    if (!ParseAttributeUInt32(Archive, ItemIterator->Index, "OptionPoolID", &DropItem->OptionPoolIndex)) goto error;

                    DropItem->ItemID.IsCharacterBinding = true;
                    DropItem->ItemOptions = RTQuestItemOptions(DropItem->ItemOptions, 1);

                    Float64 ScalarDropRate = 0.0;
                    if (!ParseAttributeFloat64(Archive, ItemIterator->Index, "Rate", &ScalarDropRate)) goto error;
                    DropItem->DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);

                    ItemIterator = ArchiveQueryNodeIteratorNext(Archive, ItemIterator);
                }

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveClear(Archive, true);
        }
    }

    for (Index WorldIndex = 0; WorldIndex < Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!RTWorldDataExists(Runtime->WorldManager, WorldIndex)) continue;

        RTWorldDataRef World = RTWorldDataGet(Runtime->WorldManager, WorldIndex);

        Int64 NodeIndex = ArchiveQueryNodeWithAttribute(
            MainArchive,
            -1,
            "cabal.cabal_world.world",
            "id",
            UInt64ToStringNoAlloc(World->WorldIndex)
        );

        if (NodeIndex < 0) continue;

        Index WorldIndex = 0;
        if (!ParseAttributeIndex(MainArchive, NodeIndex, "id", &WorldIndex)) continue;
        assert(WorldIndex == World->WorldIndex);
    }

    ArchiveDestroy(Archive);
    return true;

error:
    for (Index WorldIndex = 0; WorldIndex < Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!RTWorldDataExists(Runtime->WorldManager, WorldIndex)) continue;

        RTWorldDataRef World = RTWorldDataGet(Runtime->WorldManager, WorldIndex);
        ArrayDestroy(World->MobTable);
    }

    ArchiveDestroy(Archive);
    return false;
}

Bool ServerLoadSkillData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef SkillArchive
) {
    RTRuntimeRef Runtime = Context->Runtime;
    ArchiveRef Archive = SkillArchive;
    Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal_skill.new_skill_list");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "skill_main");
    while (Iterator) {
        Int32 SkillIndex = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &SkillIndex)) goto error;

        RTCharacterSkillDataRef SkillData = MemoryPoolReserve(Runtime->SkillDataPool, SkillIndex);
        SkillData->SkillID = SkillIndex;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "type", &SkillData->SkillType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "group", &SkillData->SkillGroup)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "multi", &SkillData->Multi)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "intensity", &SkillData->Intensity)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "element", &SkillData->Element)) goto error;
        
        ArchiveIteratorRef AttributeIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "attribute");
        if (AttributeIterator) {
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "target", &SkillData->Target)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "max_target", &SkillData->MaxTarget)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "reach", &SkillData->Reach)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "mreach", &SkillData->MReach)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "range", &SkillData->Range)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "range_type", &SkillData->RangeType)) goto error;

            ArchiveIteratorRef CodeIterator = ArchiveQueryNodeIteratorFirst(Archive, AttributeIterator->Index, "attribute");
            if (CodeIterator) {
                if (!ParseAttributeInt32(Archive, CodeIterator->Index, "grip_calc", &SkillData->GripCalc)) goto error;
                if (!ParseAttributeInt32(Archive, CodeIterator->Index, "anicode", &SkillData->AniCode)) goto error;
                if (!ParseAttributeInt32(Archive, CodeIterator->Index, "fxcode_on", &SkillData->FxCodeOn)) goto error;
                if (!ParseAttributeInt32(Archive, CodeIterator->Index, "fxcode_keep", &SkillData->FxCodeKeep)) goto error;
            }

            ArchiveIteratorRef FrameIterator = ArchiveQueryNodeIteratorFirst(Archive, AttributeIterator->Index, "frame");
            if (FrameIterator) {
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "term", &SkillData->FrameTerm)) goto error;
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "blendlimit", &SkillData->BlendLimit)) goto error;
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "hit_frame", &SkillData->HitFrame)) goto error;
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "firing_frame", &SkillData->FiringFrame)) goto error;
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "ani_f", &SkillData->AniFrame)) goto error;
            }

            ArchiveIteratorRef EffectIterator = ArchiveQueryNodeIteratorFirst(Archive, AttributeIterator->Index, "effect");
            if (EffectIterator) {
                if (!ParseAttributeInt32(Archive, EffectIterator->Index, "visual_type", &SkillData->VisualType)) goto error;
            }

            ArchiveIteratorRef ConditionIterator = ArchiveQueryNodeIteratorFirst(Archive, AttributeIterator->Index, "condition");
            if (ConditionIterator) {
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "twohand", &SkillData->ConditionTwoHand)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "dual", &SkillData->ConditionDual)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "onehand", &SkillData->ConditionOneHand)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "bracer", &SkillData->ConditionBracer)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "onechakram", &SkillData->ConditionOneChakram)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "twochakram", &SkillData->ConditionTwoChakram)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "exclusive", &SkillData->ConditionExclusive)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "usecase", &SkillData->ConditionUseCase)) goto error;
            }
        }

        ArchiveIteratorRef ParamIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "skill_param");
        if (ParamIterator) {
            if (!ParseAttributeInt32(Archive, ParamIterator->Index, "exp1", &SkillData->SkillExp1)) goto error;
            if (!ParseAttributeInt32(Archive, ParamIterator->Index, "exp2", &SkillData->SkillExp2)) goto error;

            ArchiveIteratorRef PowerIterator = ArchiveQueryNodeIteratorFirst(Archive, ParamIterator->Index, "power");
            if (PowerIterator) {
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "samp", SkillData->SAmp, 2, ',')) goto error;
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "atk", SkillData->Atk, 3, ',')) goto error;
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "shit", SkillData->SHit, 2, ',')) goto error;
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "spenet", SkillData->SPenet, 2, ',')) goto error;
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "critdmg", SkillData->CritDmg, 2, ',')) goto error;
            }

            ArchiveIteratorRef CostIterator = ArchiveQueryNodeIteratorFirst(Archive, ParamIterator->Index, "cost");
            if (CostIterator) {
                if (!ParseAttributeInt32(Archive, CostIterator->Index, "cooltime_id", &SkillData->CooltimeID)) goto error;
                if (!ParseAttributeInt32Array(Archive, CostIterator->Index, "mp", SkillData->Mp, 2, ',')) goto error;
                if (!ParseAttributeInt32(Archive, CostIterator->Index, "sp", &SkillData->Sp)) goto error;
                if (!ParseAttributeInt32(Archive, CostIterator->Index, "rage_value", &SkillData->RageValue)) goto error;
            }
        }

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "skill_value");
    while (Iterator) {
        Int32 SkillIndex = 0;
        Int32 Group = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &SkillIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "group", &Group)) goto error;

        RTCharacterSkillDataRef SkillData = MemoryPoolFetch(Runtime->SkillDataPool, SkillIndex);
        assert(SkillData);

        ArchiveIteratorRef ValueIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "value");
        while (ValueIterator) {
            assert(SkillData->SkillValueCount < RUNTIME_MEMORY_MAX_SKILL_VALUE_DATA_COUNT);

            RTSkillValueDataRef SkillValue = &SkillData->SkillValues[SkillData->SkillValueCount];
            memset(SkillValue, 0, sizeof(struct _RTSkillValueData));

            if (!ParseAttributeInt32(Archive, ValueIterator->Index, "bforce_id", &SkillValue->ForceEffectIndex)) goto error;
            if (!ParseAttributeInt32Array(Archive, ValueIterator->Index, "bforce_value", SkillValue->ForceEffectValue, 3, ',')) goto error;
            if (!ParseAttributeInt32(Archive, ValueIterator->Index, "type", &SkillValue->Type)) goto error;
            if (!ParseAttributeInt32(Archive, ValueIterator->Index, "value_type", &SkillValue->ValueType)) goto error;
            if (!ParseAttributeInt32(Archive, ValueIterator->Index, "check_readiness", &SkillValue->CheckReadiness)) goto error;

            ParseAttributeInt32Array(Archive, ValueIterator->Index, "power", SkillValue->Power, 2, ',');
            ParseAttributeInt32Array(Archive, ValueIterator->Index, "dur", SkillValue->Duration, 2, ',');

            SkillData->SkillValueCount += 1;
            ValueIterator = ArchiveQueryNodeIteratorNext(Archive, ValueIterator);
        }

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "skill_order");
    while (Iterator) {
        Int32 SkillID;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &SkillID)) goto error;

        RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillID);
        assert(SkillData);

        assert(SkillData->SkillLevelCount < RUNTIME_MEMORY_MAX_SKILL_LEVEL_DATA_COUNT);

        RTSkillLevelDataRef SkillLevelData = &SkillData->SkillLevels[SkillData->SkillLevelCount];
        SkillLevelData->SkillID = SkillID;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "order_level", &SkillLevelData->Index)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "rank", &SkillLevelData->SkillRank)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "rankpoint", &SkillLevelData->SkillRankPoint)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "start_level", &SkillLevelData->StartLevel)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "end_level", &SkillLevelData->EndLevel)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "skill_point", &SkillLevelData->SkillPoint)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "train_price", &SkillLevelData->TrainPrice)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "untrain_price", &SkillLevelData->UntrainPrice)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "stylemastery", &SkillLevelData->StyleMastery)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "twohand", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dual", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic_arrow", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "sword_shield", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic_sword", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "chakram", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic_gun", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic_spirit", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM - 1])) goto error;
    
        SkillData->SkillLevelCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadDungeonExtraData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    RTDungeonDataRef DungeonData
) {
    ArchiveRef Archive = ArchiveCreateEmpty(Context->Runtime->Allocator);

    Char FilePath[MAX_PATH] = { 0 };
    sprintf(FilePath, "%s\\Dungeon\\Dungeon_%d.xml", ServerDirectory, DungeonData->DungeonIndex);

    if (ArchiveLoadFromFile(Archive, FilePath, false)) {
        Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "Dungeon");
        if (ParentIndex < 0) goto error;

        ArchiveIteratorRef GroupIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "TriggerGroup");
        while (GroupIterator) {
            Index TriggerGroupIndex = 0;
            if (!ParseAttributeIndex(Archive, GroupIterator->Index, "Index", &TriggerGroupIndex)) {
                Error("Loading '%s' in '%s' failed!", "Index", FilePath);
                goto error;
            }
            assert(!DictionaryLookup(DungeonData->TriggerGroups, &TriggerGroupIndex));

            struct _Array TempArray = { 0 };
            DictionaryInsert(DungeonData->TriggerGroups, &TriggerGroupIndex, &TempArray, sizeof(struct _Array));
            ArrayRef TriggerGroup = DictionaryLookup(DungeonData->TriggerGroups, &TriggerGroupIndex);
            ArrayInitializeEmpty(TriggerGroup, Context->Runtime->Allocator, sizeof(struct _RTDungeonTriggerData), 8);

            ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, GroupIterator->Index, "Trigger");
            while (Iterator) {
                RTDungeonTriggerDataRef TriggerData = ArrayAppendUninitializedElement(TriggerGroup);
                memset(TriggerData, 0, sizeof(struct _RTDungeonTriggerData));

                if (!ParseAttributeInt32(Archive, Iterator->Index, "Type", &TriggerData->Type)) {
                    Error("Loading '%s' in '%s' failed!", "Type", FilePath);
                    goto error;
                }

                TriggerData->LiveMobCount = ParseAttributeInt32ArrayCounted(
                    Archive,
                    Iterator->Index,
                    "LiveMobIndexList",
                    TriggerData->LiveMobIndexList,
                    RUNTIME_DUNGEON_TRIGGER_MAX_MOB_COUNT,
                    ','
                );

                TriggerData->DeadMobCount = ParseAttributeInt32ArrayCounted(
                    Archive,
                    Iterator->Index,
                    "DeadMobIndexList",
                    TriggerData->DeadMobIndexList,
                    RUNTIME_DUNGEON_TRIGGER_MAX_MOB_COUNT,
                    ','
                );

                if (!ParseAttributeInt32(Archive, Iterator->Index, "NpcIndex", &TriggerData->NpcIndex)) {
                    Error("Loading '%s' in '%s' failed!", "NpcIndex", FilePath);
                    goto error;
                }

                if (!ParseAttributeInt32(Archive, Iterator->Index, "ActionGroupIndex", &TriggerData->ActionGroupIndex)) {
                    Error("Loading '%s' in '%s' failed!", "ActionGroupIndex", FilePath);
                    goto error;
                }

                Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
            }

            GroupIterator = ArchiveQueryNodeIteratorNext(Archive, GroupIterator);
        }

        GroupIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "ActionGroup");
        while (GroupIterator) {
            Index ActionGroupIndex = 0;
            if (!ParseAttributeIndex(Archive, GroupIterator->Index, "Index", &ActionGroupIndex)) {
                Error("Loading '%s' in '%s' failed!", "Index", FilePath);
                goto error;
            }
            assert(!DictionaryLookup(DungeonData->ActionGroups, &ActionGroupIndex));

            struct _Array TempArray = { 0 };
            DictionaryInsert(DungeonData->ActionGroups, &ActionGroupIndex, &TempArray, sizeof(struct _Array));
            ArrayRef ActionGroup = DictionaryLookup(DungeonData->ActionGroups, &ActionGroupIndex);
            ArrayInitializeEmpty(ActionGroup, Context->Runtime->Allocator, sizeof(struct _RTDungeonTriggerActionData), 8);

            ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, GroupIterator->Index, "Action");
            while (Iterator) {
                RTDungeonTriggerActionDataRef ActionData = ArrayAppendUninitializedElement(ActionGroup);
                memset(ActionData, 0, sizeof(struct _RTDungeonTriggerActionData));

                if (!ParseAttributeUInt64(Archive, Iterator->Index, "Delay", &ActionData->Delay)) {
                    Error("Loading '%s' in '%s' failed!", "Delay", FilePath);
                    goto error;
                }

                if (!ParseAttributeInt32(Archive, Iterator->Index, "TargetMobIndex", &ActionData->TargetMobIndex)) {
                    Error("Loading '%s' in '%s' failed!", "TargetMobIndex", FilePath);
                    goto error;
                }

                if (!ParseAttributeInt32(Archive, Iterator->Index, "TargetAction", &ActionData->TargetAction)) {
                    Error("Loading '%s' in '%s' failed!", "TargetAction", FilePath);
                    goto error;
                }

                if (!ParseAttributeInt32(Archive, Iterator->Index, "TargetSpawnInterval", &ActionData->TargetSpawnInterval)) {
                    Error("Loading '%s' in '%s' failed!", "TargetSpawnInterval", FilePath);
                    goto error;
                }

                if (!ParseAttributeInt32(Archive, Iterator->Index, "TargetSpawnCount", &ActionData->TargetSpawnCount)) {
                    Error("Loading '%s' in '%s' failed!", "TargetSpawnCount", FilePath);
                    goto error;
                }

                Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
            }

            GroupIterator = ArchiveQueryNodeIteratorNext(Archive, GroupIterator);
        }

        struct _RTDungeonTimeControlData TimeControlData = { 0 };

        GroupIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "TimeControl");
        while (GroupIterator) {
            Index MobIndex = 0;
            if (!ParseAttributeIndex(Archive, GroupIterator->Index, "MobIndex", &MobIndex)) {
                Error("Loading '%s' in '%s' failed!", "MobIndex", FilePath);
                goto error;
            }

            if (DictionaryLookup(DungeonData->TimeControls, &MobIndex)) {
                Error("MobIndex %llu already has a TimeControl in Dungeon %d!", MobIndex, DungeonData->DungeonIndex);
                GroupIterator = ArchiveQueryNodeIteratorNext(Archive, GroupIterator);
                continue;
            }

            if (!ParseAttributeInt32(Archive, GroupIterator->Index, "Event", &TimeControlData.Event)) {
                Error("Loading '%s' in '%s' failed!", "Event", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, GroupIterator->Index, "Value", &TimeControlData.Value)) {
                Error("Loading '%s' in '%s' failed!", "Value", FilePath);
                goto error;
            }

            DictionaryInsert(DungeonData->TimeControls, &MobIndex, &TimeControlData, sizeof(struct _RTDungeonTimeControlData));

            GroupIterator = ArchiveQueryNodeIteratorNext(Archive, GroupIterator);
        }

        GroupIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "WorldDropPool.Item");
        while (GroupIterator) {
            RTDropItemRef DropItem = (RTDropItemRef)ArrayAppendUninitializedElement(DungeonData->DropTable.WorldDropPool);
            memset(DropItem, 0, sizeof(struct _RTDropItem));

            if (!ParseAttributeUInt64(Archive, GroupIterator->Index, "ItemID", &DropItem->ItemID.Serial)) {
                Error("Loading '%s' in '%s' failed!", "ItemID", FilePath);
                goto error;
            }
            if (!ParseAttributeUInt64(Archive, GroupIterator->Index, "ItemOption", &DropItem->ItemOptions)) {
                Error("Loading '%s' in '%s' failed!", "ItemOption", FilePath);
                goto error;
            }
            if (!ParseAttributeInt32(Archive, GroupIterator->Index, "ItemDuration", &DropItem->DurationIndex)) {
                Error("Loading '%s' in '%s' failed!", "ItemDuration", FilePath);
                goto error;
            }
            if (!ParseAttributeInt32(Archive, GroupIterator->Index, "MinMobLevel", &DropItem->MinMobLevel)) {
                Error("Loading '%s' in '%s' failed!", "MinMobLevel", FilePath);
                goto error;
            }
            if (!ParseAttributeInt32(Archive, GroupIterator->Index, "MaxMobLevel", &DropItem->MaxMobLevel)) {
                Error("Loading '%s' in '%s' failed!", "MaxMobLevel", FilePath);
                goto error;
            }
            if (!ParseAttributeInt32(Archive, GroupIterator->Index, "OptionPoolID", &DropItem->OptionPoolIndex)) {
                Error("Loading '%s' in '%s' failed!", "OptionPoolID", FilePath);
                goto error;
            }
            
            Float64 ScalarDropRate = 0.0;
            if (!ParseAttributeFloat64(Archive, GroupIterator->Index, "Rate", &ScalarDropRate)) {
                Error("Loading '%s' in '%s' failed!", "Rate", FilePath);
                goto error;
            }
            DropItem->DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);

            GroupIterator = ArchiveQueryNodeIteratorNext(Archive, GroupIterator);
        }

        GroupIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "MobDropPool");
        while (GroupIterator) {
            Index MobSpeciesIndex = 0;
            if (!ParseAttributeIndex(Archive, GroupIterator->Index, "MobSpeciesIndex", &MobSpeciesIndex)) goto error;

            Index DropPoolIndex = MobSpeciesIndex;
            ArrayRef DropPool = DictionaryLookup(DungeonData->DropTable.MobDropPool, &DropPoolIndex);
            if (!DropPool) {
                struct _Array TempArray = { 0 };
                DictionaryInsert(DungeonData->DropTable.MobDropPool, &DropPoolIndex, &TempArray, sizeof(struct _Array));
                DropPool = DictionaryLookup(DungeonData->DropTable.MobDropPool, &DropPoolIndex);
                ArrayInitializeEmpty(DropPool, Context->Runtime->Allocator, sizeof(struct _RTDropItem), 8);
            }

            ArchiveIteratorRef ItemIterator = ArchiveQueryNodeIteratorFirst(Archive, GroupIterator->Index, "Item");
            while (ItemIterator) {
                RTDropItemRef DropItem = (RTDropItemRef)ArrayAppendUninitializedElement(DropPool);
                memset(DropItem, 0, sizeof(struct _RTDropItem));

                if (!ParseAttributeUInt64(Archive, ItemIterator->Index, "ItemID", &DropItem->ItemID.Serial)) {
                    Error("Loading '%s' in '%s' failed!", "ItemID", FilePath);
                    goto error;
                }
                if (!ParseAttributeUInt64(Archive, ItemIterator->Index, "ItemOption", &DropItem->ItemOptions)) {
                    Error("Loading '%s' in '%s' failed!", "ItemOption", FilePath);
                    goto error;
                }
                if (!ParseAttributeInt32(Archive, ItemIterator->Index, "ItemDuration", &DropItem->DurationIndex)) {
                    Error("Loading '%s' in '%s' failed!", "ItemDuration", FilePath);
                    goto error;
                }
                if (!ParseAttributeInt32(Archive, ItemIterator->Index, "OptionPoolID", &DropItem->OptionPoolIndex)) {
                    Error("Loading '%s' in '%s' failed!", "OptionPoolID", FilePath);
                    goto error;
                }

                Float64 ScalarDropRate = 0.0;
                if (!ParseAttributeFloat64(Archive, ItemIterator->Index, "Rate", &ScalarDropRate)) {
                    Error("Loading '%s' in '%s' failed!", "Rate", FilePath);
                    goto error;
                }
                DropItem->DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);
                DropItem->MinMobLevel = 0;
                DropItem->MaxMobLevel = 1000;

                ItemIterator = ArchiveQueryNodeIteratorNext(Archive, ItemIterator);
            }

            GroupIterator = ArchiveQueryNodeIteratorNext(Archive, GroupIterator);
        }
    }

    ArchiveDestroy(Archive);
    return true;

error:
    ArchiveDestroy(Archive);
    return false;
}

Bool ServerLoadPatternPartData(
    ServerContextRef Context,
    CString FilePath
) {
    RTRuntimeRef Runtime = Context->Runtime;
    ArchiveRef Archive = ArchiveCreateEmpty(Context->Runtime->Allocator);

    if (ArchiveLoadFromFile(Archive, FilePath, false)) {
        Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "PatternPart");
        if (ParentIndex < 0) 
            goto error;

        Index PatternPartIndex = 0;
        if (!ParseAttributeIndex(Archive, ParentIndex, "Index", &PatternPartIndex)) 
            goto error;

        RTMissionDungeonPatternPartDataRef PatternPartData = (RTMissionDungeonPatternPartDataRef)DictionaryLookup(Runtime->PatternPartData, &PatternPartIndex);
        if (!PatternPartData) 
            goto error;

        ArchiveIteratorRef MobIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "Mob");
        while (MobIterator) {
            RTMobRef Mob = (RTMobRef)ArrayAppendUninitializedElement(PatternPartData->MobTable);
            memset(Mob, 0, sizeof(struct _RTMob));
            Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;
            Mob->Spawn.PatternPartIndex = PatternPartIndex;

            if (!ParseAttributeUInt16(Archive, MobIterator->Index, "MobIndex", &Mob->ID.EntityIndex)) {
                Error("Loading '%s' in '%s' failed!", "MobIndex", FilePath);
                goto error;
            }

            if (!ParseAttributeIndex(Archive, MobIterator->Index, "SpeciesIndex", &Mob->Spawn.MobSpeciesIndex)) {
                Error("Loading '%s' in '%s' failed!", "SpeciesIndex", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "X", &Mob->Spawn.AreaX)) {
                Error("Loading '%s' in '%s' failed!", "X", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "Y", &Mob->Spawn.AreaY)) {
                Error("Loading '%s' in '%s' failed!", "Y", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "Width", &Mob->Spawn.AreaWidth)) {
                Error("Loading '%s' in '%s' failed!", "Width", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "Height", &Mob->Spawn.AreaHeight)) {
                Error("Loading '%s' in '%s' failed!", "Height", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "SpawnInterval", &Mob->Spawn.SpawnInterval)) {
                Error("Loading '%s' in '%s' failed!", "SpawnInterval", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "SpawnCount", &Mob->Spawn.SpawnCount)) {
                Error("Loading '%s' in '%s' failed!", "SpawnCount", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "SpawnDefault", &Mob->Spawn.SpawnDefault)) {
                Error("Loading '%s' in '%s' failed!", "SpawnDefault", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "Level", &Mob->Spawn.Level)) {
                Error("Loading '%s' in '%s' failed!", "Level", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "PerfectDrop", &Mob->Spawn.PerfectDrop)) {
                Error("Loading '%s' in '%s' failed!", "PerfectDrop", FilePath);
                goto error;
            }

            ParseAttributeInt32(Archive, MobIterator->Index, "SpawnTriggerIndex", &Mob->Spawn.SpawnTriggerID);
            ParseAttributeInt32(Archive, MobIterator->Index, "KillTriggerIndex", &Mob->Spawn.KillTriggerID);
            ParseAttributeInt32(Archive, MobIterator->Index, "ServerMobIndex", &Mob->Spawn.ServerMobIndex);
            ParseAttributeInt32(Archive, MobIterator->Index, "LootDelay", &Mob->Spawn.LootDelay);

            ParseAttributeInt32Array(Archive, MobIterator->Index, "EventProperty", Mob->Spawn.EventProperty, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
            ParseAttributeInt32Array(Archive, MobIterator->Index, "EventMobList", Mob->Spawn.EventMobs, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
            ParseAttributeInt32Array(Archive, MobIterator->Index, "EventInterval", Mob->Spawn.EventInterval, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
            ParseAttributeString(Archive, MobIterator->Index, "Script", Mob->Spawn.Script, MAX_PATH);

            Mob->SpeciesData = &Context->Runtime->MobData[Mob->Spawn.MobSpeciesIndex];
            Mob->IsInfiniteSpawn = true;
            Mob->IsPermanentDeath = false;
            Mob->RemainingSpawnCount = 0;

            MobIterator = ArchiveQueryNodeIteratorNext(Archive, MobIterator);
        }
    }

    ArchiveDestroy(Archive);
    return true;

error:
    ArchiveDestroy(Archive);
    return false;
}

Bool ServerLoadQuestDungeonData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef Archive
) {
    RTRuntimeRef Runtime = Context->Runtime;
    ArchiveRef TempArchive = ArchiveCreateEmpty(Runtime->Allocator);

    Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_quest_dungeon");
    if (ParentIndex < 0) goto error;

    struct _RTDungeonData DungeonData = { 0 };

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        Index DungeonIndex = 0;
        if (!ParseAttributeIndex(Archive, Iterator->Index, "id", &DungeonIndex)) goto error;
        assert(!DictionaryLookup(Runtime->DungeonData, &DungeonIndex));
        memset(&DungeonData, 0, sizeof(struct _RTDungeonData));
        DungeonData.DungeonIndex = DungeonIndex;

        Int32 EntryItem[2];
        if (ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_item", EntryItem, 2, ':')) {
            DungeonData.EntryItemID.ID = EntryItem[0];
            DungeonData.EntryItemOption = EntryItem[1];
        }

        ParseAttributeInt32(Archive, Iterator->Index, "create_type", &DungeonData.CreateType);
        ParseAttributeInt32(Archive, Iterator->Index, "enter_type", &DungeonData.EntryType);
        ParseAttributeInt32(Archive, Iterator->Index, "user_type", &DungeonData.PlayerType);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "user_count", &DungeonData.MaxPlayerCount)) {
            DungeonData.MaxPlayerCount = 1;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "ObjCount", &DungeonData.ObjectCount);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "RemoveItem", &DungeonData.RemoveItem)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_cd_level", &DungeonData.EntryConditionLevel)) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_class", DungeonData.EntryConditionClass, 2, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_warp_id", &DungeonData.EntryWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "fail_warp_npc_id", &DungeonData.FailWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dead_warp_id", &DungeonData.DeadWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "penalty_value", &DungeonData.PenaltyValue)) goto error;

        DungeonData.PatternPartCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "pp_id",
            DungeonData
            .PatternPartIndices,
            RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT,
            ','
        );

        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "reward", DungeonData.Reward, 8, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "success_warp_npc_id", &DungeonData.SuccessWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData.WorldIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData.NextDungeonIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData.DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData.IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData.EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData.MissionTimeout)) goto error;

        RTWorldDataRef WorldData = RTWorldDataGet(Runtime->WorldManager, DungeonData.WorldIndex);
        WorldData->HasQuestDungeon = true;

        DungeonData.TriggerGroups = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData.ActionGroups = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData.TimeControls = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData.DropTable.WorldDropPool = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTDropItem), 8);
        DungeonData.DropTable.MobDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData.DropTable.QuestDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);
        DictionaryInsert(Runtime->DungeonData, &DungeonIndex, &DungeonData, sizeof(struct _RTDungeonData));

        if (!ServerLoadDungeonExtraData(Context, RuntimeDirectory, ServerDirectory, &DungeonData)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    struct _RTMissionDungeonPatternPartData PatternPartData = { 0 };

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        Index PatternPartIndex = 0;
        if (!ParseAttributeIndex(Archive, Iterator->Index, "id", &PatternPartIndex)) goto error;
        assert(!DictionaryLookup(Runtime->PatternPartData, &PatternPartIndex));
        memset(&PatternPartData, 0, sizeof(struct _RTMissionDungeonPatternPartData));
        PatternPartData.MobTable = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMob), 8);

        struct {
            Int32 MobID;
            Int32 MaxCount;
        } MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];

        PatternPartData.MissionMobCount = ParseAttributeInt32Array2D(
            Archive,
            Iterator->Index,
            "mission_mobs",
            (Int32*)&MissionMobs[0],
            2, // NOTE: The old quest dungeon data has old format for mission mobs field.
            RUNTIME_MAX_QUEST_COUNTER_COUNT,
            ':',
            ','
        );

        for (Index Index = 0; Index < PatternPartData.MissionMobCount; Index += 1) {
            PatternPartData.MissionMobs[Index].MobID = MissionMobs[Index].MobID;
            PatternPartData.MissionMobs[Index].Count = MissionMobs[Index].MaxCount;
            PatternPartData.MissionMobs[Index].MaxCount = MissionMobs[Index].MaxCount;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "mission_npc", &PatternPartData.MissionNpcIndex);
        DictionaryInsert(Runtime->PatternPartData, &PatternPartIndex, &PatternPartData, sizeof(struct _RTMissionDungeonPatternPartData));

        Char FilePath[MAX_PATH] = { 0 };
        sprintf(FilePath, "%s\\Dungeon\\PatternPart_%llu.xml", ServerDirectory, PatternPartIndex);
        if (!ServerLoadPatternPartData(Context, FilePath)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveDestroy(TempArchive);
    return true;

error:
    ArchiveDestroy(TempArchive);
    return false;
}

Bool ServerLoadMissionDungeonData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef Archive
) {
    RTRuntimeRef Runtime = Context->Runtime;
    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_mission_dungeon");
    if (ParentIndex < 0) goto error;

    struct _RTDungeonData DungeonData = { 0 };

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        Index DungeonIndex = 0;
        if (!ParseAttributeIndex(Archive, Iterator->Index, "id", &DungeonIndex)) goto error;
        assert(!DictionaryLookup(Runtime->DungeonData, &DungeonIndex));
        memset(&DungeonData, 0, sizeof(struct _RTDungeonData));
        DungeonData.DungeonIndex = DungeonIndex;

        Int32 EntryItem[2];
        if (ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_item", EntryItem, 2, ':')) {
            DungeonData.EntryItemID.ID = EntryItem[0];
            DungeonData.EntryItemOption = EntryItem[1];
        }

        ParseAttributeInt32(Archive, Iterator->Index, "create_type", &DungeonData.CreateType);
        ParseAttributeInt32(Archive, Iterator->Index, "enter_type", &DungeonData.EntryType);
        ParseAttributeInt32(Archive, Iterator->Index, "user_type", &DungeonData.PlayerType);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "user_count", &DungeonData.MaxPlayerCount)) {
            DungeonData.MaxPlayerCount = 1;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "ObjCount", &DungeonData.ObjectCount);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "RemoveItem", &DungeonData.RemoveItem)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_cd_level", &DungeonData.EntryConditionLevel)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "max_user", &DungeonData.MaxPlayerCount)) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_class", DungeonData.EntryConditionClass, 2, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_warp_id", &DungeonData.EntryWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "fail_warp_npc_id", &DungeonData.FailWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dead_warp_id", &DungeonData.DeadWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &DungeonData.DungeonIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "penalty_value", &DungeonData.PenaltyValue)) goto error;

        DungeonData.PatternPartCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "pp_id",
            DungeonData.PatternPartIndices,
            RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT,
            ','
        );

        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "reward", DungeonData.Reward, 8, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "success_warp_npc_id", &DungeonData.SuccessWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData.WorldIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData.NextDungeonIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "warpnpc_set", &DungeonData.WarpNpcSetID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useoddcircle_count", &DungeonData.UseOddCircleCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData.DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useveradrix_count", &DungeonData.UseVeradrixCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData.IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData.EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData.MissionTimeout)) goto error;

        RTWorldDataRef WorldData = RTWorldDataGet(Runtime->WorldManager, DungeonData.WorldIndex);
        WorldData->HasMissionDungeon = true;

        DungeonData.TriggerGroups = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData.ActionGroups = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData.TimeControls = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData.DropTable.WorldDropPool = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTDropItem), 8);
        DungeonData.DropTable.MobDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData.DropTable.QuestDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);
        DictionaryInsert(Runtime->DungeonData, &DungeonIndex, &DungeonData, sizeof(struct _RTDungeonData));

        if (!ServerLoadDungeonExtraData(Context, RuntimeDirectory, ServerDirectory, &DungeonData)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    struct _RTMissionDungeonPatternPartData PatternPartData = { 0 };

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        Index PatternPartIndex = 0;
        if (!ParseAttributeIndex(Archive, Iterator->Index, "id", &PatternPartIndex)) goto error;
        assert(!DictionaryLookup(Runtime->PatternPartData, &PatternPartIndex));
        memset(&PatternPartData, 0, sizeof(struct _RTMissionDungeonPatternPartData));

        PatternPartData.MobTable = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMob), 8);
        PatternPartData.MissionMobCount = ParseAttributeInt32Array2D(
            Archive,
            Iterator->Index,
            "mission_mobs",
            (Int32*)&PatternPartData.MissionMobs[0],
            sizeof(struct _RTQuestUnitMobData) / sizeof(Int32),
            RUNTIME_MAX_QUEST_COUNTER_COUNT,
            ':',
            ','
        );

        ParseAttributeInt32(Archive, Iterator->Index, "mission_npc", &PatternPartData.MissionNpcIndex);

        DictionaryInsert(Runtime->PatternPartData, &PatternPartIndex, &PatternPartData, sizeof(struct _RTMissionDungeonPatternPartData));

        Char FilePath[MAX_PATH] = { 0 };
        sprintf(FilePath, "%s\\Dungeon\\PatternPart_%llu.xml", ServerDirectory, PatternPartIndex);
        if (!ServerLoadPatternPartData(Context, FilePath)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadDungeonData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef Cont1Archive,
    ArchiveRef Cont2Archive,
    ArchiveRef Cont3Archive
) {
    if (!ServerLoadQuestDungeonData(Context, RuntimeDirectory, ServerDirectory, Cont1Archive)) return false;
    if (!ServerLoadMissionDungeonData(Context, RuntimeDirectory, ServerDirectory, Cont2Archive)) return false;
    if (!ServerLoadMissionDungeonData(Context, RuntimeDirectory, ServerDirectory, Cont3Archive)) return false;

    return true;
}

Bool ServerLoadWorldDropData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    RTRuntimeRef Runtime = Context->Runtime;
    ArchiveRef Archive = ArchiveCreateEmpty(AllocatorGetSystemDefault());

    Char FilePath[MAX_PATH];
    PathCombine(ServerDirectory, "WorldDrop.xml", FilePath);

    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "WorldDrop");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "Item");
    while (Iterator) {
        Int32 WorldIndex = -1;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Terrain_World", &WorldIndex)) goto error;

        if (!RTWorldDataExists(Runtime->WorldManager, WorldIndex)) {
            Error("WorldDrop: World %d not found!", WorldIndex);
            Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
            continue;
        }

        RTWorldDataRef World = RTWorldDataGet(Runtime->WorldManager, WorldIndex);
        RTDropTableRef DropTable = &World->DropTable;
        
        Index DungeonIndex = UINT64_MAX;
        if (!ParseAttributeIndex(Archive, Iterator->Index, "DungeonID", &DungeonIndex)) goto error;

        if (DungeonIndex != UINT64_MAX && DungeonIndex != 0) {
            if (!World->HasQuestDungeon && !World->HasMissionDungeon) {
                Error("WorldDrop: World %d has no dungeon %llu!", WorldIndex, DungeonIndex);
                Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
                continue;
            }

            RTDungeonDataRef DungeonData = (RTDungeonDataRef)DictionaryLookup(Runtime->DungeonData, &DungeonIndex);
            DropTable = &DungeonData->DropTable;

            if (!DungeonData) {
                Error("WorldDrop: World %d has no dungeon %llu!", WorldIndex, DungeonIndex);
                Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
                continue;
            }
        }

        Int32 MobSpeciesIndex = -1;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Terrain_Mob", &MobSpeciesIndex)) goto error;

        RTDropItemRef DropItem = NULL;
        if (MobSpeciesIndex > 0) {
            Index DropPoolIndex = MobSpeciesIndex;
            ArrayRef DropPool = DictionaryLookup(World->DropTable.MobDropPool, &DropPoolIndex);
            if (!DropPool) {
                struct _Array TempArray = { 0 };
                DictionaryInsert(World->DropTable.MobDropPool, &DropPoolIndex, &TempArray, sizeof(struct _Array));
                DropPool = DictionaryLookup(World->DropTable.MobDropPool, &DropPoolIndex);
                ArrayInitializeEmpty(DropPool, Runtime->Allocator, sizeof(struct _RTDropItem), 8);
            }

            DropItem = (RTDropItemRef)ArrayAppendUninitializedElement(DropPool);
            memset(DropItem, 0, sizeof(struct _RTDropItem));
        }
        else {
            DropItem = (RTDropItemRef)ArrayAppendUninitializedElement(World->DropTable.WorldDropPool);
            memset(DropItem, 0, sizeof(struct _RTDropItem));

            if (!ParseAttributeInt32(Archive, Iterator->Index, "MinLv", &DropItem->MinMobLevel)) goto error;
            if (!ParseAttributeInt32(Archive, Iterator->Index, "MaxLv", &DropItem->MaxMobLevel)) goto error;
        }

        if (!ParseAttributeUInt64(Archive, Iterator->Index, "ItemKind", &DropItem->ItemID.Serial)) goto error;
        if (!ParseAttributeUInt64(Archive, Iterator->Index, "ItemOpt", &DropItem->ItemOptions)) goto error;

        Float64 ScalarDropRate = 0.0;
        if (!ParseAttributeFloat64(Archive, Iterator->Index, "DropRate", &ScalarDropRate)) goto error;
        DropItem->DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "OptPoolIdx", &DropItem->OptionPoolIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "DurationIdx", &DropItem->DurationIndex)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveDestroy(Archive);
    return true;

error:
    ArchiveDestroy(Archive);
    return false;
}
