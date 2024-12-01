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
    Int32 SellPrice1;
    UInt8 Unknown1[16];
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
    Int32 MinMythLevel;
};

struct _ArchiveMobData {
    Char FileName[60];
    Char TextName[34];
    Int32 Scale;
    Float32 Radius;
    Float32 MovementSpeed;
    Float32 MovementAnimationSpeed;
    Float32 ChaseSpeed;
    Float32 ChaseAnimationSpeed;
    Float32 unk6;
    Int32 ShowDeath;
    Int32 ShowSpawn;
    Int32 AlphaDeath;
    Int32 AlphaSpawn;
    Float32 DefaultSkillTime;
    Int32 Group1;
    Int32 Group2;
    Int32 Property;
    Float32 SpecialSkillTime;
    Int16 Level;
    Int32 Bex;
    Int16 ServerBossType;
    Int16 Color;
    Int32 ServerBoss;
};

struct _ArchiveMobServerData {
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
    Int32 Exp;
    Int32 Attack;
    Int32 DefaultSkillStance;
    Int32 SpecialSkillStance;
    Float32 MovementSpeed;
    Float32 ChaseSpeed;
    Int32 Scale;
    Int32 DefaultSkillGroup;
    Int32 SpecialSkillGroup;
    UInt32 Property;
    Int32 CanAttack;
    Int32 D[3];
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
    Int32 ResistDiffDamage;
    Int32 ProportionalHPDmg;
    Int32 ServerBossType;
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
    Int32 NodeIndex,
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
    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.char_init");
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
    Int32 ParentIndex = ArchiveNodeGetChildByPath(RankArchive, -1, "cabal.rankup");
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

