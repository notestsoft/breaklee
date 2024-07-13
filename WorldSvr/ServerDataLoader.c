#include "ServerDataLoader.h"

#pragma pack(push, 1)

struct _ArchiveItemData {
    Int32 ItemID;
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

    struct {
        UInt32 Height : 2;
        UInt32 Width : 2;
    } SlotSize;

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
    ArchiveRef RankArchive,
    ArchiveRef CharacterInitArchive
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

    Archive = CharacterInitArchive;
    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "server_character_init");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "character_init");
    while (Iterator) {
        Int32 BattleStyleIndex = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "style", &BattleStyleIndex)) goto error;

        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        struct _RuntimeDataCharacterTemplate* CharacterTemplate = &Context->RuntimeData->CharacterTemplate[BattleStyleIndex - 1];
        CharacterTemplate->BattleStyleIndex = BattleStyleIndex;
        CharacterTemplate->SkillSlots.Count = 0;

        ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "skill");
        while (ChildIterator) {
            RTSkillSlotRef SkillSlot = &CharacterTemplate->SkillSlots.Skills[CharacterTemplate->SkillSlots.Count];

            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "id", &SkillSlot->ID)) goto error;
            if (!ParseAttributeUInt8(Archive, ChildIterator->Index, "level", &SkillSlot->Level)) goto error;
            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "slot", &SkillSlot->Index)) goto error;

            CharacterTemplate->SkillSlots.Count += 1;

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        CharacterTemplate->QuickSlots.Count = 0;

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "quickslot");
        while (ChildIterator) {
            RTQuickSlotRef QuickSlot = &CharacterTemplate->QuickSlots.QuickSlots[CharacterTemplate->QuickSlots.Count];

            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "skill", &QuickSlot->SkillIndex)) goto error;
            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "slot", &QuickSlot->SlotIndex)) goto error;

            CharacterTemplate->QuickSlots.Count += 1;

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "inventory");
        while (ChildIterator) {
            struct _RTItemSlot Slot = { 0 };

            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "slot", &Slot.SlotIndex)) goto error;
            if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "item_id", &Slot.Item.ID)) goto error;
            if (!ParseAttributeUInt64(Archive, ChildIterator->Index, "item_option", &Slot.ItemOptions)) goto error;
            
            if (!RTInventorySetSlot(Context->Runtime, &CharacterTemplate->Inventory, &Slot)) goto error;

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

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

    ArchiveRef Messages = ArchiveCreateEmpty(Context->Runtime->Allocator);
    Bool Success = ArchiveLoadFromFile(Messages, PathCombineNoAlloc(RuntimeDirectory, "Language\\Korean\\cabal_msg.enc.xml"), false);

    for (Int32 FileIndex = 0; FileIndex < 4; FileIndex++) {
        Char FilePath[MAX_PATH];
        PathCombine(RuntimeDirectory, FileNames[FileIndex], FilePath);

        UInt8* Buffer = NULL;
        Int32 BufferLength = 0;

        if (!EncryptionDecryptFile(FilePath, &Buffer, &BufferLength)) {
            goto error;
        }

        struct _ArchiveItemData* ItemData = (struct _ArchiveItemData*)Buffer;
        Int32 ItemCount = BufferLength / sizeof(struct _ArchiveItemData);

        for (Int32 Index = 0; Index < ItemCount; Index += 1) {
            assert(Runtime->ItemDataCount < RUNTIME_MEMORY_MAX_ITEM_DATA_COUNT);

            struct _ArchiveItemData* ArchiveItemData = &ItemData[Index];
            RTItemDataRef ItemData = &Runtime->ItemData[Runtime->ItemDataCount];
            ItemData->ItemID = ArchiveItemData->ItemID;
            ItemData->ItemType = ArchiveItemData->ItemType;
            ItemData->ItemGrade = ArchiveItemData->Grade;
            ItemData->SellPrice = ArchiveItemData->SellPrice;
            ItemData->BuyPrice = 0;
            ItemData->MinLevel = ArchiveItemData->MinLevel;
            memcpy(ItemData->Options, ArchiveItemData->Options, sizeof(ArchiveItemData->Options));
            ItemData->Width = ArchiveItemData->SlotSize.Width;
            ItemData->Height = ArchiveItemData->SlotSize.Height;
            ItemData->MaxStackSize = ArchiveItemData->MaxStackSize;
            ItemData->UniqueGrade = ArchiveItemData->UniqueGrade;
            ItemData->MasterGrade = ArchiveItemData->MeisterGrade;
            ItemData->EnchantCodeLink = ArchiveItemData->EnchantCodeLink;
            memcpy(ItemData->ExtendedOptions, ArchiveItemData->ExtendedOptions, sizeof(ArchiveItemData->ExtendedOptions));
            Runtime->ItemDataCount += 1;

            if (ArchiveItemData->ItemType == RUNTIME_ITEM_TYPE_CHAOS_SAFEGUARD) {
                Trace("ChaosSafeGuard(%d, %d, %s)",
                    ItemData->ItemID,
                    ArchiveItemData->Options[0],
                    GetItemDescription(Messages, ArchiveItemData->ItemName)
                );
            }
            /*
            if (ArchiveItemData->MeisterGrade > 0) {
                Trace("Meister(%d, %d, %s)",
                    ItemData->ItemID,
                    ArchiveItemData->MeisterGrade,
                    GetItemDescription(Messages, ArchiveItemData->ItemName)
                );
            }

            if (
                ItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_CHAKRAM
            ) {
                Trace("Weapon(%d) : Options(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
                    ItemData->ItemID,
                    ItemData->Weapon.UnknownOptions1[0],
                    ItemData->Weapon.UnknownOptions1[1],
                    ItemData->Weapon.UnknownOptions1[2],
                    ItemData->Weapon.UnknownOptions1[3],
                    ItemData->Weapon.UnknownOptions1[4],
                    ItemData->Weapon.UnknownOptions1[5],
                    ItemData->Weapon.UnknownOptions2[0],
                    ItemData->Weapon.UnknownOptions2[1],
                    ItemData->Weapon.UnknownOptions2[2],
                    ItemData->Weapon.UnknownOptions2[3],
                    ItemData->Weapon.UnknownOptions2[4],
                    ItemData->Weapon.UnknownOptions2[5]
                );
            }

            if (
                ItemData->ItemType == RUNTIME_ITEM_TYPE_SUIT ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_GLOVES ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_BOOTS ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_HELMED1 ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BIKE ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BOARD
            ) {
                    Trace("Armor(%d) : Options(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
                        ItemData->ItemID,
                        ItemData->Armor.UnknownOptions1[0],
                        ItemData->Armor.UnknownOptions1[1],
                        ItemData->Armor.UnknownOptions1[2],
                        ItemData->Armor.UnknownOptions1[3],
                        ItemData->Armor.UnknownOptions1[4],
                        ItemData->Armor.UnknownOptions1[5],
                        ItemData->Armor.UnknownOptions2[0],
                        ItemData->Armor.UnknownOptions2[1],
                        ItemData->Armor.UnknownOptions2[2],
                        ItemData->Armor.UnknownOptions2[3],
                        ItemData->Armor.UnknownOptions2[4],
                        ItemData->Armor.UnknownOptions2[5]
                    );
                
            }

            if (
                ItemData->ItemType == RUNTIME_ITEM_TYPE_AMULET ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_BELT ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_BOOTS ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_BRACELET ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_BROOCH ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_CHARM ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_CREST ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_EARRING ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_EPAULET ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_RING ||
                ItemData->ItemType == RUNTIME_ITEM_TYPE_ARTIFACT
            ) {
                    Trace("Accessory(%d) : Options(%d, %d, %d, %d, %d, %d, %d, %d, %d)",
                        ItemData->ItemID,
                        ItemData->Accessory.UnknownOptions1[0],
                        ItemData->Accessory.UnknownOptions1[1], // Accessory Epaulet Dmg reduction
                        ItemData->Accessory.UnknownOptions1[2],
                        ItemData->Accessory.UnknownOptions2[0],
                        ItemData->Accessory.UnknownOptions2[1],
                        ItemData->Accessory.UnknownOptions2[2],
                        ItemData->Accessory.UnknownOptions2[3],
                        ItemData->Accessory.UnknownOptions2[4],
                        ItemData->Accessory.UnknownOptions2[5]
                    );
            }
                */
        }

        free(Buffer);
    }

    ArchiveDestroy(Messages);

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

