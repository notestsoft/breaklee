#pragma once

#include "Base.h"
#include "Constants.h"

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
    Int32 CraftCode
);

Bool RTCharacterRemoveRequestCraftFavorite(
    RTCharacterRef Character,
    Int32 CraftCode
);

Bool RTCharacterIsRequestCraftRecipeFavorited(
    RTCharacterRef Character, 
    Int32 CraftCode
);

Bool RTCharacterHasRequiredItemsForRecipe(
    RTCharacterRef Character,
    RTRuntimeDataContextRef Context,
    RTRuntimeRef Runtime,
    Int32 RequestCode,
    Int32 InventorySlotCount,
    struct _RTRequestCraftInventorySlot InventoryItemIndexes[]
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
    Int32 RequestCode,
    Int32 InventorySlotCount,
    struct _RTRequestCraftInventorySlot InventoryItemIndexes[]
);

Bool RTCharacterAttemptRequestCraftChance(
    float rate
);

EXTERN_C_END