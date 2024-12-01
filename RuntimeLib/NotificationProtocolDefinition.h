#ifndef NOTIFICATION_PROTOCOL_ENUM
#define NOTIFICATION_PROTOCOL_ENUM(...)
#endif

#ifndef NOTIFICATION_PROTOCOL_STRUCT
#define NOTIFICATION_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef NOTIFICATION_PROTOCOL
#define NOTIFICATION_PROTOCOL(__NAME__, __COMMAND__, __BODY__)
#endif

// 1 - Dead not revivable
// 5 - Dead revivable
// 7 - Pause game for 5 sec
// 8 - Disconnected error
// 9 - Disconnected error
// 12 - The item doesnt exist
// 13 - The dungeon is full
// 14 - Skill activation not enough mp
// 15 - Dead revivable
// 17 - Not enough sp to switch to aura mode
// 19 - Server maintenance
// 20 - Time limit system cannot start the dungeon
// 21 - Mission war lobby closed
// 22 - Insufficient mission battle alz for fee
// 23 - Mission battle conditions not met
// 24 - Mission battle closed
// 27 - Channel is only for pc safe
// 28 - You cannot receive reward for now
// 31 - Warping character to other position
// 32 - Able to enter dungeon if characters are in same party
// 33 - Unable to enter dungeon in trade channel (force closing windows)
// 34 - Unable to enter dungeon (force closing windows)
// 35 - Skill cooldown
// 36 - Vital gear only in nation war
// 38 - Open select server screen
// 44 - Force disconnect client
// 45 - Cannot use warp in pvp
// 47 - Force disconnect client
// 49 - Unable to enter dungeon
// 64 - Insufficient sp
// 65 - Only for 2FA users
// 69 - Rankings are being calculated | entry will open at midnight
// 70 - Insufficient sp for battle mode
// 71 - Gps warp only for premium (opens premium shop dialog)
// 80 - Force close client
// 82 - Cannot enter dungeon due to force wing level
// 83 - Cannot enter dungeon due to force wing level
// 86 - Cannot use skill while silence or other
// 88 - Failed to enter war due to combat power
// 89 - Only bringer can use this skill
// 90 - Only guardians can use this skill
// 91 - Skill only in ingres proelium

NOTIFICATION_PROTOCOL(ERROR_CODE, 7,
    UInt16 ErrorCommand;
    UInt16 ErrorSubCommand;
    UInt16 ErrorCode;
    UInt16 Unknown1;
)

NOTIFICATION_PROTOCOL_ENUM(
    NOTIFICATION_SPAWN_TYPE_LIST = 0,
    NOTIFICATION_SPAWN_TYPE_INIT = 0x30,
    NOTIFICATION_SPAWN_TYPE_WARP,
    NOTIFICATION_SPAWN_TYPE_MOVE,
    NOTIFICATION_SPAWN_TYPE_RESURRECT
)

NOTIFICATION_PROTOCOL_ENUM(
    NOTIFICATION_DESPAWN_TYPE_DEAD = 0x10,
    NOTIFICATION_DESPAWN_TYPE_WARP,
    NOTIFICATION_DESPAWN_TYPE_INIT,
    NOTIFICATION_DESPAWN_TYPE_RETURN,
    NOTIFICATION_DESPAWN_TYPE_DISAPPEAR,
    NOTIFICATION_DESPAWN_TYPE_NOTIFY_DEAD
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_GUILD,
    UInt8 GuildNameLength;
    Char GuildName[0]; // GuildNameLength
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_EQUIPMENT_SLOT,
    UInt8 EquipmentSlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 ItemDuration;
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_PERSONAL_SHOP_MESSAGE,
    UInt8 MessageLength;
    Char Message[0];
    // UInt64 Unknown1;
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_PERSONAL_SHOP_INFO,
    UInt64 Unknown1;
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_BUFF_SLOT,
    UInt16 SkillIndex;
    UInt8 SkillLevel;
    UInt8 Unknown1[82];
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_PET_SLOT,
    struct {
        UInt8 EvolutionLevel : 3;
        UInt8 NameLength : 5;
    } Info;
    UInt32 PetSlotIndex;
    Char PetName[0];
    // UInt32 Unknown1;
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_BLESSING_BEAD_BALOON_SLOT,
    UInt8 Unknown1;
    UInt32 Unknown2;
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_BALOON_SLOT,
    RTItem Item;
    UInt8 SlotIndex;
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_BFX_SLOT,
    UInt8 BfxIndex;
    UInt32 Duration;
)