error:
    return false;
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
    Char MapFileName[MAX_PATH];
    Char MapFilePath[MAX_PATH];

    for (Index WorldIndex = 0; WorldIndex < Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!RTWorldDataExists(Runtime->WorldManager, WorldIndex)) continue;
        
        RTWorldDataRef World = RTWorldDataGet(Runtime->WorldManager, WorldIndex);

        Char WorldFilePath[MAX_PATH];
        Char WorldFileName[MAX_PATH];
        sprintf(WorldFileName, "world%zu.enc", World->WorldIndex);
        PathCombine(RuntimeDirectory, "World", WorldFilePath);
        PathCombine(WorldFilePath, WorldFileName, WorldFilePath);
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
            PathCombine(RuntimeDirectory, "Map", MapFilePath);
            PathCombine(MapFilePath, MapFileName, MapFilePath);
            Info("Loading map file: %s", MapFileName);

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

            UInt32 EffectCount = *((UInt32*)&MapData[MapDataOffset]);
            MapDataOffset += 4;

            for (Int32 Index = 0; Index < EffectCount; Index++) {
                UInt16 TextLength = *((UInt16*)&MapData[MapDataOffset]);
                MapDataOffset += 18;
                MapDataOffset += TextLength;
            }

            UInt32 TextureCount = *((UInt32*)&MapData[MapDataOffset]);
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
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MissionGate", &Mob->Spawn.MissionGate)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "PerfectDrop", &Mob->Spawn.PerfectDrop)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Type", &Mob->Spawn.Type)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Min", &Mob->Spawn.Min)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Max", &Mob->Spawn.Max)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Authority", &Mob->Spawn.Authority)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Server_Mob", &Mob->Spawn.ServerMobID)) goto error;
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
        if (FileExists(DropWorldFilePath)) {
            if (!ArchiveLoadFromFile(Archive, DropWorldFilePath, false)) goto error;

            Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "drops");
            if (ParentIndex < 0) goto error;

            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "item");
            while (ChildIterator) {
                assert(World->DropTable.WorldItemCount < RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT);

                RTWorldDropItemRef DropItem = &World->DropTable.WorldItems[World->DropTable.WorldItemCount];

                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "ItemKind", &DropItem->Item.ItemID.ID)) goto error;
                if (!ParseAttributeUInt64(Archive, ChildIterator->Index, "ItemOpt", &DropItem->Item.ItemOptions)) goto error;

                Float64 ScalarDropRate = 0.0;
                if (!ParseAttributeFloat64(Archive, ChildIterator->Index, "DropRate", &ScalarDropRate)) goto error;
                DropItem->Item.DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MinLv", &DropItem->MinMobLevel)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MaxLv", &DropItem->MaxMobLevel)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "OptPoolIdx", &DropItem->Item.OptionPoolIndex)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "DurationIdx", &DropItem->Item.DurationIndex)) goto error;

                World->DropTable.WorldItemCount += 1;
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveClear(Archive, true);
        }

        Char DropMobFilePath[MAX_PATH];
        Char DropMobFileName[MAX_PATH]; 

        sprintf(DropMobFileName, "world%zu-terrain-mob.xml", World->WorldIndex);
        PathCombine(ServerDirectory, DropMobFileName, DropMobFilePath);
        if (FileExists(DropMobFilePath)) {
            if (!ArchiveLoadFromFile(Archive, DropMobFilePath, false)) goto error;

            Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "drops");
            if (ParentIndex < 0) goto error;

            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "item");
            while (ChildIterator) {
                assert(World->DropTable.MobItemCount < RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT);

                RTMobDropItemRef DropItem = &World->DropTable.MobItems[World->DropTable.MobItemCount];

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpeciesIdx", &DropItem->MobSpeciesIndex)) goto error;
                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "ItemKind", &DropItem->Item.ItemID.ID)) goto error;
                if (!ParseAttributeUInt64(Archive, ChildIterator->Index, "ItemOpt", &DropItem->Item.ItemOptions)) goto error;

                Float64 ScalarDropRate = 0.0;
                if (!ParseAttributeFloat64(Archive, ChildIterator->Index, "DropRate", &ScalarDropRate)) goto error;
                DropItem->Item.DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "OptPoolIdx", &DropItem->Item.OptionPoolIndex)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "DurationIdx", &DropItem->Item.DurationIndex)) goto error;

                World->DropTable.MobItemCount += 1;
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveClear(Archive, true);
        }

        Char DropQuestFilePath[MAX_PATH];
        Char DropQuestFileName[MAX_PATH];

        sprintf(DropQuestFileName, "world%zu-terrain-quest.xml", World->WorldIndex);
        PathCombine(ServerDirectory, DropQuestFileName, DropQuestFilePath);
        if (FileExists(DropQuestFilePath)) {
            if (!ArchiveLoadFromFile(Archive, DropQuestFilePath, false)) goto error;

            Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "drops");
            if (ParentIndex < 0) goto error;

            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "item");
            while (ChildIterator) {
                assert(World->DropTable.QuestItemCount < RUNTIME_MEMORY_MAX_QUEST_DROP_ITEM_COUNT);

                RTQuestDropItemRef DropItem = &World->DropTable.QuestItems[World->DropTable.QuestItemCount];

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpeciesIdx", &DropItem->MobSpeciesIndex)) goto error;
                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "ItemKind", &DropItem->Item.ItemID.ID)) goto error;
                if (!ParseAttributeUInt64(Archive, ChildIterator->Index, "ItemOpt", &DropItem->Item.ItemOptions)) goto error;

                Float64 ScalarDropRate = 0.0;
                if (!ParseAttributeFloat64(Archive, ChildIterator->Index, "DropRate", &ScalarDropRate)) goto error;
                DropItem->Item.DropRate = (Int32)(ScalarDropRate / 100.0 * INT32_MAX);

                World->DropTable.QuestItemCount += 1;
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

