#ifndef RUNTIME_DATA_FILE_BEGIN
#define RUNTIME_DATA_FILE_BEGIN(__NAME__)
#endif 

#ifndef RUNTIME_DATA_FILE_END
#define RUNTIME_DATA_FILE_END
#endif 

#ifndef RUNTIME_DATA_TYPE_BEGIN
#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__)
#endif

#ifndef RUNTIME_DATA_TYPE_BEGIN_CHILD
#define RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__, __COUNT__)
#endif

#ifndef RUNTIME_DATA_PROPERTY_PRECONDITION
#define RUNTIME_DATA_PROPERTY_PRECONDITION(__TYPE__, __NAME__, __QUERY__, __VALUE__)
#endif

#ifndef RUNTIME_DATA_PROPERTY
#define RUNTIME_DATA_PROPERTY(__TYPE__, __NAME__, __QUERY__)
#endif

#ifndef RUNTIME_DATA_PROPERTY_ARRAY
#define RUNTIME_DATA_PROPERTY_ARRAY(__TYPE__, __NAME__, __QUERY__, __COUNT__, __SEPARATOR__)
#endif

#ifndef RUNTIME_DATA_TYPE_END_CHILD
#define RUNTIME_DATA_TYPE_END_CHILD(__NAME__, __COUNT__)
#endif

#ifndef RUNTIME_DATA_TYPE_END
#define RUNTIME_DATA_TYPE_END(__NAME__)
#endif

#ifndef RUNTIME_DATA_TYPE_INDEX
#define RUNTIME_DATA_TYPE_INDEX(__NAME__, __TYPE__, __FIELD__)
#endif

#ifndef RUNTIME_DATA_TYPE_INDEX_SUFFIXED
#define RUNTIME_DATA_TYPE_INDEX_SUFFIXED(__NAME__, __SUFFIX__, __TYPE__, __FIELD__)
#endif

#ifndef RUNTIME_DATA_TYPE_INDEX_CHILD
#define RUNTIME_DATA_TYPE_INDEX_CHILD(__PARENT__, __NAME__, __TYPE__, __FIELD__)
#endif

#ifndef RUNTIME_DATA_TYPE_INDEX_SINGLE
#define RUNTIME_DATA_TYPE_INDEX_SINGLE(__NAME__)
#endif

#ifndef RUNTIME_DATA_TYPE_INDEX_RANGE
#define RUNTIME_DATA_TYPE_INDEX_RANGE(__NAME__, __TYPE__, __LOWER_FIELD__, __UPPER_FIELD__)
#endif

#include "Ability.h"
#include "Achievement.h"
#include "AnimaMastery.h"
#include "Assistant.h"
#include "Avatar.h"
#include "AwakenAuraMode.h"
#include "BattleStyleChange.h"
#include "BelongItem.h"
#include "BlessingBeadCustomizing.h"
#include "BossArena.h"
#include "BossResistList.h"
#include "CashShop.h"
#include "ChallengeDungeon.h"
#include "ChallengeMissionData.h"
#include "ChangeItem.h"
#include "ChangeShape.h"
#include "ChaosUpgrade.h"
#include "Character.h"
#include "ChatBaloon.h"
#include "CheckItemAbility.h"
#include "Collection.h"
#include "Const.h"
#include "CostumeWarehouse.h"
#include "Craft.h"
#include "Destroy.h"
#include "DivineUpgrade.h"
#include "DropEffect.h"
#include "DungeonTimeLimit.h"
#include "Effector.h"
#include "Enchant.h"
#include "EnvironmentSettings.h"
#include "EpicBoost.h"
#include "Event.h"
#include "ExtraObject.h"
#include "ExtremeUpgrade.h"
#include "FieldBossRaid.h"
#include "FieldWar.h"
#include "ForceCoreOption.h"
#include "ForceWing.h"
#include "Giftbox.h"
#include "GuildDungeon.h"
#include "GuildTreasure.h"
#include "HonorMedal.h"
#include "Item.h"
#include "Keymap.h"
#include "Level.h"
#include "Lottery.h"
#include "MapInfo.h"
#include "Market.h"
#include "Meister.h"
#include "MeritSystem.h"
#include "MissionBattle.h"
#include "MissionDungeon.h"
#include "Mob.h"
#include "MobSkill.h"
#include "Myth.h"
#include "NewbieSupport.h"
#include "OverlordMastery.h"
#include "Pet.h"
#include "PremiumServiceList.h"
#include "PVPBattle.h"
#include "Quest.h"
#include "QuestDungeon.h"
#include "RandomWarp.h"
#include "Rank.h"
#include "Request.h"
#include "SecretShop.h"
#include "SetEffect.h"
#include "Shop.h"
#include "Skill.h"
#include "SkillEnchant.h"
#include "Stellar.h"
#include "Title.h"
#include "Transform.h"
#include "WarAdvantage.h"
#include "Warp.h"

#undef RUNTIME_DATA_FILE_BEGIN
#undef RUNTIME_DATA_FILE_END
#undef RUNTIME_DATA_TYPE_BEGIN
#undef RUNTIME_DATA_TYPE_BEGIN_CHILD
#undef RUNTIME_DATA_PROPERTY_PRECONDITION
#undef RUNTIME_DATA_PROPERTY
#undef RUNTIME_DATA_PROPERTY_ARRAY
#undef RUNTIME_DATA_TYPE_END_CHILD
#undef RUNTIME_DATA_TYPE_END
#undef RUNTIME_DATA_TYPE_INDEX
#undef RUNTIME_DATA_TYPE_INDEX_SUFFIXED
#undef RUNTIME_DATA_TYPE_INDEX_CHILD
#undef RUNTIME_DATA_TYPE_INDEX_SINGLE
#undef RUNTIME_DATA_TYPE_INDEX_RANGE