NOTIFICATION_PROTOCOL_STRUCT(CHARACTERS_SPAWN_INDEX,
    UInt32 CharacterIndex;
    RTEntityID Entity;
    UInt32 Level;
    UInt16 OverlordLevel;
    UInt32 HolyPower;
    UInt32 Rebirth;
    UInt32 MythLevel;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    UInt64 MaxHP;
    UInt64 CurrentHP;
    UInt64 CurrentShield;
    Int32 MaxRage;
    Int32 CurrentRage;
    UInt32 MovementSpeed;
    UInt16 PositionBeginX;
    UInt16 PositionBeginY;
    UInt16 PositionEndX;
    UInt16 PositionEndY;
    UInt16 PKState;
    UInt8 Nation;
    UInt32 Unknown3;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    Int32 ActiveAuraSkillIndex;
    UInt8 CharacterExtendedStyle;
    UInt8 IsDead;
    UInt8 EquipmentSlotCount;
    UInt8 CostumeSlotCount;
    UInt8 IsInvisible;
    UInt8 IsPersonalShop;
    UInt32 GuildIndex;
    Int32 GuildColor;
    struct {
        UInt8 Unknown8 : 1;
        UInt8 Unknown9 : 1;
        UInt8 Unknown10 : 1;
        UInt8 HasPetInfo : 1;
        UInt8 PetCount : 2;
        UInt8 HasBlessingBeadBaloon : 1;
        UInt8 HasItemBallon : 1;
    };
    Int8 SkillBuffCount;
    Int8 GmBuffCount;
    Int8 ForceCaliburBuffCount;
    Int8 FirePlaceBuffCount;
    Int32 BfxCount;
    UInt8 IsBringer;
    UInt32 EventNameColor;
    UInt16 DisplayTitle;
    UInt16 EventTitle;
    UInt16 WarTitle;
    UInt16 AbilityTitle;
    UInt8 Unknown16[17];
    UInt8 NameLength;
    Char Name[0]; // Size: NameLength - 1
    // S2C_DATA_CHARACTERS_SPAWN_GUILD Guild;
    // S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT EquipmentSlots[EquipmentSlotCount];
    // S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT CostumeSlots[CostumeSlotCount];
    // S2C_DATA_CHARACTERS_SPAWN_PERSONAL_SHOP_MESSAGE PersonalShopMessage[IsPersonalShop];
    // S2C_DATA_CHARACTERS_SPAWN_PERSONAL_SHOP_INFO PersonalShopInfo[IsPersonalShop];
    // S2C_DATA_CHARACTERS_SPAWN_BUFF_SLOT Buffs[ActiveBuffCount + DebuffCount + GmBuffCount + PassiveBuffCount];
    // S2C_DATA_CHARACTERS_SPAWN_PET_SLOT Pets[PetCount];
    // S2C_DATA_CHARACTERS_SPAWN_BALOON_SLOT ItemBaloon[HasItemBaloon];
)

NOTIFICATION_PROTOCOL_ENUM(
    OBJECT_STATUS_NORMAL = 0,
    OBJECT_STATUS_BUILDING = 1,
    OBJECT_STATUS_REPAIRING = 2,
    OBJECT_STATUS_DELETED = 3,
)