    for (Int BattleStyleIndex = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN; BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX; BattleStyleIndex += 1) {
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
    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest.cabal_quest");
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
    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "World.warp_npc");
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
    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Messages, -1, "cabal_message.item_msg.msg");
    while (Iterator) {
        Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Messages, Iterator->Index, "id");
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

    for (Int FileIndex = 0; FileIndex < 4; FileIndex += 1) {
        CString FilePath = PathCombineAll(RuntimeDirectory, FileNames[FileIndex], NULL);
        UInt8* Buffer = NULL;
        Int32 BufferLength = 0;

        if (!EncryptionDecryptFile(FilePath, &Buffer, &BufferLength)) {
            Fatal("Error loading file: %s", FilePath);
            goto error;
        }

        Int32 ItemCount = BufferLength / sizeof(struct _ArchiveItemData);
        for (Int Index = 0; Index < ItemCount; Index += 1) {
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
            ItemData->UniqueBattleStyleIndex = ArchiveItemData->LimitClass;
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
    FileRef MobFile = NULL;
    UInt8* MobMemory = NULL;
    Int32 MobMemoryLength = 0;
    FileRef MobServerFile = NULL;
    UInt8* MobServerMemory = NULL;
    Int32 MobServerMemoryLength = 0;

    MobFile = FileOpen(PathCombineAll(RuntimeDirectory, "mob.dat", NULL));
    if (!MobFile) goto error;

    MobServerFile = FileOpen(PathCombineAll(RuntimeDirectory, "mobserver.dat", NULL));
    if (!MobServerFile) goto error;

    FileRead(MobFile, &MobMemory, &MobMemoryLength);
    FileClose(MobFile);
    MobFile = NULL;

    FileRead(MobServerFile, &MobServerMemory, &MobServerMemoryLength);
    FileClose(MobServerFile);
    MobServerFile = NULL;

    struct _ArchiveMobData* ArchiveMob = (struct _ArchiveMobData*)MobMemory;
    struct _ArchiveMobServerData* ArchiveMobServer = (struct _ArchiveMobServerData*)MobServerMemory;

    Int MobCount = MobMemoryLength / sizeof(struct _ArchiveMobData);
    Int MobServerCount = MobServerMemoryLength / sizeof(struct _ArchiveMobServerData);
    if (MobCount != MobServerCount) goto error;

    for (Int Index = 0; Index < MobCount; Index += 1) {
        struct _ArchiveMobData* ArchiveMobData = &ArchiveMob[Index];
        struct _ArchiveMobServerData* ArchiveMobServerData = &ArchiveMobServer[Index];

        assert(Index < RUNTIME_MEMORY_MAX_MOB_DATA_COUNT);
        RTMobSpeciesDataRef MobData = &Runtime->MobData[Index];
        Runtime->MobDataCount = MAX(Runtime->MobDataCount, Index + 1);
        MobData->MobSpeciesIndex = Index;
        MobData->Level = ArchiveMobServerData->Level;
        MobData->MoveSpeed = ArchiveMobServerData->MovementSpeed;
        MobData->ChaseSpeed = ArchiveMobServerData->ChaseSpeed;
        MobData->Radius = ArchiveMobData->Radius;
        MobData->Property = ArchiveMobServerData->Property;
        MobData->AttackPattern = ArchiveMobServerData->AttackPattern;
        MobData->Cooperate = ArchiveMobServerData->Cooperative;
        MobData->Escape = 0; // TODO: Property is missing
        MobData->AttackType = ArchiveMobServerData->Attack;
        MobData->Scale = ArchiveMobServerData->Scale;
        MobData->FindCount = ArchiveMobServerData->FindCount;
        MobData->FindInterval = ArchiveMobServerData->FindInterval * 1000;
        MobData->MoveInterval = ArchiveMobServerData->MoveInterval;
        MobData->AlertRange = ArchiveMobServerData->AlertRange;
        MobData->ChaseRange = ArchiveMobServerData->ChaseRange;
        MobData->LimitRangeB = ArchiveMobServerData->LimitRangeB;
        MobData->ChaseInterval = 100;
        MobData->HP = ArchiveMobServerData->HP;
        MobData->Defense = ArchiveMobServerData->Defense;
        MobData->AttackRate = ArchiveMobServerData->AttackRate;
        MobData->DefenseRate = ArchiveMobServerData->DefenseRate;
        MobData->HPRecharge = ArchiveMobServerData->HPRecharge;
        MobData->DamageReduction = ArchiveMobServerData->DamageDiscount;
        MobData->Accuracy = ArchiveMobServerData->Accuracy;
        MobData->Penetration = ArchiveMobServerData->Penetration;
        MobData->ResistCriticalRate = ArchiveMobServerData->ResistCriticalRate;
        MobData->IgnoreAccuracy = ArchiveMobServerData->IgnoreAccuracy;
        MobData->IgnoreDamageReduction = ArchiveMobServerData->IgnoreDamageReduction;
        MobData->IgnorePenetration = ArchiveMobServerData->IgnorePenetration;
        MobData->AbsoluteDamage = ArchiveMobServerData->AbsoluteDamage;
        MobData->ResistSkillAmp = ArchiveMobServerData->ResistSkillAmp;
        MobData->ResistCriticalDamage = ArchiveMobServerData->ResistCriticalDamage;
        MobData->ResistSuppression = ArchiveMobServerData->ResistSuppression;
        MobData->ResistSilence = ArchiveMobServerData->ResistSilence;
        MobData->ReflectDamage = ArchiveMobServerData->ProportionalHPDmg;
        MobData->Flee = ArchiveMobServerData->MoveInterval;
        MobData->MoveInterval = 500;
        MobData->Accuracy = ArchiveMobServerData->Accuracy;
        MobData->DefaultSkill.PhysicalAttackMin = ArchiveMobServerData->DefaultSkillPhysicalAttackMin;
        MobData->DefaultSkill.PhysicalAttackMax = ArchiveMobServerData->DefaultSkillPhysicalAttackMax;
        MobData->DefaultSkill.Reach = ArchiveMobServerData->DefaultSkillReach;
        MobData->DefaultSkill.Range = ArchiveMobServerData->DefaultSkillRange;
        MobData->DefaultSkill.Stance = ArchiveMobServerData->DefaultSkillStance;
        MobData->DefaultSkill.SkillGroup = ArchiveMobServerData->DefaultSkillGroup;
        MobData->DefaultSkill.Interval = ceilf(ArchiveMobServerData->DefaultSkillInterval * 10) * 100;
        MobData->SpecialSkill.PhysicalAttackMin = ArchiveMobServerData->SpecialSkillPhysicalAttackMin;
        MobData->SpecialSkill.PhysicalAttackMax = ArchiveMobServerData->SpecialSkillPhysicalAttackMax;
        MobData->SpecialSkill.Reach = ArchiveMobServerData->SpecialSkillReach;
        MobData->SpecialSkill.Range = ArchiveMobServerData->SpecialSkillRange;
        MobData->SpecialSkill.Stance = ArchiveMobServerData->SpecialSkillStance;
        MobData->SpecialSkill.SkillGroup = ArchiveMobServerData->SpecialSkillGroup;
        MobData->SpecialSkill.Interval = ceilf(ArchiveMobServerData->SpecialSkillInterval * 10) * 100;
        MobData->AttackSignal = ArchiveMobServerData->AttackSignalValue;
        MobData->BossType = ArchiveMobServerData->ServerBossType;
        MobData->Exp = ArchiveMobServerData->Exp;
        MobData->AttackCountAmp = ArchiveMobServerData->AttackCountAmp;
        MobData->AggroPattern = ArchiveMobServerData->AggroPattern;
        MobData->ResistDiffDamage = ArchiveMobServerData->ResistDiffDamage;

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
        
        MobData->CanAttack = ArchiveMobServerData->CanAttack == 0; // TODO: Resolve correct mapping
        MobData->CanMove = (MobData->FindCount >= RUNTIME_MOB_FIND_COUNT_UNMOVABLE) ? 0 : 1;

        if (MobData->AttackType == RUNTIME_MOB_ATTACK_TYPE_NONE) {
            MobData->CanAttack = 0;
        }
        
        MobData->AggressiveType = RUNTIME_MOB_AGGRESSIVE_TYPE_PASSIVE;
        if (ArchiveMobServerData->Aggressive) {
            MobData->AggressiveType = RUNTIME_MOB_AGGRESSIVE_TYPE_AGGRESSIVE;
        }

        if (ArchiveMobServerData->AggroPattern == RUNTIME_MOB_AGGRESSIVE_TYPE_NATIONONLY) {
            MobData->AggressiveType = RUNTIME_MOB_AGGRESSIVE_TYPE_NATIONONLY;
        }
    }

    Int32 SkillGroupUsage[50] = { 0 };
    Int32 AttackPatternUsage[50] = { 0 };

    for (Int Index = 0; Index < MobCount; Index += 1) {
        struct _ArchiveMobServerData* ArchiveMobData = &ArchiveMobServer[Index];

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

    for (Int Index = 0; Index < 50; Index += 1) {
        if (SkillGroupUsage[Index] > 0)
            Warn("Mob Skill Group: %d", Index);
    }

    for (Int Index = 0; Index < 50; Index += 1) {
        if (AttackPatternUsage[Index] > 0)
            Warn("Mob Attack Pattern: %d", Index);
    }

    free(MobMemory);
    free(MobServerMemory);
    return true;

error:
    if (MobFile) FileClose(MobFile);
    if (MobServerFile) FileClose(MobServerFile);
    if (MobMemory) free(MobMemory);
    if (MobServerMemory) free(MobServerMemory);

    return false;
}

Bool ServerLoadWarpData(
    RTRuntimeRef Runtime,
    ArchiveRef Archive
);

Bool ServerLoadWorldMobData(
    RTRuntimeRef Runtime,
    CString RuntimeDirectory,
    CString ServerDirectory,
    CString ScriptDirectory,
    ArchiveRef Archive,
    Int32 ArchiveParentIndex,
    RTWorldDataRef WorldData
) {
    ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ArchiveParentIndex, "MobPool.Mob");
    while (ChildIterator) {
        RTMobRef Mob = (RTMobRef)ArrayAppendUninitializedElement(WorldData->MobTable);
        memset(Mob, 0, sizeof(struct _RTMob));

        if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "MobIndex", &Mob->ID.EntityIndex)) goto error;
        Mob->ID.WorldIndex = WorldData->WorldIndex;
        Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpeciesIndex", &Mob->Spawn.MobSpeciesIndex)) goto error;
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "X", &Mob->Spawn.AreaX)) goto error;
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Y", &Mob->Spawn.AreaY)) goto error;
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Width", &Mob->Spawn.AreaWidth)) goto error;
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Height", &Mob->Spawn.AreaHeight)) goto error;
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpawnInterval", &Mob->Spawn.SpawnInterval)) goto error;
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpawnDefault", &Mob->Spawn.SpawnDefault)) goto error;
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MissionGate", &Mob->Spawn.IsMissionGate)) goto error;
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "PerfectDrop", &Mob->Spawn.PerfectDrop)) goto error;
        ParseAttributeInt32(Archive, ChildIterator->Index, "MobPatrolIndex", &Mob->Spawn.MobPatrolIndex);
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MobPatternIndex", &Mob->Spawn.MobPatternIndex)) goto error;
        
        Char MobScriptFileName[MAX_PATH] = { 0 };
        if (ParseAttributeString(Archive, ChildIterator->Index, "Script", MobScriptFileName, MAX_PATH)) {
            if (strlen(MobScriptFileName) > 0) {
                CString MobScriptFilePath = PathCombineAll(ScriptDirectory, MobScriptFileName, NULL);
                Mob->Script = RTScriptManagerLoadScript(Runtime->ScriptManager, MobScriptFilePath);
            }
        }

        Mob->SpeciesData = &Runtime->MobData[Mob->Spawn.MobSpeciesIndex];
        Mob->IsInfiniteSpawn = true;
        Mob->IsPermanentDeath = false;
        Mob->RemainingSpawnCount = 0;

        ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadCommonDropData(
    RTRuntimeRef Runtime,
    ArchiveRef Archive,
    Int32 ArchiveParentIndex,
    ArrayRef DropPool,
    CString DropPoolName
) {
    ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ArchiveParentIndex, DropPoolName);
    while (ChildIterator) {
        Int32 MinMobLevel = 0;
        Int32 MaxMobLevel = 0;

        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MinMobLevel", &MinMobLevel)) goto error;
        if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MaxMobLevel", &MaxMobLevel)) goto error;

        ArchiveIteratorRef ItemIterator = ArchiveQueryNodeIteratorFirst(Archive, ChildIterator->Index, "Item");
        while (ItemIterator) {
            RTDropItemRef DropItem = (RTDropItemRef)ArrayAppendUninitializedElement(DropPool);
            memset(DropItem, 0, sizeof(struct _RTDropItem));

            if (!ParseAttributeUInt64(Archive, ItemIterator->Index, "ItemID", &DropItem->ItemID.Serial)) goto error;
            if (!ParseAttributeUInt64(Archive, ItemIterator->Index, "ItemOption", &DropItem->ItemOptions)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "ItemDuration", &DropItem->DurationIndex)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "OptionPoolID", &DropItem->OptionPoolIndex)) goto error;

            Float64 ScalarDropRate = 0.0;
            if (!ParseAttributeFloat64(Archive, ItemIterator->Index, "Rate", &ScalarDropRate)) goto error;
            DropItem->DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);
            DropItem->MinMobLevel = MinMobLevel;
            DropItem->MaxMobLevel = MaxMobLevel;

            ItemIterator = ArchiveQueryNodeIteratorNext(Archive, ItemIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadSpeciesDropData(
    RTRuntimeRef Runtime,
    ArchiveRef Archive,
    Int32 ArchiveParentIndex,
    DictionaryRef SpeciesDropPool,
    CString DropPoolName,
    Bool HasQuestItemOptions
) {
    ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ArchiveParentIndex, DropPoolName);
    while (ChildIterator) {
        Int DropPoolIndex = 0;
        if (!ParseAttributeInt(Archive, ChildIterator->Index, "MobSpeciesIndex", &DropPoolIndex)) goto error;

        ArrayRef DropPool = (ArrayRef)DictionaryLookup(SpeciesDropPool, &DropPoolIndex);
        if (!DropPool) {
            struct _Array TempArray = { 0 };
            DictionaryInsert(SpeciesDropPool, &DropPoolIndex, &TempArray, sizeof(struct _Array));
            DropPool = DictionaryLookup(SpeciesDropPool, &DropPoolIndex);
            ArrayInitializeEmpty(DropPool, Runtime->Allocator, sizeof(struct _RTDropItem), 8);
        }

        ArchiveIteratorRef ItemIterator = ArchiveQueryNodeIteratorFirst(Archive, ChildIterator->Index, "Item");
        while (ItemIterator) {
            RTDropItemRef DropItem = (RTDropItemRef)ArrayAppendUninitializedElement(DropPool);
            memset(DropItem, 0, sizeof(struct _RTDropItem));

            if (!ParseAttributeUInt64(Archive, ItemIterator->Index, "ItemID", &DropItem->ItemID.Serial)) goto error;
            if (!ParseAttributeUInt64(Archive, ItemIterator->Index, "ItemOption", &DropItem->ItemOptions)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "ItemDuration", &DropItem->DurationIndex)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "OptionPoolID", &DropItem->OptionPoolIndex)) goto error;

            if (HasQuestItemOptions) {
                DropItem->ItemID.IsCharacterBinding = true;
                DropItem->ItemOptions = RTQuestItemOptions(DropItem->ItemOptions, 1);
            }

            Float64 ScalarDropRate = 0.0;
            if (!ParseAttributeFloat64(Archive, ItemIterator->Index, "Rate", &ScalarDropRate)) goto error;
            DropItem->DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);
            DropItem->MinMobLevel = 0;
            DropItem->MaxMobLevel = 1000;

            ItemIterator = ArchiveQueryNodeIteratorNext(Archive, ItemIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadTerrainData(
    RTRuntimeRef Runtime,
    CString RuntimeDirectory,
    CString ServerDirectory,
    CString ScriptDirectory,
    ArchiveRef TerrainArchive
) {

    Int32 ParentIndex = ArchiveNodeGetChildByPath(TerrainArchive, -1, "Terrain.map");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(TerrainArchive, ParentIndex, "map_index");
    while (Iterator) {
        Int32 WorldIndex = 0;
        if (!ParseAttributeInt32(TerrainArchive, Iterator->Index, "world_id", &WorldIndex)) goto error;

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

    return true;

error:
    return false;
}

Bool ServerLoadWorldData(
    RTRuntimeRef Runtime,
    CString RuntimeDirectory,
    CString ServerDirectory,
    CString ScriptDirectory,
    ArchiveRef MainArchive,
    ArchiveRef TempArchive
) {
    Int32 HasMapCode = 0;
    Int32 GpsOrder = 0;
    Int32 WarAllowed = 0;
    Int32 WarControl = 0;

    for (Int WorldIndex = 0; WorldIndex < Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!RTWorldDataExists(Runtime->WorldManager, WorldIndex)) continue;
        
        RTWorldDataRef World = RTWorldDataGet(Runtime->WorldManager, WorldIndex);
        CString WorldFilePath = CStringFormat(
            PathCombineAll(RuntimeDirectory, "World", "world%zu.enc", NULL),
            World->WorldIndex
        );

        ArchiveClear(TempArchive, true);

        if (!ArchiveLoadFromFileEncryptedNoAlloc(TempArchive, WorldFilePath, false)) {
            Error("Loading world file %s failed!", WorldFilePath);
            continue;
        }

        if (!ServerLoadWarpData(Runtime, TempArchive)) goto error;

        Int32 NodeIndex = ArchiveQueryNodeWithAttribute(
            TempArchive,
            -1,
            "World.cabal_world.world",
            "id",
            UInt64ToStringNoAlloc(World->WorldIndex)
        );

        if (NodeIndex < 0) goto error;

        Int32 WorldIndex = 0;
        if (!ParseAttributeInt32(TempArchive, NodeIndex, "id", &WorldIndex)) goto error;
        assert(WorldIndex == World->WorldIndex);

        if (!ParseAttributeInt32(TempArchive, NodeIndex, "type", &World->Type)) goto error;
        if (!ParseAttributeInt32(TempArchive, NodeIndex, "m_code", &HasMapCode)) goto error;
        if (!ParseAttributeInt32(TempArchive, NodeIndex, "gpsorder", &World->MapCodeIndex)) goto error;
        if (!ParseAttributeInt32(TempArchive, NodeIndex, "allowedwar", &WarAllowed)) goto error;
        if (!ParseAttributeInt32(TempArchive, NodeIndex, "warcontrol", &WarControl)) goto error;

        if (HasMapCode > 0) World->Flags |= RUNTIME_WORLD_FLAGS_HAS_MAPCODE;
        if (WarAllowed > 0) World->Flags |= RUNTIME_WORLD_FLAGS_WAR_ALLOWED;
        if (WarControl > 0) World->Flags |= RUNTIME_WORLD_FLAGS_WAR_CONTROL;

        Char MapFileName[MAX_PATH] = { 0 };
        if (!ParseAttributeString(TempArchive, NodeIndex, "map_file", MapFileName, MAX_PATH)) goto error;
        if (strlen(MapFileName) > 0) {
            CString MapFilePath = PathCombineAll(RuntimeDirectory, "Map", MapFileName, NULL);
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

            for (Int Index = 0; Index < EffectCount; Index++) {
                UInt16 TextLength = *((UInt16*)&MapData[MapDataOffset]);
                MapDataOffset += 18;
                MapDataOffset += TextLength;
            }

            Int32 TextureCount = *((Int32*)&MapData[MapDataOffset]);
            MapDataOffset += 4;

            for (Int Index = 0; Index < TextureCount; Index++) {
                UInt32 Size = *((UInt32*)&MapData[MapDataOffset]);
                MapDataOffset += 4;
                MapDataOffset += Size;
            }

            MapDataOffset += 12;
            MapDataOffset += (Int)(RUNTIME_WORLD_SIZE + 1) * (RUNTIME_WORLD_SIZE + 1) * sizeof(UInt32);
            UInt32* TileData = (UInt32*)&MapData[MapDataOffset];
            memcpy(World->Tiles, TileData, sizeof(UInt32) * RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE);
            free(MapData);
        }

        ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(TempArchive, NodeIndex, "world_npc");
        while (ChildIterator) {
            assert(Runtime->NpcCount < RUNTIME_MEMORY_MAX_NPC_COUNT);

            RTNpcRef Npc = &Runtime->Npcs[Runtime->NpcCount];

            if (!ParseAttributeInt32(TempArchive, ChildIterator->Index, "id", &Npc->ID)) goto error;
            if (!ParseAttributeInt32(TempArchive, ChildIterator->Index, "x", &Npc->X)) goto error;
            if (!ParseAttributeInt32(TempArchive, ChildIterator->Index, "y", &Npc->Y)) goto error;

            Npc->WorldID = World->WorldIndex;

            Runtime->NpcCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(TempArchive, ChildIterator);
        }

        // Load Trainer Data
        // TODO: Move trainer data to server data
        {
            ArchiveIteratorRef SkillIterator = ArchiveQueryNodeIteratorFirst(TempArchive, NodeIndex, "skill");
            if (SkillIterator) {
                assert(Runtime->TrainerDataCount < RUNTIME_MEMORY_MAX_TRAINER_DATA_COUNT);

                RTTrainerDataRef TrainerData = &Runtime->TrainerData[Runtime->TrainerDataCount];
                TrainerData->Index = Runtime->TrainerDataCount;
                TrainerData->WorldID = World->WorldIndex;
                TrainerData->NpcID = 2;

                while (SkillIterator) {
                    assert(TrainerData->SkillCount < RUNTIME_TRAINER_MAX_SKILL_COUNT);

                    RTTrainerSkillDataRef SkillData = &TrainerData->Skills[TrainerData->SkillCount];

                    if (!ParseAttributeInt32(TempArchive, SkillIterator->Index, "id", &SkillData->ID)) goto error;
                    if (!ParseAttributeInt32(TempArchive, SkillIterator->Index, "slot_id", &SkillData->SlotID)) goto error;
                    if (!ParseAttributeInt32(TempArchive, SkillIterator->Index, "level", &SkillData->Level)) goto error;
                    if (!ParseAttributeInt32(TempArchive, SkillIterator->Index, "skill_book", &SkillData->SkillBookID)) goto error;

                    TrainerData->SkillCount += 1;
                    SkillIterator = ArchiveQueryNodeIteratorNext(TempArchive, SkillIterator);
                }

                Runtime->TrainerDataCount += 1;
            }
        }

        WorldFilePath = CStringFormat(
            PathCombineAll(ServerDirectory, "World", "World_%zu.xml", NULL),
            World->WorldIndex
        );

        if (FileExists(WorldFilePath)) {
            Info("Loading world file: %s", WorldFilePath);

            if (!ArchiveLoadFromFile(TempArchive, WorldFilePath, false)) goto error;

            Int32 ParentIndex = ArchiveNodeGetChildByPath(TempArchive, -1, "World");
            if (ParentIndex < 0) goto error;

            if (!ServerLoadWorldMobData(Runtime, RuntimeDirectory, ServerDirectory, ScriptDirectory, TempArchive, ParentIndex, World)) goto error;
            if (!ServerLoadCommonDropData(Runtime, TempArchive, ParentIndex, World->DropTable.WorldDropPool, "CommonDropPool")) goto error;
            if (!ServerLoadSpeciesDropData(Runtime, TempArchive, ParentIndex, World->DropTable.MobDropPool, "MobDropPool", false)) goto error;
            if (!ServerLoadSpeciesDropData(Runtime, TempArchive, ParentIndex, World->DropTable.QuestDropPool, "QuestDropPool", true)) goto error;

            ArchiveClear(TempArchive, true);
        }
    }

    for (Int WorldIndex = 0; WorldIndex < Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!RTWorldDataExists(Runtime->WorldManager, WorldIndex)) continue;

        RTWorldDataRef World = RTWorldDataGet(Runtime->WorldManager, WorldIndex);

        Int32 NodeIndex = ArchiveQueryNodeWithAttribute(
            MainArchive,
            -1,
            "cabal.cabal_world.world",
            "id",
            UInt64ToStringNoAlloc(World->WorldIndex)
        );

        if (NodeIndex < 0) continue;

        Int32 WorldIndex = 0;
        if (!ParseAttributeInt32(MainArchive, NodeIndex, "id", &WorldIndex)) continue;
        assert(WorldIndex == World->WorldIndex);
    }

    return true;

error:
    for (Int WorldIndex = 0; WorldIndex < Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!RTWorldDataExists(Runtime->WorldManager, WorldIndex)) continue;

        RTWorldDataRef World = RTWorldDataGet(Runtime->WorldManager, WorldIndex);
        ArrayDestroy(World->MobTable);
    }

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
    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal_skill.new_skill_list");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "skill_main");
    while (Iterator) {
        Int32 SkillIndex = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &SkillIndex)) goto error;

        RTCharacterSkillDataRef SkillData = MemoryPoolReserve(Runtime->SkillDataPool, SkillIndex);
        SkillData->SkillID = SkillIndex;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "type", &SkillData->SkillType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "multi", &SkillData->Multi)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "group", &SkillData->SkillGroup)) goto error;
        ParseAttributeInt32Array(Archive, Iterator->Index, "dur", SkillData->Duration, 2, ',');
        if (!ParseAttributeInt32(Archive, Iterator->Index, "intensity", &SkillData->Intensity)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "element", &SkillData->Element)) goto error;
        
        ArchiveIteratorRef AttributeIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "attribute");
        if (AttributeIterator) {
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "target", &SkillData->Target)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "max_target", &SkillData->MaxTarget)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "reach", &SkillData->MaxReach)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "mreach", &SkillData->MinReach)) goto error;
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
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "term", &SkillData->CastFrame)) goto error;
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
                if (!ParseAttributeInt32(Archive, PowerIterator->Index, "damagetype", &SkillData->DamageType)) goto error;
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
    CString FilePath = CStringFormat(
        PathCombineAll(ServerDirectory, "World", "Dungeon", "Dungeon_%d.xml", NULL),
        DungeonData->DungeonIndex
    );

    if (ArchiveLoadFromFile(Archive, FilePath, false)) {
        Info("Loading dungeon data: %s", FilePath);
        Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "Dungeon");
        if (ParentIndex < 0) goto error;

        DungeonData->StartKillMobCount = ParseAttributeInt32ArrayCounted(
            Archive,
            ParentIndex,
            "StartKillMobList",
            DungeonData->StartKillMobList,
            RUNTIME_DUNGEON_MAX_START_KILL_MOB_COUNT,
            ','
        );

        ArchiveIteratorRef GroupIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "TriggerGroup");
        while (GroupIterator) {
            Int TriggerGroupIndex = 0;
            if (!ParseAttributeInt(Archive, GroupIterator->Index, "Index", &TriggerGroupIndex)) {
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
                    RUNTIME_DUNGEON_MAX_TRIGGER_MOB_COUNT,
                    ','
                );

                TriggerData->DeadMobCount = ParseAttributeInt32ArrayCounted(
                    Archive,
                    Iterator->Index,
                    "DeadMobIndexList",
                    TriggerData->DeadMobIndexList,
                    RUNTIME_DUNGEON_MAX_TRIGGER_MOB_COUNT,
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
            Int ActionGroupIndex = 0;
            if (!ParseAttributeInt(Archive, GroupIterator->Index, "Index", &ActionGroupIndex)) {
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
            Int MobIndex = 0;
            if (!ParseAttributeInt(Archive, GroupIterator->Index, "MobIndex", &MobIndex)) {
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

        GroupIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "TimerControl");
        while (GroupIterator) {
            DungeonData->TimerData.Active = true;

            if (!ParseAttributeUInt32(Archive, GroupIterator->Index, "ItemID", &DungeonData->TimerData.ItemID)) goto error;

            DungeonData->TimerData.MobIndexCount = ParseAttributeInt32ArrayCounted(
                Archive,
                GroupIterator->Index,
                "MobIndexList",
                DungeonData->TimerData.MobIndexList,
                RUNTIME_DUNGEON_MAX_TIMER_MOB_COUNT,
                ','
            );

            ArchiveIteratorRef TimerIterator = ArchiveQueryNodeIteratorFirst(Archive, GroupIterator->Index, "Timer");
            while (TimerIterator) {
                assert(DungeonData->TimerData.TimerCount < RUNTIME_DUNGEON_MAX_TIMER_COUNT);

                RTTimerDataRef Timer = &DungeonData->TimerData.Timers[DungeonData->TimerData.TimerCount];
                DungeonData->TimerData.TimerCount += 1;

                if (!ParseAttributeInt32(Archive, TimerIterator->Index, "Type", &Timer->Type)) goto error;
                if (!ParseAttributeInt32(Archive, TimerIterator->Index, "Index", &Timer->TargetMobIndex)) goto error;
                if (!ParseAttributeInt32(Archive, TimerIterator->Index, "Event", &Timer->TargetEvent)) goto error;
                if (!ParseAttributeInt32(Archive, TimerIterator->Index, "Value", &Timer->Interval)) goto error;

                TimerIterator = ArchiveQueryNodeIteratorNext(Archive, TimerIterator);
            }

            GroupIterator = ArchiveQueryNodeIteratorNext(Archive, GroupIterator);
        }

        if (!ServerLoadCommonDropData(Context->Runtime, Archive, ParentIndex, DungeonData->DropTable.WorldDropPool, "CommonDropPool")) goto error;
        if (!ServerLoadSpeciesDropData(Context->Runtime, Archive, ParentIndex, DungeonData->DropTable.MobDropPool, "MobDropPool", false)) goto error;
        if (!ServerLoadSpeciesDropData(Context->Runtime, Archive, ParentIndex, DungeonData->DropTable.QuestDropPool, "QuestDropPool", true)) goto error;
    }

    ArchiveDestroy(Archive);
    return true;

error:
    ArchiveDestroy(Archive);
    return false;
}

Bool ServerLoadDungeonMissionData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    RTDungeonDataRef DungeonData
) {
    RTRuntimeRef Runtime = Context->Runtime;
    ArchiveRef Archive = ArchiveCreateEmpty(Runtime->Allocator);
    CString WorldFilePath = CStringFormat(
        PathCombineAll(RuntimeDirectory, "World", "world%d.enc", NULL),
        DungeonData->WorldIndex
    );

    ArchiveClear(Archive, true);

    if (!ArchiveLoadFromFileEncryptedNoAlloc(Archive, WorldFilePath, false)) {
        Error("Loading world file %s failed!", WorldFilePath);
        goto error;
    }

    Int32 NodeIndex = ArchiveQueryNodeWithAttribute(
        Archive,
        -1,
        "World.cabal_world.world",
        "id",
        UInt64ToStringNoAlloc(DungeonData->WorldIndex)
    );
    if (NodeIndex < 0) goto error;

    Int32 MissionNodeIndex = ArchiveQueryNodeWithAttribute(
        Archive,
        NodeIndex,
        "mission",
        "dungeon_id",
        UInt64ToStringNoAlloc(DungeonData->DungeonIndex)
    );
    if (MissionNodeIndex < 0) {
        ArchiveDestroy(Archive);
        return true;
    }

    ArchiveIteratorRef NodeIterator = ArchiveQueryNodeIteratorFirst(Archive, MissionNodeIndex, "mission_immune");
    while (NodeIterator) {
        Int MobIndex = 0;
        if (!ParseAttributeInt(Archive, NodeIterator->Index, "id", &MobIndex))
            goto error;

        RTDungeonImmuneControlDataRef ImmuneControlData = DictionaryLookup(DungeonData->ImmuneControls, &MobIndex);
        if (!ImmuneControlData) {
            struct _RTDungeonImmuneControlData NewImmuneControlData = { 0 };
            DictionaryInsert(DungeonData->ImmuneControls, &MobIndex, &NewImmuneControlData, sizeof(struct _RTDungeonImmuneControlData));
            ImmuneControlData = DictionaryLookup(DungeonData->ImmuneControls, &MobIndex);
        }
        assert(ImmuneControlData);

        assert(ImmuneControlData->ImmuneCount < RUNTIME_DUNGEON_MAX_IMMUNE_CONTROL_COUNT);
        RTImmuneDataRef ImmuneData = &ImmuneControlData->ImmuneList[ImmuneControlData->ImmuneCount];
        ImmuneControlData->ImmuneCount += 1;

        if (!ParseAttributeInt32(Archive, NodeIterator->Index, "target", &ImmuneData->TargetIndex))
            goto error;
        if (!ParseAttributeInt32(Archive, NodeIterator->Index, "active_type", &ImmuneData->ActivationType))
            goto error;
        if (!ParseAttributeInt32(Archive, NodeIterator->Index, "attack", &ImmuneData->CanAttack))
            goto error;
        if (!ParseAttributeInt32(Archive, NodeIterator->Index, "select", &ImmuneData->CanSelect))
            goto error;

        NodeIterator = ArchiveQueryNodeIteratorNext(Archive, NodeIterator);
    }

    NodeIterator = ArchiveQueryNodeIteratorFirst(Archive, MissionNodeIndex, "gate");
    while (NodeIterator) {
        Int MobIndex = 0;
        if (!ParseAttributeInt(Archive, NodeIterator->Index, "id", &MobIndex))
            goto error;

        Bool MobFound = false;
        for (Int PatternPartIndex = 0; PatternPartIndex < DungeonData->PatternPartCount; PatternPartIndex += 1) {
            RTMissionDungeonPatternPartDataRef PatternPartData = RTRuntimeGetPatternPartByID(
                Runtime,
                DungeonData->PatternPartIndices[PatternPartIndex]
            );

            for (Int Index = 0; Index < ArrayGetElementCount(PatternPartData->MobTable); Index += 1) {
                RTMobRef Mob = (RTMobRef)ArrayGetElementAtIndex(PatternPartData->MobTable, Index);
                if (Mob->ID.EntityIndex != MobIndex) continue;

                Mob->Spawn.IsMissionGate = true;
                MobFound = true;
                break;
            }

            if (MobFound) break;
        }

        RTDungeonGateControlDataRef GateControlData = DictionaryLookup(DungeonData->GateControls, &MobIndex);
        if (!GateControlData) {
            struct _RTDungeonGateControlData NewGateControlData = { 0 };
            DictionaryInsert(DungeonData->GateControls, &MobIndex, &NewGateControlData, sizeof(struct _RTDungeonGateControlData));
            GateControlData = DictionaryLookup(DungeonData->GateControls, &MobIndex);
        }
        assert(GateControlData);

        if (!ParseAttributeInt32(Archive, NodeIterator->Index, "r", &GateControlData->Rotation)) goto error;
        if (!ParseAttributeInt32Array(Archive, NodeIterator->Index, "position", &GateControlData->Position.X, 2, ':')) goto error;
        if (!ParseAttributeInt32(Archive, NodeIterator->Index, "attack", &GateControlData->CanAttack)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIterator->Index, "select", &GateControlData->CanSelect)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIterator->Index, "use_debuff", &GateControlData->CanDebuff)) goto error;

        GateControlData->CellCount = ParseAttributeInt32Array2D(
            Archive,
            NodeIterator->Index,
            "cells",
            &GateControlData->CellList[0].X,
            2,
            RUNTIME_DUNGEON_MAX_GATE_CELL_COUNT,
            ':',
            ','
        );

        NodeIterator = ArchiveQueryNodeIteratorNext(Archive, NodeIterator);
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
        Info("Loading pattern part data: %s", FilePath);
        Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "PatternPart");
        if (ParentIndex < 0) 
            goto error;

        Int PatternPartIndex = 0;
        if (!ParseAttributeInt(Archive, ParentIndex, "Index", &PatternPartIndex)) 
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

            if (!ParseAttributeInt32(Archive, MobIterator->Index, "SpeciesIndex", &Mob->Spawn.MobSpeciesIndex)) {
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
            ParseAttributeInt32(Archive, MobIterator->Index, "ServerMobIndex", &Mob->Spawn.MobPatternIndex);
            ParseAttributeInt32(Archive, MobIterator->Index, "LootDelay", &Mob->Spawn.LootDelay);

            ParseAttributeInt32Array(Archive, MobIterator->Index, "EventProperty", Mob->Spawn.EventProperty, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
            ParseAttributeInt32Array(Archive, MobIterator->Index, "EventMobList", Mob->Spawn.EventMobs, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
            ParseAttributeInt32Array(Archive, MobIterator->Index, "EventInterval", Mob->Spawn.EventInterval, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
            ParseAttributeString(Archive, MobIterator->Index, "Script", Mob->Spawn.Script, MAX_PATH);

            Mob->EnemyCount = ParseAttributeInt32ArrayCounted(
                Archive, 
                MobIterator->Index, 
                "Enemies", 
                Mob->Enemies, 
                RUNTIME_MEMORY_MAX_MOB_ENEMY_COUNT,
                ','
            );

            Mob->SpeciesData = &Context->Runtime->MobData[Mob->Spawn.MobSpeciesIndex];
            Mob->IsInfiniteSpawn = Mob->Spawn.SpawnCount == 1 && Mob->Spawn.SpawnDefault;
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

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_quest_dungeon");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        Int DungeonIndex = 0;
        if (!ParseAttributeInt(Archive, Iterator->Index, "id", &DungeonIndex)) goto error;
        assert(!DictionaryLookup(Runtime->DungeonData, &DungeonIndex));

        struct _RTDungeonData DungeonDataMemory = { 0 };
        DictionaryInsert(Runtime->DungeonData, &DungeonIndex, &DungeonDataMemory, sizeof(struct _RTDungeonData));
        RTDungeonDataRef DungeonData = DictionaryLookup(Runtime->DungeonData, &DungeonIndex);
        assert(DungeonData);

        DungeonData->DungeonIndex = (Int32)DungeonIndex;

        Int32 EntryItem[2];
        if (ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_item", EntryItem, 2, ':')) {
            DungeonData->EntryItemID.ID = EntryItem[0];
            DungeonData->EntryItemOption = EntryItem[1];
        }

        ParseAttributeInt32(Archive, Iterator->Index, "create_type", &DungeonData->CreateType);
        ParseAttributeInt32(Archive, Iterator->Index, "enter_type", &DungeonData->EntryType);
        ParseAttributeInt32(Archive, Iterator->Index, "user_type", &DungeonData->PlayerType);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "user_count", &DungeonData->MaxPlayerCount)) {
            DungeonData->MaxPlayerCount = 1;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "ObjCount", &DungeonData->ObjectCount);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "RemoveItem", &DungeonData->RemoveItem)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_cd_level", &DungeonData->EntryConditionLevel)) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_class", DungeonData->EntryConditionClass, 2, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_warp_id", &DungeonData->EntryWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "fail_warp_npc_id", &DungeonData->FailWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dead_warp_id", &DungeonData->DeadWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "penalty_value", &DungeonData->PenaltyValue)) goto error;

        DungeonData->PatternPartCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "pp_id",
            DungeonData->PatternPartIndices,
            RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT,
            ','
        );

        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "reward", DungeonData->Reward, 8, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "success_warp_npc_id", &DungeonData->SuccessWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData->WorldIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData->NextDungeonIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData->DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData->IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData->EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData->MissionTimeout)) goto error;

        RTWorldDataRef WorldData = RTWorldDataGet(Runtime->WorldManager, DungeonData->WorldIndex);
        WorldData->HasQuestDungeon = true;

        DungeonData->TriggerGroups = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->ActionGroups = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->TimeControls = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->ImmuneControls = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->GateControls = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->DropTable.WorldDropPool = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTDropItem), 8);
        DungeonData->DropTable.MobDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->DropTable.QuestDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);

        if (!ServerLoadDungeonExtraData(Context, RuntimeDirectory, ServerDirectory, DungeonData)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    struct _RTMissionDungeonPatternPartData PatternPartData = { 0 };

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        Int PatternPartIndex = 0;
        if (!ParseAttributeInt(Archive, Iterator->Index, "id", &PatternPartIndex)) goto error;
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

        for (Int Index = 0; Index < PatternPartData.MissionMobCount; Index += 1) {
            PatternPartData.MissionMobs[Index].MobID = MissionMobs[Index].MobID;
            PatternPartData.MissionMobs[Index].Count = MissionMobs[Index].MaxCount;
            PatternPartData.MissionMobs[Index].MaxCount = MissionMobs[Index].MaxCount;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "mission_npc", &PatternPartData.MissionNpcIndex);
        DictionaryInsert(Runtime->PatternPartData, &PatternPartIndex, &PatternPartData, sizeof(struct _RTMissionDungeonPatternPartData));

        CString PatternPartFilePath = CStringFormat(
            PathCombineAll(ServerDirectory, "World", "Dungeon", "PatternPart", "PatternPart_%zu.xml", NULL),
            PatternPartIndex
        );

        if (!ServerLoadPatternPartData(Context, PatternPartFilePath)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_quest_dungeon");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        Int DungeonIndex = 0;
        if (!ParseAttributeInt(Archive, Iterator->Index, "id", &DungeonIndex)) goto error;
        assert(DictionaryLookup(Runtime->DungeonData, &DungeonIndex));

        RTDungeonDataRef DungeonData = DictionaryLookup(Runtime->DungeonData, &DungeonIndex);
        assert(DungeonData);

        if (!ServerLoadDungeonMissionData(Context, RuntimeDirectory, ServerDirectory, DungeonData)) goto error;

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

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        Int DungeonIndex = 0;
        if (!ParseAttributeInt(Archive, Iterator->Index, "id", &DungeonIndex)) goto error;
        assert(!DictionaryLookup(Runtime->DungeonData, &DungeonIndex));

        struct _RTDungeonData DungeonDataMemory = { 0 };
        DictionaryInsert(Runtime->DungeonData, &DungeonIndex, &DungeonDataMemory, sizeof(struct _RTDungeonData));
        RTDungeonDataRef DungeonData = DictionaryLookup(Runtime->DungeonData, &DungeonIndex);
        assert(DungeonData);

        DungeonData->DungeonIndex = (Int32)DungeonIndex;

        Int32 EntryItem[2];
        if (ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_item", EntryItem, 2, ':')) {
            DungeonData->EntryItemID.ID = EntryItem[0];
            DungeonData->EntryItemOption = EntryItem[1];
        }

        ParseAttributeInt32(Archive, Iterator->Index, "create_type", &DungeonData->CreateType);
        ParseAttributeInt32(Archive, Iterator->Index, "enter_type", &DungeonData->EntryType);
        ParseAttributeInt32(Archive, Iterator->Index, "user_type", &DungeonData->PlayerType);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "user_count", &DungeonData->MaxPlayerCount)) {
            DungeonData->MaxPlayerCount = 1;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "ObjCount", &DungeonData->ObjectCount);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "RemoveItem", &DungeonData->RemoveItem)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_cd_level", &DungeonData->EntryConditionLevel)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "max_user", &DungeonData->MaxPlayerCount)) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_class", DungeonData->EntryConditionClass, 2, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_warp_id", &DungeonData->EntryWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "fail_warp_npc_id", &DungeonData->FailWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dead_warp_id", &DungeonData->DeadWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &DungeonData->DungeonIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "penalty_value", &DungeonData->PenaltyValue)) goto error;

        DungeonData->PatternPartCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "pp_id",
            DungeonData->PatternPartIndices,
            RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT,
            ','
        );

        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "reward", DungeonData->Reward, 8, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "success_warp_npc_id", &DungeonData->SuccessWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData->WorldIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData->NextDungeonIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "warpnpc_set", &DungeonData->WarpNpcSetID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useoddcircle_count", &DungeonData->UseOddCircleCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData->DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useveradrix_count", &DungeonData->UseVeradrixCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData->IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData->EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData->MissionTimeout)) goto error;

        RTWorldDataRef WorldData = RTWorldDataGet(Runtime->WorldManager, DungeonData->WorldIndex);
        WorldData->HasMissionDungeon = true;

        DungeonData->TriggerGroups = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->ActionGroups = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->TimeControls = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->ImmuneControls = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->GateControls = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->DropTable.WorldDropPool = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTDropItem), 8);
        DungeonData->DropTable.MobDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);
        DungeonData->DropTable.QuestDropPool = IndexDictionaryCreate(Runtime->Allocator, 8);

        if (!ServerLoadDungeonExtraData(Context, RuntimeDirectory, ServerDirectory, DungeonData)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    struct _RTMissionDungeonPatternPartData PatternPartData = { 0 };

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        Int PatternPartIndex = 0;
        if (!ParseAttributeInt(Archive, Iterator->Index, "id", &PatternPartIndex)) goto error;
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

        CString PatternPartFilePath = CStringFormat(
            PathCombineAll(ServerDirectory, "World", "Dungeon", "PatternPart", "PatternPart_%zu.xml", NULL),
            PatternPartIndex
        );

        if (!ServerLoadPatternPartData(Context, PatternPartFilePath)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }
    
    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_mission_dungeon");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        Int DungeonIndex = 0;
        if (!ParseAttributeInt(Archive, Iterator->Index, "id", &DungeonIndex)) goto error;
        assert(DictionaryLookup(Runtime->DungeonData, &DungeonIndex));

        RTDungeonDataRef DungeonData = DictionaryLookup(Runtime->DungeonData, &DungeonIndex);
        assert(DungeonData);

        if (!ServerLoadDungeonMissionData(Context, RuntimeDirectory, ServerDirectory, DungeonData)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadWorldDropData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef TempArchive
) {
    RTRuntimeRef Runtime = Context->Runtime;
    CString DropFilePath = PathCombineAll(ServerDirectory, "World", "World_0.xml", NULL);

    if (FileExists(DropFilePath)) {
        Info("Loading world file: %s", DropFilePath);

        ArchiveClear(TempArchive, true);
        if (!ArchiveLoadFromFile(TempArchive, DropFilePath, false)) goto error;

        Int32 ParentIndex = ArchiveNodeGetChildByPath(TempArchive, -1, "World");
        if (ParentIndex < 0) goto error;

        if (!ServerLoadCommonDropData(Runtime, TempArchive, ParentIndex, Runtime->DropTable.WorldDropPool, "CommonDropPool")) goto error;
        if (!ServerLoadSpeciesDropData(Runtime, TempArchive, ParentIndex, Runtime->DropTable.MobDropPool, "MobDropPool", false)) goto error;
        if (!ServerLoadSpeciesDropData(Runtime, TempArchive, ParentIndex, Runtime->DropTable.QuestDropPool, "QuestDropPool", true)) goto error;

        ArchiveClear(TempArchive, true);
    }

    return true;

error:
    return false;
}

Bool ServerLoadMobPatrolData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    RTRuntimeRef Runtime = Context->Runtime;
    ArchiveRef Archive = ArchiveCreateEmpty(AllocatorGetSystemDefault());
    ArchiveRef PatrolArchive = ArchiveCreateEmpty(AllocatorGetSystemDefault());
    CString FilePath = PathCombineAll(ServerDirectory, "MobPatrolList.xml", NULL);

    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "MobPatrolList");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "MobPatrol");
    while (Iterator) {
        ArchiveClear(PatrolArchive, true);

        Int32 PatrolIndex = -1;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Index", &PatrolIndex)) goto error;

        Char FilePath[MAX_PATH] = { 0 };
        if (!ParseAttributeString(Archive, Iterator->Index, "FilePath", FilePath, MAX_PATH)) goto error;

        CString PatternFilePath = PathCombineAll(ServerDirectory, FilePath, NULL);
        if (!ArchiveLoadFromFile(PatrolArchive, PatternFilePath, false)) goto error;

        Int32 PatrolParentIndex = ArchiveNodeGetChildByPath(PatrolArchive, -1, "MobPatrol");
        if (PatrolParentIndex < 0) goto error;

        RTMobPatrolDataRef PatrolData = (RTMobPatrolDataRef)MemoryPoolReserve(Runtime->MobPatrolDataPool, PatrolIndex);
        PatrolData->Index = (Int32)PatrolIndex;
        PatrolData->Branches = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMobPatrolBranchData), 8);

        ArchiveIteratorRef BranchIterator = ArchiveQueryNodeIteratorFirst(PatrolArchive, PatrolParentIndex, "Branch");
        while (BranchIterator) {
            RTMobPatrolBranchDataRef BranchData = (RTMobPatrolBranchDataRef)ArrayAppendUninitializedElement(PatrolData->Branches);
            memset(BranchData, 0, sizeof(struct _RTMobPatrolBranchData));
            BranchData->Waypoints = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMobPatrolWaypointData), 8);

            if (!ParseAttributeInt32(PatrolArchive, BranchIterator->Index, "Index", &BranchData->Index)) goto error;

            BranchData->LinkCount = ParseAttributeInt32ArrayCounted(
                PatrolArchive,
                BranchIterator->Index,
                "LinkList",
                &BranchData->LinkList[0],
                RUNTIME_MOB_PATROL_MAX_LINK_COUNT,
                ','
            );

            ArchiveIteratorRef WaypointIterator = ArchiveQueryNodeIteratorFirst(PatrolArchive, BranchIterator->Index, "Waypoint");
            while (WaypointIterator) {
                RTMobPatrolWaypointDataRef WaypointData = (RTMobPatrolWaypointDataRef)ArrayAppendUninitializedElement(BranchData->Waypoints);
                memset(WaypointData, 0, sizeof(struct _RTMobPatrolWaypointData));

                if (!ParseAttributeInt32(PatrolArchive, WaypointIterator->Index, "Type", &WaypointData->Type)) goto error;
                if (!ParseAttributeInt32(PatrolArchive, WaypointIterator->Index, "X", &WaypointData->X)) goto error;
                if (!ParseAttributeInt32(PatrolArchive, WaypointIterator->Index, "Y", &WaypointData->Y)) goto error;
                if (!ParseAttributeInt32(PatrolArchive, WaypointIterator->Index, "Delay", &WaypointData->Delay)) goto error;

                WaypointIterator = ArchiveQueryNodeIteratorNext(PatrolArchive, WaypointIterator);
            }

            BranchIterator = ArchiveQueryNodeIteratorNext(PatrolArchive, BranchIterator);
        }

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveDestroy(Archive);
    ArchiveDestroy(PatrolArchive);
    return true;