Bool ServerLoadDungeonData(
    ServerContextRef Context,
    CString RuntimeDirectory,
    CString ServerDirectory,
    ArchiveRef Cont1Archive,
    ArchiveRef Cont2Archive,
    ArchiveRef Cont3Archive
) {
    RTRuntimeRef Runtime = Context->Runtime;
    ArchiveRef Archive = Cont1Archive;
    ArchiveRef TempArchive = ArchiveCreateEmpty(AllocatorGetSystemDefault());

    Int64 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_quest_dungeon");
    if (ParentIndex < 0) goto error;

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        assert(Runtime->DungeonDataCount < RUNTIME_MEMORY_MAX_DUNGEON_COUNT);

        RTDungeonDataRef DungeonData = &Runtime->DungeonData[Runtime->DungeonDataCount];
        memset(DungeonData, 0, sizeof(struct _RTDungeonData));

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
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &DungeonData->DungeonID)) goto error;
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
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData->WorldID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData->NextDungeonID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData->DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData->IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData->EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData->MissionTimeout)) goto error;

        RTWorldDataRef WorldData = RTWorldDataGet(Runtime->WorldManager, DungeonData->WorldID);
        WorldData->HasQuestDungeon = true;

        Runtime->DungeonDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    // TODO: Load additional pattern part data
    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        assert(Runtime->MissionDungeonPatternPartDataCount < RUNTIME_MEMORY_MAX_DUNGEON_PATTERN_PART_COUNT);

        RTMissionDungeonPatternPartDataRef PatternPartData = &Runtime->MissionDungeonPatternPartData[Runtime->MissionDungeonPatternPartDataCount];
        memset(PatternPartData, 0, sizeof(struct _RTMissionDungeonPatternPartData));

        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &PatternPartData->ID)) goto error;

        PatternPartData->MobTable = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMob), 8);

        struct {
            Int32 MobID;
            Int32 MaxCount;
        } MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];

        PatternPartData->MissionMobCount = ParseAttributeInt32Array2D(
            Archive,
            Iterator->Index,
            "mission_mobs",
            (Int32 *)&MissionMobs[0],
            2, // NOTE: The old quest dungeon data has old format for mission mobs field.
            RUNTIME_MAX_QUEST_COUNTER_COUNT,
            ':',
            ','
        );

        for (Index Index = 0; Index < PatternPartData->MissionMobCount; Index += 1) {
            PatternPartData->MissionMobs[Index].MobID = MissionMobs[Index].MobID;
            PatternPartData->MissionMobs[Index].Count = MissionMobs[Index].MaxCount;
            PatternPartData->MissionMobs[Index].MaxCount = MissionMobs[Index].MaxCount;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "mission_npc", &PatternPartData->MissionNpcID);

        Runtime->MissionDungeonPatternPartDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Archive = Cont2Archive;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_mission_dungeon");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        assert(Runtime->DungeonDataCount < RUNTIME_MEMORY_MAX_DUNGEON_COUNT);

        RTDungeonDataRef DungeonData = &Runtime->DungeonData[Runtime->DungeonDataCount];
        memset(DungeonData, 0, sizeof(struct _RTDungeonData));
        
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
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &DungeonData->DungeonID)) goto error;
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
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData->WorldID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData->NextDungeonID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "warpnpc_set", &DungeonData->WarpNpcSetID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useoddcircle_count", &DungeonData->UseOddCircleCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData->DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useveradrix_count", &DungeonData->UseVeradrixCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData->IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData->EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData->MissionTimeout)) goto error;

        RTWorldDataRef WorldData = RTWorldDataGet(Runtime->WorldManager, DungeonData->WorldID);
        WorldData->HasMissionDungeon = true;

        Runtime->DungeonDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        assert(Runtime->MissionDungeonPatternPartDataCount < RUNTIME_MEMORY_MAX_DUNGEON_PATTERN_PART_COUNT);

        RTMissionDungeonPatternPartDataRef PatternPartData = &Runtime->MissionDungeonPatternPartData[Runtime->MissionDungeonPatternPartDataCount];
        memset(PatternPartData, 0, sizeof(struct _RTMissionDungeonPatternPartData));

        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &PatternPartData->ID)) goto error;

        PatternPartData->MobTable = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMob), 8);
        PatternPartData->MissionMobCount = ParseAttributeInt32Array2D(
            Archive,
            Iterator->Index,
            "mission_mobs",
            (Int32 *)&PatternPartData->MissionMobs[0],
            sizeof(struct _RTQuestUnitMobData) / sizeof(Int32),
            RUNTIME_MAX_QUEST_COUNTER_COUNT,
            ':',
            ','
        );

        ParseAttributeInt32(Archive, Iterator->Index, "mission_npc", &PatternPartData->MissionNpcID);

        Runtime->MissionDungeonPatternPartDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Archive = Cont3Archive;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_mission_dungeon");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        assert(Runtime->DungeonDataCount < RUNTIME_MEMORY_MAX_DUNGEON_COUNT);

        RTDungeonDataRef DungeonData = &Runtime->DungeonData[Runtime->DungeonDataCount];
        memset(DungeonData, 0, sizeof(struct _RTDungeonData));

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
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &DungeonData->DungeonID)) goto error;
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
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData->WorldID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData->NextDungeonID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "warpnpc_set", &DungeonData->WarpNpcSetID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useoddcircle_count", &DungeonData->UseOddCircleCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData->DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useveradrix_count", &DungeonData->UseVeradrixCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData->IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData->EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData->MissionTimeout)) goto error;

        RTWorldDataRef WorldData = RTWorldDataGet(Runtime->WorldManager, DungeonData->WorldID);
        if (!WorldData) {
            Info("No world (%d) found for dungeon (%d)", DungeonData->WorldID, DungeonData->DungeonID);
            Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
            continue;
        }

        WorldData->HasMissionDungeon = true;

        Runtime->DungeonDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        assert(Runtime->MissionDungeonPatternPartDataCount < RUNTIME_MEMORY_MAX_DUNGEON_PATTERN_PART_COUNT);

        RTMissionDungeonPatternPartDataRef PatternPartData = &Runtime->MissionDungeonPatternPartData[Runtime->MissionDungeonPatternPartDataCount];
        memset(PatternPartData, 0, sizeof(struct _RTMissionDungeonPatternPartData));

        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &PatternPartData->ID)) goto error;

        PatternPartData->MobTable = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMob), 8);
        PatternPartData->MissionMobCount = ParseAttributeInt32Array2D(
            Archive,
            Iterator->Index,
            "mission_mobs",
            (Int32 *)&PatternPartData->MissionMobs[0],
            sizeof(struct _RTQuestUnitMobData) / sizeof(Int32),
            RUNTIME_MAX_QUEST_COUNTER_COUNT,
            ':',
            ','
        );

        ParseAttributeInt32(Archive, Iterator->Index, "mission_npc", &PatternPartData->MissionNpcID);

        Runtime->MissionDungeonPatternPartDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }


    Char FilePath[MAX_PATH];
    ArchiveClear(Archive, true);
    PathCombine(ServerDirectory, "quest_dungeon_mobs.xml", FilePath);

    Archive = TempArchive;
    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest_dungeon_mobs");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "mob");
    while (Iterator) {
        Int32 PatternPartID;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PPIdx", &PatternPartID)) goto error;

        RTMissionDungeonPatternPartDataRef PatternPartData = RTRuntimeGetPatternPartByID(Runtime, PatternPartID);
        assert(PatternPartData);

        RTMobRef Mob = (RTMobRef)ArrayAppendUninitializedElement(PatternPartData->MobTable);
        memset(Mob, 0, sizeof(struct _RTMob));

        Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

        if (!ParseAttributeUInt16(Archive, Iterator->Index, "MobIdx", &Mob->ID.EntityIndex)) {
            Error("Loading '%s' in '%s' failed!", "MobIdx", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeIndex(Archive, Iterator->Index, "PPIdx", &Mob->Spawn.PatternPartIndex)) {
            Error("Loading '%s' in '%s' failed!", "PPIdx", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeIndex(Archive, Iterator->Index, "SpeciesIdx", &Mob->Spawn.MobSpeciesIndex)) {
            Error("Loading '%s' in '%s' failed!", "SpeciesIdx", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "PosX", &Mob->Spawn.AreaX)) {
            Error("Loading '%s' in '%s' failed!", "PosX", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "PosY", &Mob->Spawn.AreaY)) {
            Error("Loading '%s' in '%s' failed!", "PosY", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Width", &Mob->Spawn.AreaWidth)) {
            Error("Loading '%s' in '%s' failed!", "Width", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Height", &Mob->Spawn.AreaHeight)) {
            Error("Loading '%s' in '%s' failed!", "Height", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpwnInterval", &Mob->Spawn.SpawnInterval)) {
            Error("Loading '%s' in '%s' failed!", "SpwnInterval", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpwnCount", &Mob->Spawn.SpawnCount)) {
            Error("Loading '%s' in '%s' failed!", "SpwnCount", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpawnDefault", &Mob->Spawn.SpawnDefault)) {
            Error("Loading '%s' in '%s' failed!", "SpawnDefault", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Grade", &Mob->Spawn.Grade)) {
            Error("Loading '%s' in '%s' failed!", "Grade", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Lv", &Mob->Spawn.Level)) {
            Error("Loading '%s' in '%s' failed!", "Lv", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "MissionGate", &Mob->Spawn.MissionGate)) {
            Error("Loading '%s' in '%s' failed!", "MissionGate", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "PerfectDrop", &Mob->Spawn.PerfectDrop)) {
            Error("Loading '%s' in '%s' failed!", "PerfectDrop", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Type", &Mob->Spawn.Type)) {
            Error("Loading '%s' in '%s' failed!", "Type", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Min", &Mob->Spawn.Min)) {
            Error("Loading '%s' in '%s' failed!", "Min", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Max", &Mob->Spawn.Max)) {
            Error("Loading '%s' in '%s' failed!", "Max", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Authority", &Mob->Spawn.Authority)) {
            Error("Loading '%s' in '%s' failed!", "Authority", "quest_dungeon_mobs.xml");
            goto error;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "TrgIdxSpawn", &Mob->Spawn.SpawnTriggerID);
        ParseAttributeInt32(Archive, Iterator->Index, "TrgIdxKill", &Mob->Spawn.KillTriggerID);
        ParseAttributeInt32(Archive, Iterator->Index, "Server_Mob", &Mob->Spawn.ServerMobID);
        ParseAttributeInt32(Archive, Iterator->Index, "Loot_Delay", &Mob->Spawn.LootDelay);

        ParseAttributeInt32Array(Archive, Iterator->Index, "EvtProperty", Mob->Spawn.EventProperty, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
        ParseAttributeInt32Array(Archive, Iterator->Index, "EvtMobs", Mob->Spawn.EventMobs, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
        ParseAttributeInt32Array(Archive, Iterator->Index, "EvtInterval", Mob->Spawn.EventInterval, RUNTIME_MOB_MAX_EVENT_COUNT, ',');

        Mob->SpeciesData = &Runtime->MobData[Mob->Spawn.MobSpeciesIndex];
        Mob->IsInfiniteSpawn = true;
        Mob->IsPermanentDeath = false;
        Mob->RemainingSpawnCount = 0;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    PathCombine(ServerDirectory, "mission_dungeon_trigger_to_event.xml", FilePath);

    Archive = TempArchive;
    if (!ArchiveLoadFromFile(Archive, FilePath, false)) {
        Error("Loading archive '%s' failed!", "mission_dungeon_trigger_to_event.xml");
        goto error;
    }

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest_dungeon_trigger_to_event");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "trigger");
    while (Iterator) {
        Int32 DungeonID;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "QDungeonIdx", &DungeonID)) {
            Error("Loading '%s' in '%s' failed!", "QDungeonIdx", "quest_dungeon_trigger_to_event.xml");
            goto error;
        }

        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, DungeonID);
        assert(DungeonData);
        assert(DungeonData->TriggerCount < RUNTIME_MEMORY_DUNGEON_TRIGGER_DATA_COUNT);

        RTDungeonTriggerDataRef TriggerData = &DungeonData->TriggerData[DungeonData->TriggerCount];
        memset(TriggerData, 0, sizeof(struct _RTDungeonTriggerData));        
        TriggerData->DungeonID = DungeonID;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "TrgIdx", &TriggerData->TriggerID)) {
            Error("Loading '%s' in '%s' failed!", "TrgIdx", "quest_dungeon_trigger_to_event.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "TrgType", &TriggerData->TriggerType)) {
            Error("Loading '%s' in '%s' failed!", "TrgType", "quest_dungeon_trigger_to_event.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "TrgNpcIdx", &TriggerData->TriggerNpcID)) {
            Error("Loading '%s' in '%s' failed!", "TrgNpcIdx", "quest_dungeon_trigger_to_event.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "EvtActGroupIdx", &TriggerData->ActionGroupID)) {
            Error("Loading '%s' in '%s' failed!", "EvtActGroupIdx", "quest_dungeon_trigger_to_event.xml");
            goto error;
        }
        
        TriggerData->LiveMobCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "LiveStateMMapIdx",
            TriggerData->LiveMobs,
            RUNTIME_DUNGEON_TRIGGER_MAX_MOB_COUNT,
            ','
        );

        TriggerData->DeadMobCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "DeadStateMMapIdx",
            TriggerData->DeadMobs,
            RUNTIME_DUNGEON_TRIGGER_MAX_MOB_COUNT,
            ','
        );

        DungeonData->TriggerCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveClear(Archive, true);
    PathCombine(ServerDirectory, "mission_dungeon_act_group.xml", FilePath);

    Archive = TempArchive;
    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest_dungeon_act_group");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "act");
    while (Iterator) {
        Int32 DungeonID;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "QDungeonIdx", &DungeonID)) goto error;

        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, DungeonID);
        assert(DungeonData);
        assert(DungeonData->TriggerActionCount < RUNTIME_MEMORY_DUNGEON_TRIGGER_ACTION_DATA_COUNT);
        
        RTDungeonTriggerActionDataRef TriggerActionData = &DungeonData->TriggerActionData[DungeonData->TriggerActionCount];
        memset(TriggerActionData, 0, sizeof(struct _RTDungeonTriggerActionData));
        TriggerActionData->DungeonID = DungeonID;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "EvtActGroupIdx", &TriggerActionData->ActionGroupID)) {
            Error("Loading '%s' in '%s' failed!", "EvtActGroupIdx", "quest_dungeon_act_group.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "TgtAction", &TriggerActionData->ActionType)) {
            Error("Loading '%s' in '%s' failed!", "TgtAction", "quest_dungeon_act_group.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "TgtMMapIdx", &TriggerActionData->TargetID)) {
            Error("Loading '%s' in '%s' failed!", "TgtMMapIdx", "quest_dungeon_act_group.xml");
            goto error;
        }

        if (!ParseAttributeUInt64(Archive, Iterator->Index, "EvtDelay", &TriggerActionData->Delay)) {
            Error("Loading '%s' in '%s' failed!", "EvtDelay", "quest_dungeon_act_group.xml");
            goto error;
        }

        DungeonData->TriggerActionCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveClear(Archive, true);
    PathCombine(ServerDirectory, "mission_dungeon_mobs.xml", FilePath);

    Archive = TempArchive;
    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest_dungeon_mobs");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "mob");
    while (Iterator) {
        Int32 PatternPartID;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PPIdx", &PatternPartID)) goto error;

        RTMissionDungeonPatternPartDataRef PatternPartData = RTRuntimeGetPatternPartByID(Runtime, PatternPartID);
        assert(PatternPartData);

        RTMobRef Mob = (RTMobRef)ArrayAppendUninitializedElement(PatternPartData->MobTable);
        memset(Mob, 0, sizeof(struct _RTMob));

        Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

        if (!ParseAttributeUInt16(Archive, Iterator->Index, "MobIdx", &Mob->ID.EntityIndex)) {
            Error("Loading '%s' in '%s' failed!", "MobIdx", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeIndex(Archive, Iterator->Index, "PPIdx", &Mob->Spawn.PatternPartIndex)) {
            Error("Loading '%s' in '%s' failed!", "PPIdx", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeIndex(Archive, Iterator->Index, "SpeciesIdx", &Mob->Spawn.MobSpeciesIndex)) {
            Error("Loading '%s' in '%s' failed!", "SpeciesIdx", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "PosX", &Mob->Spawn.AreaX)) {
            Error("Loading '%s' in '%s' failed!", "PosX", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "PosY", &Mob->Spawn.AreaY)) {
            Error("Loading '%s' in '%s' failed!", "PosY", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Width", &Mob->Spawn.AreaWidth)) {
            Error("Loading '%s' in '%s' failed!", "Width", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Height", &Mob->Spawn.AreaHeight)) {
            Error("Loading '%s' in '%s' failed!", "Height", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpwnInterval", &Mob->Spawn.SpawnInterval)) {
            Error("Loading '%s' in '%s' failed!", "SpwnInterval", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpwnCount", &Mob->Spawn.SpawnCount)) {
            Error("Loading '%s' in '%s' failed!", "SpwnCount", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpawnDefault", &Mob->Spawn.SpawnDefault)) {
            Error("Loading '%s' in '%s' failed!", "SpawnDefault", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Grade", &Mob->Spawn.Grade)) {
            Error("Loading '%s' in '%s' failed!", "Grade", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Lv", &Mob->Spawn.Level)) {
            Error("Loading '%s' in '%s' failed!", "Lv", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "MissionGate", &Mob->Spawn.MissionGate)) {
            Error("Loading '%s' in '%s' failed!", "MissionGate", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "PerfectDrop", &Mob->Spawn.PerfectDrop)) {
            Error("Loading '%s' in '%s' failed!", "PerfectDrop", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Type", &Mob->Spawn.Type)) {
            Error("Loading '%s' in '%s' failed!", "Type", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Min", &Mob->Spawn.Min)) {
            Error("Loading '%s' in '%s' failed!", "Min", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Max", &Mob->Spawn.Max)) {
            Error("Loading '%s' in '%s' failed!", "Max", "quest_dungeon_mobs.xml");
            goto error;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "Authority", &Mob->Spawn.Authority)) {
            Error("Loading '%s' in '%s' failed!", "Authority", "quest_dungeon_mobs.xml");
            goto error;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "TrgIdxSpawn", &Mob->Spawn.SpawnTriggerID);
        ParseAttributeInt32(Archive, Iterator->Index, "TrgIdxKill", &Mob->Spawn.KillTriggerID);
        ParseAttributeInt32(Archive, Iterator->Index, "Server_Mob", &Mob->Spawn.ServerMobID);
        ParseAttributeInt32(Archive, Iterator->Index, "Loot_Delay", &Mob->Spawn.LootDelay);

        ParseAttributeInt32Array(Archive, Iterator->Index, "EvtProperty", Mob->Spawn.EventProperty, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
        ParseAttributeInt32Array(Archive, Iterator->Index, "EvtMobs", Mob->Spawn.EventMobs, RUNTIME_MOB_MAX_EVENT_COUNT, ',');
        ParseAttributeInt32Array(Archive, Iterator->Index, "EvtInterval", Mob->Spawn.EventInterval, RUNTIME_MOB_MAX_EVENT_COUNT, ',');

        Mob->SpeciesData = &Runtime->MobData[Mob->Spawn.MobSpeciesIndex];
        Mob->IsInfiniteSpawn = true;
        Mob->IsPermanentDeath = false;
        Mob->RemainingSpawnCount = 0;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveDestroy(TempArchive);
    return true;

error:
    for (Int32 Index = 0; Index < Runtime->MissionDungeonPatternPartDataCount; Index += 1) {
        ArrayDestroy(Runtime->MissionDungeonPatternPartData[Index].MobTable);
    }

    ArchiveDestroy(TempArchive);
    return false;
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
        
        Int32 DungeonIndex = -1;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "DungeonID", &DungeonIndex)) goto error;

        if (DungeonIndex > 0) {
            if (!World->HasQuestDungeon && !World->HasMissionDungeon) {
                Error("WorldDrop: World %d has no dungeon %d!", WorldIndex, DungeonIndex);
                Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
                continue;
            }

            Bool Found = false;
            for (Int32 Index = 0; Index < Runtime->DungeonDataCount; Index += 1) {
                RTDungeonDataRef DungeonData = &Runtime->DungeonData[Index];
                if (DungeonData->WorldID == WorldIndex && DungeonData->DungeonID == DungeonIndex) {
                    DropTable = &DungeonData->DropTable;
                    Found = true;
                }
            }

            if (!Found) {
                Error("WorldDrop: World %d has no dungeon %d!", WorldIndex, DungeonIndex);
                Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
                continue;
            }
        }

        Int32 MobSpeciesIndex = -1;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Terrain_Mob", &MobSpeciesIndex)) goto error;

        RTDropItemRef DropItem = NULL;
        if (MobSpeciesIndex > 0) {
            assert(DropTable->MobItemCount < RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT);
            RTMobDropItemRef MobDropItem = &DropTable->MobItems[DropTable->MobItemCount];
            MobDropItem->MobSpeciesIndex = MobSpeciesIndex;
            DropItem = &MobDropItem->Item;
            DropTable->MobItemCount += 1;
        }
        else {
            // TODO: Add Group property for drops
            assert(DropTable->WorldItemCount < RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT);
            RTWorldDropItemRef WorldDropItem = &DropTable->WorldItems[DropTable->WorldItemCount];
            if (!ParseAttributeInt32(Archive, Iterator->Index, "MinLv", &WorldDropItem->MinMobLevel)) goto error;
            if (!ParseAttributeInt32(Archive, Iterator->Index, "MaxLv", &WorldDropItem->MaxMobLevel)) goto error;
            DropItem = &WorldDropItem->Item;
            DropTable->WorldItemCount += 1;
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