NOTIFICATION_PROTOCOL_ENUM(
    OBJECT_TYPE_WARP = 1,
    OBJECT_TYPE_RELIVE = 2,
    OBJECT_TYPE_ATTACK = 3,
    OBJECT_TYPE_BUFF = 4,
    OBJECT_TYPE_DEBUFF = 5,
    OBJECT_TYPE_MOB = 6,
)

NOTIFICATION_PROTOCOL_STRUCT(OBJECTS_SPAWN_INDEX,
    RTEntityID Entity;
    UInt8 ObjectType;
    UInt16 EntitySourceIndex;
    UInt8 PositionX;
    UInt8 PositionY;
    UInt64 CurrentHP;
    UInt64 MaxHP;
    UInt8 Nation;
    UInt8 Status;
    UInt32 CharacterIndex;
    UInt8 NpcIndex;
)

NOTIFICATION_PROTOCOL(OBJECTS_SPAWN, 198,
    UInt8 Count;
    NOTIFICATION_DATA_OBJECTS_SPAWN_INDEX Objects[0];
)

NOTIFICATION_PROTOCOL_ENUM(
    OBJECT_DESPAWN_TYPE_CANCEL = 1,
    OBJECT_DESPAWN_TYPE_DESTROY = 2,
    OBJECT_DESPAWN_TYPE_TIMEOUT = 3,
)

NOTIFICATION_PROTOCOL(OBJECTS_DESPAWN, 199,
    RTEntityID Entity;
    UInt8 DespawnType;
)

NOTIFICATION_PROTOCOL(CHARACTERS_SPAWN, 200,
    UInt8 Count;
    UInt8 SpawnType;
    //S2C_DATA_CHARACTERS_SPAWN_INDEX Data[0];
)

NOTIFICATION_PROTOCOL(CHARACTER_DESPAWN, 201,
    UInt32 CharacterIndex;
    UInt8 DespawnType;
)

NOTIFICATION_PROTOCOL_STRUCT(MOBS_SPAWN_INDEX,
    RTEntityID Entity;
    UInt16 PositionBeginX;
    UInt16 PositionBeginY;
    UInt16 PositionEndX;
    UInt16 PositionEndY;
    UInt16 MobSpeciesIndex;
    UInt64 MaxHP;
    UInt64 CurrentHP;
    UInt8 IsChasing;
    UInt8 Level;
    UInt8 Nation;
    UInt8 Unknown1;
    UInt32 Unknown2;
    UInt16 MobEffectIndex;
    UInt32 SpecialAnimationIndex;
    UInt8 Unknown3;
    UInt16 ServerMobBalloonIndex;
    UInt32 Unknown4;
    UInt32 MaybeAttackTimeFrame;
    UInt32 AttackerIndex;
    UInt8 Unknown5[22];
)

NOTIFICATION_PROTOCOL(MOBS_SPAWN, 202,
    UInt8 Count;
    NOTIFICATION_DATA_MOBS_SPAWN_INDEX Mobs[0];
)

NOTIFICATION_PROTOCOL(MOBS_DESPAWN, 203,
    RTEntityID Entity;
    UInt8 DespawnType;
)

NOTIFICATION_PROTOCOL_ENUM(
    NOTIFICATION_ITEMS_SPAWN_CONTEXT_TYPE_NONE = 0,
    NOTIFICATION_ITEMS_SPAWN_CONTEXT_TYPE_MOBS = 1,
    NOTIFICATION_ITEMS_SPAWN_CONTEXT_TYPE_USER = 2,
)

NOTIFICATION_PROTOCOL_STRUCT(ITEMS_SPAWN_INDEX,
    RTEntityID Entity;
    UInt64 ItemOptions;
    UInt32 Unknown1;
    UInt32 SourceIndex;
    UInt64 ItemID;
    UInt16 X;
    UInt16 Y;
    UInt16 UniqueKey;
    UInt8 ContextType;
    RTItemProperty ItemProperty;
)