error:
    ArchiveDestroy(Archive);
    ArchiveDestroy(PatrolArchive);
    return false;
}

Bool ServerLoadMobPatternData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    CString ScriptDirectory
) {
    RTRuntimeRef Runtime = Context->Runtime;
    ArchiveRef Archive = ArchiveCreateEmpty(AllocatorGetSystemDefault());
    ArchiveRef PatternArchive = ArchiveCreateEmpty(AllocatorGetSystemDefault());
    CString FilePath = PathCombineAll(ServerDirectory, "MobPatternList.xml", NULL);

    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "MobPatternList");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "MobPattern");
    while (Iterator) {
        ArchiveClear(PatternArchive, true);

        Int32 PatternIndex = -1;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Index", &PatternIndex)) goto error;

        Char FilePath[MAX_PATH] = { 0 };
        if (!ParseAttributeString(Archive, Iterator->Index, "FilePath", FilePath, MAX_PATH)) goto error;

        CString PatternFilePath = PathCombineAll(ServerDirectory, FilePath, NULL);
        if (!ArchiveLoadFromFile(PatternArchive, PatternFilePath, false)) goto error;

        Int32 PatternParentIndex = ArchiveNodeGetChildByPath(PatternArchive, -1, "MobPattern");
        if (PatternParentIndex < 0) goto error;

        RTMobPatternDataRef PatternData = (RTMobPatternDataRef)MemoryPoolReserve(Runtime->MobPatternDataPool, PatternIndex);
        PatternData->Index = (Int32)PatternIndex;
        PatternData->TriggerCount = 0;
        PatternData->MobPool = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMobPatternSpawnData), 8);

        ArchiveIteratorRef TriggerIterator = ArchiveQueryNodeIteratorFirst(PatternArchive, PatternParentIndex, "MobTriggerGroup");
        while (TriggerIterator) {
            assert(PatternData->TriggerCount < RUNTIME_MOB_PATTERN_MAX_TRIGGER_GROUP_COUNT);

            RTMobTriggerDataRef TriggerData = &PatternData->Triggers[PatternData->TriggerCount];
            memset(TriggerData, 0, sizeof(struct _RTMobTriggerData));
            PatternData->TriggerCount += 1;

            if (!ParseAttributeInt32(PatternArchive, TriggerIterator->Index, "Index", &TriggerData->Index)) goto error;
            if (!ParseAttributeInt32(PatternArchive, TriggerIterator->Index, "Type", &TriggerData->Type)) goto error;
            if (!ParseAttributeInt32Array(
                PatternArchive,
                TriggerIterator->Index,
                "Parameters",
                &TriggerData->Parameters.Memory.Values[0],
                RUNTIME_MOB_PATTERN_MAX_PARAMETER_COUNT,
                ':'
            )) goto error;

            ArchiveIteratorRef ActionGroupIterator = ArchiveQueryNodeIteratorFirst(PatternArchive, TriggerIterator->Index, "MobActionGroup");
            while (ActionGroupIterator) {
                assert(TriggerData->ActionGroupCount < RUNTIME_MOB_PATTERN_MAX_ACTION_GROUP_COUNT);

                RTMobActionGroupDataRef ActionGroupData = &TriggerData->ActionGroups[TriggerData->ActionGroupCount];
                memset(ActionGroupData, 0, sizeof(struct _RTMobActionGroupData));
                TriggerData->ActionGroupCount += 1;

                if (!ParseAttributeInt32(PatternArchive, ActionGroupIterator->Index, "Index", &ActionGroupData->Index)) goto error;
                if (!ParseAttributeInt32(PatternArchive, ActionGroupIterator->Index, "Delay", &ActionGroupData->Delay)) goto error;

                ArchiveIteratorRef ActionIterator = ArchiveQueryNodeIteratorFirst(PatternArchive, ActionGroupIterator->Index, "MobAction");
                while (ActionIterator) {
                    assert(ActionGroupData->ActionCount < RUNTIME_MOB_PATTERN_MAX_ACTION_COUNT);

                    RTMobActionDataRef ActionData = &ActionGroupData->Actions[ActionGroupData->ActionCount];
                    memset(ActionData, 0, sizeof(struct _RTMobActionData));
                    ActionGroupData->ActionCount += 1;

                    if (!ParseAttributeInt32(PatternArchive, ActionIterator->Index, "Index", &ActionData->Index)) goto error;
                    if (!ParseAttributeInt32(PatternArchive, ActionIterator->Index, "Type", &ActionData->Type)) goto error;
                    if (!ParseAttributeInt32Array(
                        PatternArchive,
                        ActionIterator->Index,
                        "Parameters",
                        &ActionData->Parameters.Memory.Values[0],
                        RUNTIME_MOB_PATTERN_MAX_PARAMETER_COUNT,
                        ':'
                    )) goto error;
                    if (!ParseAttributeInt32(PatternArchive, ActionIterator->Index, "Duration", &ActionData->Duration)) goto error;
                    if (!ParseAttributeInt32(PatternArchive, ActionIterator->Index, "TargetType", &ActionData->TargetType)) goto error;
                    if (!ParseAttributeInt32Array(
                        PatternArchive,
                        ActionIterator->Index,
                        "TargetParameters",
                        &ActionData->TargetParameters[0],
                        RUNTIME_MOB_PATTERN_MAX_PARAMETER_COUNT,
                        ':'
                    )) goto error;
                    if (!ParseAttributeInt32(PatternArchive, ActionIterator->Index, "Loop", &ActionData->Loop)) goto error;
                    if (!ParseAttributeInt32(PatternArchive, ActionIterator->Index, "LoopDelay", &ActionData->LoopDelay)) goto error;
                    if (!ParseAttributeInt32(PatternArchive, ActionIterator->Index, "Priority", &ActionData->Priority)) goto error;

                    ActionIterator = ArchiveQueryNodeIteratorNext(PatternArchive, ActionIterator);
                }

                ActionGroupIterator = ArchiveQueryNodeIteratorNext(PatternArchive, ActionGroupIterator);
            }

            TriggerIterator = ArchiveQueryNodeIteratorNext(PatternArchive, TriggerIterator);
        }

        ArchiveIteratorRef MobIterator = ArchiveQueryNodeIteratorFirst(PatternArchive, PatternParentIndex, "MobPool.Mob");
        while (MobIterator) {
            RTMobPatternSpawnDataRef MobSpawn = (RTMobPatternSpawnDataRef)ArrayAppendUninitializedElement(PatternData->MobPool);
            memset(MobSpawn, 0, sizeof(struct _RTMobPatternSpawnData));

            if (!ParseAttributeInt32(PatternArchive, MobIterator->Index, "MobIndex", &MobSpawn->Index)) {
                Error("Loading '%s' in '%s' failed!", "MobIndex", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(PatternArchive, MobIterator->Index, "SpeciesIndex", &MobSpawn->MobSpeciesIndex)) {
                Error("Loading '%s' in '%s' failed!", "SpeciesIndex", FilePath);
                goto error;
            }

            ParseAttributeInt32(PatternArchive, MobIterator->Index, "X", &MobSpawn->AreaX);
            ParseAttributeInt32(PatternArchive, MobIterator->Index, "Y", &MobSpawn->AreaY);

            if (!ParseAttributeInt32(PatternArchive, MobIterator->Index, "Width", &MobSpawn->AreaWidth)) {
                Error("Loading '%s' in '%s' failed!", "Width", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(PatternArchive, MobIterator->Index, "Height", &MobSpawn->AreaHeight)) {
                Error("Loading '%s' in '%s' failed!", "Height", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(PatternArchive, MobIterator->Index, "SpawnInterval", &MobSpawn->Interval)) {
                Error("Loading '%s' in '%s' failed!", "SpawnInterval", FilePath);
                goto error;
            }

            if (!ParseAttributeInt32(PatternArchive, MobIterator->Index, "SpawnCount", &MobSpawn->Count)) {
                Error("Loading '%s' in '%s' failed!", "SpawnCount", FilePath);
                goto error;
            }

            ParseAttributeInt32(PatternArchive, MobIterator->Index, "MobPatternIndex", &MobSpawn->MobPatternIndex);

            Char MobScriptFileName[MAX_PATH] = { 0 };
            if (ParseAttributeString(PatternArchive, MobIterator->Index, "Script", MobScriptFileName, MAX_PATH)) {
                if (strlen(MobScriptFileName) > 0) {
                    CString MobScriptFilePath = PathCombineAll(ScriptDirectory, MobScriptFileName, NULL);
                    MobSpawn->Script = RTScriptManagerLoadScript(Runtime->ScriptManager, MobScriptFilePath);
                }
            }

            MobIterator = ArchiveQueryNodeIteratorNext(PatternArchive, MobIterator);
        }

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveDestroy(Archive);
    ArchiveDestroy(PatternArchive);
    return true;

error:
    ArchiveDestroy(Archive);
    ArchiveDestroy(PatternArchive);
    return false;
}

Bool ServerLoadOptionPoolData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    ArchiveRef Archive = ArchiveCreateEmpty(AllocatorGetSystemDefault());
    CString FilePath = PathCombineAll(ServerDirectory, "Loot", "OptionPool.xml", NULL);
    if (FileExists(FilePath)) {
        Info("Loading option pool file: %s", FilePath);

        if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

        Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "OptionPool");
        if (ParentIndex < 0) goto error;

        ArchiveIteratorRef PoolIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "Pool");
        while (PoolIterator) {
            Int32 PoolIndex = 0;
            if (!ParseAttributeInt32(Archive, PoolIterator->Index, "Index", &PoolIndex)) goto error;

            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, PoolIterator->Index, "ItemLevelPool.ItemLevel");
            while (ChildIterator) {
                Int32 Level = 0;
                Float64 Rate = 0;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Level", &Level)) goto error;
                if (!ParseAttributeFloat64(Archive, ChildIterator->Index, "Rate", &Rate)) goto error;

                RTOptionPoolManagerAddItemLevel(Context->Runtime->OptionPoolManager, PoolIndex, Level, Rate);

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, PoolIterator->Index, "EpicLevelPool.EpicLevel");
            while (ChildIterator) {
                Int32 Level = 0;
                Float64 Rate = 0;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Level", &Level)) goto error;
                if (!ParseAttributeFloat64(Archive, ChildIterator->Index, "Rate", &Rate)) goto error;

                RTOptionPoolManagerAddEpicLevel(Context->Runtime->OptionPoolManager, PoolIndex, Level, Rate);

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, PoolIterator->Index, "EpicOptionPool.EpicOptionGroup");
            while (ChildIterator) {
                Int32 ItemType = 0;
                Int32 Level = 0;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "ItemType", &ItemType)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Level", &Level)) goto error;

                ArchiveIteratorRef SubChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ChildIterator->Index, "EpicOption");
                while (SubChildIterator) {
                    Int32 ForceIndex = 0;
                    Float64 Rate = 0;
                    if (!ParseAttributeInt32(Archive, SubChildIterator->Index, "ForceIndex", &ForceIndex)) goto error;
                    if (!ParseAttributeFloat64(Archive, SubChildIterator->Index, "Rate", &Rate)) goto error;

                    RTOptionPoolManagerAddEpicOption(Context->Runtime->OptionPoolManager, PoolIndex, ItemType, Level, ForceIndex, Rate);

                    SubChildIterator = ArchiveQueryNodeIteratorNext(Archive, SubChildIterator);
                }

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, PoolIterator->Index, "ForceSlotPool.ForceSlot");
            while (ChildIterator) {
                Int32 Count = 0;
                Float64 Rate = 0;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Count", &Count)) goto error;
                if (!ParseAttributeFloat64(Archive, ChildIterator->Index, "Rate", &Rate)) goto error;

                RTOptionPoolManagerAddForceSlot(Context->Runtime->OptionPoolManager, PoolIndex, Count, Rate);

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, PoolIterator->Index, "ForceOptionSlotPool.ForceOptionSlot");
            while (ChildIterator) {
                Int32 Count = 0;
                Float64 Rate = 0;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Count", &Count)) goto error;
                if (!ParseAttributeFloat64(Archive, ChildIterator->Index, "Rate", &Rate)) goto error;

                RTOptionPoolManagerAddForceOptionSlot(Context->Runtime->OptionPoolManager, PoolIndex, Count, Rate);

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, PoolIterator->Index, "ForceOptionPool.ForceOptionGroup");
            while (ChildIterator) {
                Int32 ItemType = 0;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "ItemType", &ItemType)) goto error;

                ArchiveIteratorRef SubChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ChildIterator->Index, "ForceOption");
                while (SubChildIterator) {
                    Int32 ForceIndex = 0;
                    Float64 Rate = 0;
                    if (!ParseAttributeInt32(Archive, SubChildIterator->Index, "ForceIndex", &ForceIndex)) goto error;
                    if (!ParseAttributeFloat64(Archive, SubChildIterator->Index, "Rate", &Rate)) goto error;

                    RTOptionPoolManagerAddForceOption(Context->Runtime->OptionPoolManager, PoolIndex, ItemType, ForceIndex, Rate);

                    SubChildIterator = ArchiveQueryNodeIteratorNext(Archive, SubChildIterator);
                }

                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            PoolIterator = ArchiveQueryNodeIteratorNext(Archive, PoolIterator);
        }

        ArchiveClear(Archive, true);
    }

    ArchiveDestroy(Archive);
    return true;

error:
    ArchiveDestroy(Archive);
    return false;
}
