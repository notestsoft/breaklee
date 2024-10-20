#pragma once

#include "Base.h"
#include "Definitions.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTRuntimeDataContext {
	AllocatorRef Allocator;
	ArrayRef FileEvents;
	Char RuntimeDataPath[MAX_PATH];
	Char ServerDataPath[MAX_PATH];

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__) \
	CString CONCAT(__NAME__, FileName);
#include "Macro.h"

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__) \
	Int32 CONCAT(__NAME__, Count);					 \
	struct CONCAT(_RTData, __NAME__)* CONCAT(__NAME__, List);
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

RTRuntimeDataContextRef RTRuntimeDataContextCreate(
	CString RuntimeDataPath,
	CString ServerDataPath,
	Bool* Result
);

Void RTRuntimeDataContextDestroy(
	RTRuntimeDataContextRef Context
);

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__)	\
Bool CONCAT(RTRuntimeData, __NAME__ ## HotReload)(		\
	RTRuntimeDataContextRef Context,					\
	CString FileName									\
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

RTDataMarketCategoryMainGroupRef RTRuntimeDataMarketCategoryMainGroupGetByCategory2(
	RTRuntimeDataContextRef Context,
	Int32 Category2
);

RTDataMarketListItemRef RTRuntimeDataMarketListItemGet(
	RTRuntimeDataContextRef Context,
	UInt64 ItemID,
	UInt64 ItemOptions
);

EXTERN_C_END