NOTIFICATION_PROTOCOL(ITEMS_SPAWN, 204,
    UInt8 Count;
    NOTIFICATION_DATA_ITEMS_SPAWN_INDEX Items[0];
)

NOTIFICATION_PROTOCOL(ITEMS_DESPAWN, 205,
    RTEntityID Entity;
    UInt8 DespawnType;
)

NOTIFICATION_PROTOCOL(CHARACTER_ITEM_EQUIP, 206,
    UInt32 CharacterIndex;
    RTItem Item;
    UInt64 ItemOptions;
    UInt16 EquipmentSlotIndex;
    UInt32 Unknown1;
    UInt8 Unknown2;
)

NOTIFICATION_PROTOCOL(CHARACTER_ITEM_UNEQUIP, 207,
    UInt32 CharacterIndex;
    UInt16 EquipmentSlotIndex;
)

NOTIFICATION_PROTOCOL_STRUCT(MOBS_DESPAWN_LIST_INDEX,
    RTEntityID EntityID;    
)

NOTIFICATION_PROTOCOL(MOBS_DESPAWN_LIST, 208,
    UInt8 Count;
    // MOBS_DESPAWN_LIST_INDEX Mobs[Count];
)

NOTIFICATION_PROTOCOL(MOB_MOVE_BEGIN, 213,
    RTEntityID Entity;
    UInt32 TickCount;
    UInt16 PositionBeginX;
    UInt16 PositionBeginY;
    UInt16 PositionEndX;
    UInt16 PositionEndY;
)

NOTIFICATION_PROTOCOL(MOB_MOVE_END, 214,
    RTEntityID Entity;
    UInt16 PositionCurrentX;
    UInt16 PositionCurrentY;
)

NOTIFICATION_PROTOCOL(MOB_CHASE_BEGIN, 215,
    RTEntityID Entity;
    UInt32 TickCount;
    UInt16 PositionBeginX;
    UInt16 PositionBeginY;
    UInt16 PositionEndX;
    UInt16 PositionEndY;
)

NOTIFICATION_PROTOCOL(MOB_CHASE_END, 216,
    RTEntityID Entity;
    UInt16 PositionCurrentX;
    UInt16 PositionCurrentY;
)

NOTIFICATION_PROTOCOL_STRUCT(SKILL_TO_CHARACTER_BATTLE_MODE,
    UInt32 CharacterIndex;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt8 CharacterExtendedStyle;
    UInt16 IsActivation;
)

NOTIFICATION_PROTOCOL_STRUCT(SKILL_TO_CHARACTER_ASTRAL_MODE,
    UInt32 CharacterIndex;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt8 CharacterExtendedStyle;
    UInt16 IsActivation;
    UInt32 Unknown2;
)

NOTIFICATION_PROTOCOL(SKILL_TO_CHARACTER, 221,
    UInt16 SkillIndex;
)

NOTIFICATION_PROTOCOL(ATTACK_TO_MOB, 225,
    UInt8 AttackType;
    UInt32 CharacterIndex;
    RTEntityID Mob;
    UInt8 MobIDType;
    UInt64 MobHP;
    UInt8 Unknown1;
    UInt64 CharacterHP;
    UInt32 Unknown2;
)

NOTIFICATION_PROTOCOL(CHARACTER_BATTLE_RANK_UP, 276,
    UInt8 Level;
)

NOTIFICATION_PROTOCOL_ENUM(
    NOTIFICATION_CHARACTER_EVENT_TYPE_LEVEL_UP = 1,
    NOTIFICATION_CHARACTER_EVENT_TYPE_RANK_UP = 2,
    NOTIFICATION_CHARACTER_EVENT_TYPE_OVERLORD_LEVEL_UP = 3,
    NOTIFICATION_CHARACTER_EVENT_TYPE_UNKNOWN = 4,
    NOTIFICATION_CHARACTER_EVENT_TYPE_FORCE_WING_GRADE_UP = 6,
    NOTIFICATION_CHARACTER_EVENT_TYPE_FORCE_WING_LEVEL_UP = 7,
)

