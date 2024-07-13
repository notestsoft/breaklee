#pragma once

#include "Base.h"
#include "Definitions.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTRuntimeDataContext {
#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__) \
	Int32 CONCAT(__NAME__, Count);									\
	struct CONCAT(_RTData, __NAME__) CONCAT(__NAME__, List)[__COUNT__];

#include "Macro.h"
};
typedef struct _RTRuntimeDataContext* RTRuntimeDataContextRef;

struct _RTDataForceCodeFormula {
	Int32 ForceEffectIndex;
	Int32 ForceLevel;
	Int32 ForceValue;
};
typedef struct _RTDataForceCodeFormula RTDataForceCodeFormula;

#pragma pack(pop)

RTRuntimeDataContextRef RTRuntimeDataContextCreate();

Void RTRuntimeDataContextDestroy(
	RTRuntimeDataContextRef Context
);

Bool RTRuntimeDataContextLoad(
	RTRuntimeDataContextRef Context,
	CString RuntimeDataPath,
	CString ServerDataPath
);

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__)	\
Bool CONCAT(RTRuntimeData, __NAME__ ## HotReload)(				\
	RTRuntimeDataContextRef Context,							\
    CString RuntimeDataPath,									\
	CString ServerDataPath,										\
	CString FileName											\
);
#include "Macro.h"

#define RUNTIME_DATA_TYPE_INDEX(__NAME__, __TYPE__, __FIELD__)	        \
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData, __NAME__ ## Get)( \
	RTRuntimeDataContextRef Context,							        \
	__TYPE__ __FIELD__											        \
);

#define RUNTIME_DATA_TYPE_INDEX_SUFFIXED(__NAME__, __SUFFIX__, __TYPE__, __FIELD__)	  \
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData ## __NAME__, Get ## __SUFFIX__)( \
	RTRuntimeDataContextRef Context,												  \
	__TYPE__ __FIELD__																  \
);

#define RUNTIME_DATA_TYPE_INDEX_CHILD(__PARENT__, __NAME__, __TYPE__, __FIELD__) \
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData, __NAME__ ## Get)(			 \
	CONCAT(RTData, __PARENT__ ## Ref) Parent,								     \
	__TYPE__ __FIELD__															 \
);

#define RUNTIME_DATA_TYPE_INDEX_SINGLE(__NAME__)				        \
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData, __NAME__ ## Get)( \
	RTRuntimeDataContextRef Context								        \
);

#define RUNTIME_DATA_TYPE_INDEX_RANGE(__NAME__, __TYPE__, __LOWER_FIELD__, __UPPER_FIELD__) \
CONCAT(RTData, __NAME__ ## Ref) CONCAT(RTRuntimeData, __NAME__ ## Get)(						\
	RTRuntimeDataContextRef Context,														\
	__TYPE__ Value																			\
);

#include "Macro.h"

RTDataAnimaMasteryValueRef RTRuntimeDataAnimaMasteryValueGet(
	RTRuntimeDataContextRef Context,
	Int32 CategoryIndex,
	Int32 ForceEffectOrder
);

Int32 RTRuntimeDataCharacterRankUpConditionGet(
	RTRuntimeDataContextRef Context,
	Int32 Rank,
	Int32 BattleStyleIndex
);

RTDataUpgradeItemGradeRef RTRuntimeDataUpgradeItemGradeGet(
	RTDataUpgradeItemRef UpgradeItem,
	Int32 Grade
);

RTDataUpgradeItemBasicGradeRef RTRuntimeDataUpgradeItemBasicGradeGet(
	RTDataUpgradeItemBasicRef UpgradeItem,
	Int32 Grade
);

RTDataNewbieSupportCategoryRewardRef RTRuntimeDataNewbieSupportCategoryRewardGet(
	RTDataNewbieSupportCategoryRef Category,
	Int32 ConditionValue1,
	Int32 ConditionValue2
);

RTDataDivineUpgradeMainRef RTRuntimeDataDivineUpgradeMainGet(
	RTRuntimeDataContextRef Context,
	Int32 ItemGrade,
	Int32 ItemType
);

RTDataDummySkillRef RTRuntimeDataDummySkillGet(
	RTRuntimeDataContextRef Context,
	Int32 Case,
	Int32 Property
);

RTDataForceCoreBaseRef RTRuntimeDataForceCoreBaseGet(
	RTRuntimeDataContextRef Context,
	Int32 ItemGrade,
	Int32 ItemType
);

RTDataForceCodeCostRef RTRuntimeDataForceCodeCostGet(
	RTRuntimeDataContextRef Context,
	Int32 CostGrade,
	Int32 FilledSlotCount
);

RTDataForceCodeRateRef RTRuntimeDataForceCodeRateGet(
	RTRuntimeDataContextRef Context,
	Int32 ItemType,
	Int32 FilledSlotCount,
	Int32 ItemUpgradeLevel
);

RTDataUpgradeGradeChangeRef RTRuntimeDataUpgradeGradeChangeGet(
	RTRuntimeDataContextRef Context,
	Int32 ItemType,
	Int32 ItemGrade
);

RTDataForceCodeFormula RTRuntimeDataForceCodeFormulaGet(
	RTRuntimeDataContextRef Context,
	RTDataItemType ItemType,
	Int32 ItemGrade,
	Int32 ItemUniqueGrade,
	Int32 ForceIndex,
	Int32 ForceLevel,
	Bool IsExtended,
	Bool IsEpic
);

RTDataShopIndexRef RTRuntimeDataShopIndexGet(
	RTRuntimeDataContextRef Context,
	Int32 WorldIndex,
	Int32 NpcIndex
);

RTDataShopItemRef RTRuntimeDataShopItemGet(
	RTDataShopPoolRef ShopPool,
	Int32 TabIndex,
	Int32 SlotIndex
);

EXTERN_C_END
