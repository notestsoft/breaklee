#pragma once

#include "Base.h"
#include "Constants.h"
#include "Timer.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTRequestCraftSlot {
    UInt8 SlotIndex;
    UInt32 RequestCode;
    Timestamp Timestamp;
    UInt8 Result;
};

struct _RTRequestCraftInventorySlot {
    Int32 InventorySlotIndex;
    Int32 Count;
};

struct _RTRequestCraftInfo {
    UInt8 SlotCount;
    UInt32 Exp;
    UInt8 RegisteredFlags[RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_RECIPE_COUNT];
    UInt32 SortingOrder;
    UInt8 FavoriteFlags[RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_RECIPE_COUNT];
};

struct _RTCharacterRequestCraftInfo {
    struct _RTRequestCraftInfo Info;
    struct _RTRequestCraftSlot Slots[RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_SLOT_COUNT];
};

#pragma pack(pop)

Int RTCharacterGetRequestCraftLevel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterIsRequestCraftRecipeRegistered(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int RequestCode
);

Bool RTCharacterRegisterRequestCraftRecipe(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int RequestCode,
    Int InventorySlotCount,
    RTRequestCraftInventorySlotRef InventorySlots
);

Bool RTCharacterAddRequestCraftFavorite(
    RTCharacterRef Character,
    Int32 RequestCode
);

Bool RTCharacterRemoveRequestCraftFavorite(
    RTCharacterRef Character,
    Int32 RequestCode
);

Bool RTCharacterIsRequestCraftRecipeFavorited(
    RTCharacterRef Character, 
    Int32 RequestCode
);

Bool RTCharacterHasRequiredItemsForRecipe(
    RTCharacterRef Character,
    RTRuntimeDataContextRef Context,
    RTRuntimeRef Runtime,
    Int32 RequestCode,
    Int32 InventorySlotCount,
    struct _RTRequestCraftInventorySlot* InventoryItemIndexes
);

Bool RTCharacterIsRequestSlotActive(
    RTCharacterRef Character,
    Int SlotIndex
);

Bool RTCharacterHasOpenRequestSlot(
    RTCharacterRef Character
);

Void RTCharacterSetRequestSlotActive(
    RTCharacterRef Character,
    RTRuntimeDataContextRef Context,
    RTRuntimeRef Runtime,
    Int SlotIndex,
    Int ArrayIndex,
    Int32 RequestCode,
    Int32 InventorySlotCount,
    struct _RTRequestCraftInventorySlot* InventoryItemIndexes
);

Bool RTCharacterClaimCraftSlot(
    RTCharacterRef Character,
    RTRuntimeDataContextRef Context,
    RTRuntimeRef Runtime,
    UInt32 SlotIndex,
    Int32 RequestCode,
    Int32 InventorySlotIndex
);

UInt8 RTCharacterGetRequestStatus(
    RTCharacterRef Character,
    UInt32 SlotIndex
);

Bool RTCharacterClearRequestSlot(
    RTCharacterRef Character,
    UInt32 SlotIndex
);

Bool RTCharacterHasAmityForRequest(
    RTCharacterRef Character,
    RTRuntimeDataContextRef Context,
    Int32 RequestCode
);

Int32 RTRequestGetArrayIndex(
    RTRuntimeRef Runtime,
    RTCharacterRequestCraftInfoRef Requests,
    Int32 SlotIndex
);

Void RTCharacterInitializeRequestTimer(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterUpdateRequestTimer(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
    Bool ForceUpdate
);

Void RTRequestCraftCountdownCharacter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

EXTERN_C_END