NOTIFICATION_PROTOCOL_ENUM(
    NOTIFICATION_CHARACTER_DATA_TYPE_HPPOTION           = 1,
    NOTIFICATION_CHARACTER_DATA_TYPE_MPPOTION           = 2,
    NOTIFICATION_CHARACTER_DATA_TYPE_HP                 = 3,
    NOTIFICATION_CHARACTER_DATA_TYPE_MP                 = 4,
    NOTIFICATION_CHARACTER_DATA_TYPE_SP                 = 5,
    NOTIFICATION_CHARACTER_DATA_TYPE_SP_INCREASE        = 6,
    NOTIFICATION_CHARACTER_DATA_TYPE_SP_DECREASE        = 7,
    NOTIFICATION_CHARACTER_DATA_TYPE_EXP                = 8,
    NOTIFICATION_CHARACTER_DATA_TYPE_RANK               = 9,
    NOTIFICATION_CHARACTER_DATA_TYPE_LEVEL              = 10,
    NOTIFICATION_CHARACTER_DATA_TYPE_SP_DECREASE_EX     = 11,
    NOTIFICATION_CHARACTER_DATA_TYPE_BUFF_POTION        = 12,
    NOTIFICATION_CHARACTER_DATA_TYPE_REPUTATION         = 13,
    NOTIFICATION_CHARACTER_DATA_TYPE_GUIDITEMFX         = 14,
    NOTIFICATION_CHARACTER_DATA_TYPE_RESURRECTION       = 15,
    NOTIFICATION_CHARACTER_DATA_TYPE_PENALTY_EXP        = 16,
    NOTIFICATION_CHARACTER_DATA_TYPE_DAMAGE_CELL        = 17,
    NOTIFICATION_CHARACTER_DATA_TYPE_DEFFICIENCY        = 18,
    NOTIFICATION_CHARACTER_DATA_TYPE_AUTH_HP_POTION     = 19,
    NOTIFICATION_CHARACTER_DATA_TYPE_RAGE               = 20,
    NOTIFICATION_CHARACTER_DATA_TYPE_MAX_RAGE           = 21,
    NOTIFICATION_CHARACTER_DATA_TYPE_OVERLORD_LEVEL     = 22,
    NOTIFICATION_CHARACTER_DATA_TYPE_HONOR_MEDAL        = 23,
    NOTIFICATION_CHARACTER_DATA_TYPE_BP                 = 24,
    NOTIFICATION_CHARACTER_DATA_TYPE_BP_POTION          = 25,
    NOTIFICATION_CHARACTER_DATA_TYPE_AWAKEN_AURA_POINT  = 26,
    NOTIFICATION_CHARACTER_DATA_TYPE_AWAKEN_AURA_TIME   = 27,
    NOTIFICATION_CHARACTER_DATA_TYPE_EVENT              = 28,
)

NOTIFICATION_PROTOCOL(CHARACTER_DATA, 287, 
    UInt8 Type;

    union {
        struct { UInt8 _0[34]; UInt64 HPAfterPotion; UInt8 _1[4]; };
        struct { UInt8 _2[38]; UInt64 HP; };
        struct { UInt8 _3[34]; UInt32 MP; UInt8 _4[8]; };
        struct { UInt8 _5[34]; UInt32 SP; UInt8 _6[8]; };
        struct { UInt8 _7[26]; UInt32 MapCode; UInt32 WarpCode; UInt32 Level; UInt8 _8[8]; };
        struct { UInt8 _9[34]; UInt32 Rage; UInt8 _10[8]; };
        struct { UInt8 _11[34]; UInt32 BP; UInt8 _12[8]; };
        struct { UInt8 _13[34]; UInt32 BuffResult; UInt8 _14[8]; };
        struct { UInt8 _15[34]; UInt32 SkillRank; UInt8 _16[8]; };
        struct { UInt8 _17[22]; UInt32 HonorMedalGrade; UInt8 _18[8]; Int64 HonorPoints; UInt8 _19[4]; };
    };
)

NOTIFICATION_PROTOCOL(CHARACTER_EVENT, 288,
    UInt8 Type;
    UInt32 CharacterIndex;
)

NOTIFICATION_PROTOCOL(DUNGEON_PATTERN_PART_COMPLETED, 303,
    UInt8 PatternPartIndex;
)

NOTIFICATION_PROTOCOL(PARTY_QUEST_ACTION, 377,
    UInt8 QuestSlotIndex;
    UInt16 QuestID;
    UInt16 NpcFlags;
    UInt32 NpcSetID;
) 

NOTIFICATION_PROTOCOL(PARTY_QUEST_LOOT_ITEM, 378,
    UInt64 ItemID;
    UInt64 ItemOptions;
    Int16 QuestItemSlotIndex;
    UInt32 ItemDuration;
)

NOTIFICATION_PROTOCOL(PARTY_QUEST_MISSION_MOB_KILL, 379,
    UInt16 QuestID;
    UInt16 MobSpeciesIndex;
)

NOTIFICATION_PROTOCOL(MOB_SPECIAL_BUFF, 390,
    RTEntityID MobID;
    UInt64 CurrentHP;
    UInt32 ReceivedDamage;
)

NOTIFICATION_PROTOCOL_STRUCT(CREATE_ITEM_SLOT,
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 SlotIndex;
    Int32 ItemDuration;
)

NOTIFICATION_PROTOCOL(CREATE_ITEM, 414,
    UInt8 ItemType;
    UInt8 ItemCount;
    NOTIFICATION_DATA_CREATE_ITEM_SLOT Items[0];
)

NOTIFICATION_PROTOCOL_STRUCT(MOB_ATTACK_AOE_TARGET,
    UInt32 CharacterIndex;
    Bool IsDead;
    UInt8 Result;
    UInt32 AppliedDamage;
    UInt64 TargetHP;
    UInt32 TargetShield;
    UInt8 Unknown1[29];
)

NOTIFICATION_PROTOCOL(MOB_ATTACK_AOE, 413,
    RTEntityID Entity;
    Bool IsDefaultSkill;
    UInt8 Unknown1;
    UInt64 MobHP;
    UInt32 Unknown2;
    UInt16 TargetCount;
    NOTIFICATION_DATA_MOB_ATTACK_AOE_TARGET Data[0];
)

NOTIFICATION_PROTOCOL(CHANGE_GENDER, 488,
    UInt8 Gender;
    UInt8 Success;
)

NOTIFICATION_PROTOCOL(CHARACTER_SKILL_MASTERY_UPDATE, 760,
    UInt32 SkillRank;
    UInt32 SkillLevel;
    UInt32 SkillLevelMax;
    UInt32 SkillExp;
    UInt32 SkillPoint;
)

NOTIFICATION_PROTOCOL_STRUCT(MOB_PATTERN_SPECIAL_ACTION_MOB,
    RTEntityID MobID;
    Int32 ActionIndex;
    UInt32 ActionElapsedTime;
)

NOTIFICATION_PROTOCOL(MOB_PATTERN_SPECIAL_ACTION, 930,
    Int32 Count;
    // MOB_PATTERN_SPECIAL_ACTION_MOB Mobs[0];
)

NOTIFICATION_PROTOCOL(MOB_PATTERN_WARP_TARGET, 933,
    UInt32 SourceIndex;
    UInt32 TargetIndex;
    UInt16 PositionX;
    UInt16 PositionY;
)

NOTIFICATION_PROTOCOL_STRUCT(BUFF_BY_OBJECT_TARGET,
    Int32 Result;
    UInt32 TargetIndex;
    UInt8 TargetEntityType;
)

NOTIFICATION_PROTOCOL(BUFF_BY_OBJECT, 935,
    UInt32 SourceIndex;
    UInt8 SourceEntityType;
    Int32 SkillIndex;
    UInt8 TargetCount;
)

NOTIFICATION_PROTOCOL_STRUCT(MOB_PATTERN_ATTACK_TARGET,
    Int32 TargetEntityType;
    UInt32 TargetIndex;
    UInt8 IsDead;
    UInt8 AttackType;
    Int32 AppliedDamage;
    Int64 CurrentHP;
    Int64 CurrentShield;
    Int32 Unknown1;
    Int32 AdditionalDamage;
    Int32 BfxIndex;
    UInt32 BfxDuration;
    UInt8 Data[0];
)

NOTIFICATION_PROTOCOL(MOB_PATTERN_ATTACK, 938,
    UInt32 SourceIndex;
    Int32 AttackIndex;
    Int32 X;
    Int32 Y;
    UInt8 ReflectResult;
    Int64 CurrentHP;
    Int64 ReflectDamage;
    Int64 Unknown3;
    Int16 TargetCount;
)

NOTIFICATION_PROTOCOL_STRUCT(MOB_DESPAWN_BY_LINK_MOB_INDEX,
    RTEntityID MobID;
    Int16 Reason;
)

NOTIFICATION_PROTOCOL(MOBS_DESPAWN_BY_LINK_MOB, 998,
    RTEntityID SourceID;
    Int16 Count;
    // MOB_DESPAWN_BY_LINK_MOB_INDEX Mobs[0];
)

NOTIFICATION_PROTOCOL(REMOVE_BUFF, 2307,
    Int32 EntityType;
    UInt32 EntityID;
    Int32 BuffType;
    Int32 BuffSlotIndex;
    Int32 SkillIndex;
)

NOTIFICATION_PROTOCOL(DUNGEON_TIME_CONTROL, 2074,
    Int32 TimeAddition;
)

NOTIFICATION_PROTOCOL(CHARACTER_STATUS, 2110,
    Int64 CurrentHP;
    Int32 CurrentMP;
    Int64 CurrentShield;
    struct _RTCharacterBattleModeInfo BattleModeInfo;
    struct _RTBuffInfo BuffInfo;
    struct _RTBuffSlot BuffSlots[0];
)

NOTIFICATION_PROTOCOL(DUNGEON_TIMER, 2542,
    UInt32 TimerType;
    UInt32 Interval;
)

NOTIFICATION_PROTOCOL_ENUM(
    DUNGEON_TIMER_INFO_TYPE_DROP = 0,
    DUNGEON_TIMER_INFO_TYPE_MOB = 1,
)

NOTIFICATION_PROTOCOL(DUNGEON_TIMER_INFO, 2552,
    UInt8 Type;
    union {
        struct { UInt64 ItemID; UInt16 ItemCount; } Drop;
        struct { UInt32 EntityID; UInt64 CurrentHP; UInt64 MaxHP; } Mob;
    };
)

NOTIFICATION_PROTOCOL(CHARACTER_FORCE_WING_GRADE, 2788,
    UInt32 CharacterIndex;
    UInt8 ForceWingGrade;
)

NOTIFICATION_PROTOCOL(CHARACTER_FORCE_WING_UPDATE, 2789,
    UInt8 Status;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    Int64 ForceWingExp;
    Int64 Unknown1;
    UInt32 TrainingPointCount;
)

NOTIFICATION_PROTOCOL(CHARACTER_FORCE_WING_EXP, 2791,
    Int64 ForceWingExp;
)

#undef NOTIFICATION_PROTOCOL_ENUM
#undef NOTIFICATION_PROTOCOL_STRUCT
#undef NOTIFICATION_PROTOCOL
