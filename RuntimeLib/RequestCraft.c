#include "RequestCraft.h"
#include "Runtime.h"
#include <time.h>

Void RTCharacterRegisteredRequestCraftFlagClear(
	RTCharacterRef Character,
	Int RequestCode
) {
	assert(0 <= RequestCode && RequestCode < RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE * RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_RECIPE_COUNT);

	Character->Data.RequestCraftInfo.Info.RegisteredFlags[RequestCode / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] &= ~(1 << (RequestCode % RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE));
	Character->SyncMask.RequestCraftInfo = true;
}

Void RTCharacterRegisteredRequestCraftFlagSet(
	RTCharacterRef Character,
	Int RequestCode
) {
	assert(0 <= RequestCode && RequestCode < RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE * RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_RECIPE_COUNT);

	Character->Data.RequestCraftInfo.Info.RegisteredFlags[RequestCode / RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE] |= (1 << (RequestCode % RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE));
	Character->SyncMask.RequestCraftInfo = true;
}

Bool RTCharacterRegisteredRequestCraftFlagIsSet(
	RTCharacterRef Character,
	Int RequestCode
) {
	assert(0 <= RequestCode && RequestCode < RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE * RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_RECIPE_COUNT);

	return (Character->Data.RequestCraftInfo.Info.RegisteredFlags[RequestCode / RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE] & (1 << (RequestCode % RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE))) > 0;
}

Void RTCharacterFavoritedRequestCraftFlagClear(
	RTCharacterRef Character,
	Int RequestCode
) {
	assert(0 <= RequestCode && RequestCode < RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE * RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_RECIPE_COUNT);

	Character->Data.RequestCraftInfo.Info.FavoriteFlags[RequestCode / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] &= ~(1 << (RequestCode % RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE));
	Character->SyncMask.RequestCraftInfo = true;
}

Void RTCharacterFavoritedRequestCraftFlagSet(
	RTCharacterRef Character,
	Int RequestCode
) {
	assert(0 <= RequestCode && RequestCode < RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE * RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_RECIPE_COUNT);

	Character->Data.RequestCraftInfo.Info.FavoriteFlags[RequestCode / RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE] |= (1 << (RequestCode % RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE));
	Character->SyncMask.RequestCraftInfo = true;
}

Bool RTCharacterFavoritedRequestCraftFlagIsSet(
	RTCharacterRef Character,
	Int RequestCode
) {
	assert(0 <= RequestCode && RequestCode < RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE * RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_RECIPE_COUNT);

	return (Character->Data.RequestCraftInfo.Info.FavoriteFlags[RequestCode / RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE] & (1 << (RequestCode % RUNTIME_CHARACTER_REQUEST_CRAFT_RECIPE_FLAG_SIZE))) > 0;
}

Int RTCharacterGetRequestCraftLevel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	for (Int Index = 0; Index < Runtime->Context->RequestCraftBaseCount; Index += 1) {
		RTDataRequestCraftBaseRef RequestCraftBaseData = &Runtime->Context->RequestCraftBaseList[Index];
		if (RequestCraftBaseData->RequestExpMin > Character->Data.RequestCraftInfo.Info.Exp) continue;
		if (RequestCraftBaseData->RequestExpMax < Character->Data.RequestCraftInfo.Info.Exp) continue;

		return RequestCraftBaseData->RequestLevel;
	}

	return 0;
}

Bool RTCharacterIsRequestCraftRecipeRegistered(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int RequestCode
) {
	return RTCharacterRegisteredRequestCraftFlagIsSet(Character, RequestCode);
}

Bool RTCharacterRegisterRequestCraftRecipe(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int RequestCode,
	Int InventorySlotCount,
	RTRequestCraftInventorySlotRef InventorySlots
) {
	if (RTCharacterIsRequestCraftRecipeRegistered(Runtime, Character, RequestCode)) return false;

	RTDataRequestCraftRecipeRef RecipeData = RTRuntimeDataRequestCraftRecipeGet(Runtime->Context, RequestCode);
	if (!RecipeData) return false;
	if (RecipeData->RegisterExp > Character->Data.RequestCraftInfo.Info.Exp) return false;

	Bool IsItemRegistration = (InventorySlotCount > 0);
	Bool IsItemRequired = (
		RecipeData->RegisterItemCount == 2 &&
		RecipeData->RegisterItem[0] > 0 &&
		RecipeData->RegisterItem[1] > 0
	);

	if (!IsItemRegistration) {
		if (Character->Data.Info.Alz < RecipeData->RegisterCost) return false;

		Character->Data.Info.Alz -= RecipeData->RegisterCost;
		Character->SyncMask.Info = true;
	}
	else if (IsItemRequired) {
		Int64 ConsumableItemCount = 0;

		for (Int Index = 0; Index < InventorySlotCount; Index += 1) {
			struct _RTRequestCraftInventorySlot InventorySlot = InventorySlots[Index];

			if (!RTInventoryCanConsumeItem(
				Runtime,
				&Character->Data.InventoryInfo,
				RecipeData->RegisterItem[0],
				InventorySlot.Count,
				InventorySlot.InventorySlotIndex
			)) return false;

			ConsumableItemCount += InventorySlot.Count;
		}

		if (ConsumableItemCount < RecipeData->RegisterItem[1]) return false;

		for (Int Index = 0; Index < InventorySlotCount; Index += 1) {
			struct _RTRequestCraftInventorySlot InventorySlot = InventorySlots[Index];

			RTInventoryConsumeItem(
				Runtime,
				&Character->Data.InventoryInfo,
				RecipeData->RegisterItem[0],
				0,
				InventorySlot.Count,
				InventorySlot.InventorySlotIndex
			);
		}

		Character->SyncMask.InventoryInfo = true;
	}

	RTCharacterRegisteredRequestCraftFlagSet(Character, RequestCode);

	return true;
}

Bool RTCharacterIsRequestCraftRecipeFavorited(RTCharacterRef Character, Int32 CraftCode) {
	return RTCharacterFavoritedRequestCraftFlagIsSet(Character, CraftCode);
}

Bool RTCharacterAddRequestCraftFavorite(
	RTCharacterRef Character,
	Int32 CraftCode
) {
	RTCharacterFavoritedRequestCraftFlagSet(Character, CraftCode);
	return true;
}

Bool RTCharacterRemoveRequestCraftFavorite(
	RTCharacterRef Character,
	Int32 CraftCode
) {
	RTCharacterFavoritedRequestCraftFlagClear(Character, CraftCode);
	return true;
}

Bool RTCharacterHasRequiredItemsForRecipe(
	RTCharacterRef Character,
	RTRuntimeDataContextRef Context,
	RTRuntimeRef Runtime,
	Int32 RequestCode,
	Int32 InventorySlotCount,
	struct _RTRequestCraftInventorySlot InventoryItemIndexes[]
) {
	Bool HasItems = true;
	for (int i = 0; i < InventorySlotCount; i++) {
		Info("InventoryIndex loop index: %d first", i);
		Int32 InvSlotIndex = InventoryItemIndexes[i].InventorySlotIndex;
		struct _RTItemSlot ItemSlot = Character->Data.InventoryInfo.Slots[InvSlotIndex];
		Info("InventoryIndex loop index: %d after ItemSlot found", i);
		RTDataRequestCraftRecipeRef RecipeData = RTRuntimeDataRequestCraftRecipeGet(Context, RequestCode);
		Info("InventoryIndex loop index: %d after get requestcraftrecipe config", i);
		Info("Item data we are looking for: ItemID: %d, ItemOptions: %d, InvSlotIndex: %d", RecipeData->RequestCraftRecipeMaterialList[i].ItemIndex, RecipeData->RequestCraftRecipeMaterialList[i].ItemOption, InvSlotIndex);
		if (RTInventoryGetConsumableItemCount(Runtime, &(Character->Data.InventoryInfo), RecipeData->RequestCraftRecipeMaterialList[i].ItemIndex, RecipeData->RequestCraftRecipeMaterialList[i].ItemOption, InvSlotIndex) < RecipeData->RequestCraftRecipeMaterialList[i].ItemCount) {
			HasItems = false;
			break;
		}
	}
	return HasItems;
}

Bool RTCharacterIsRequestSlotActive(
	RTCharacterRef Character,
	Int SlotIndex
) {
	if (Character->Data.RequestCraftInfo.Slots[SlotIndex].RequestCode > 0) {
		return true;
	}
	return false;
}

Bool RTCharacterHasOpenRequestSlot(
	RTCharacterRef Character
) {
	if (Character->Data.RequestCraftInfo.Info.SlotCount < RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_SLOT_COUNT) {
		return true;
	}
	return false;
}

Void RTCharacterSetRequestSlotActive(
	RTCharacterRef Character,
	RTRuntimeDataContextRef Context,
	RTRuntimeRef Runtime,
	Int SlotIndex,
	Int32 RequestCode,
	Int32 InventorySlotCount,
	struct _RTRequestCraftInventorySlot InventoryItemIndexes[]
) {
	// 1. Take away the required items.
	RTDataRequestCraftRecipeRef RecipeData = RTRuntimeDataRequestCraftRecipeGet(Context, RequestCode);
	// sanity checks
	if (InventorySlotCount != RecipeData->RequestCraftRecipeMaterialCount) {
		return;
	}

	for (int i = 0; i < RecipeData->RequestCraftRecipeMaterialCount; i++) {
		struct _RTDataRequestCraftRecipeMaterial ItemMaterial = RecipeData->RequestCraftRecipeMaterialList[i];
		Int64 AmountConsumed = 0;
		Int32 InvSlotIndex = InventoryItemIndexes[i].InventorySlotIndex;
		while (AmountConsumed < ItemMaterial.ItemCount) {
			// TODO: get next inventory index of same next item each loop
			AmountConsumed += RTInventoryConsumeItem(Runtime, &(Character->Data.InventoryInfo), ItemMaterial.ItemIndex, ItemMaterial.ItemOption, ItemMaterial.ItemCount - AmountConsumed, InvSlotIndex);
		}
	}
	Character->SyncMask.InventoryInfo = true;
	
	// 2. Set data in the request slot.
	Character->Data.RequestCraftInfo.Slots[SlotIndex].RequestCode = RequestCode;
	Character->Data.RequestCraftInfo.Slots[SlotIndex].SlotIndex = SlotIndex;
	Character->Data.RequestCraftInfo.Slots[SlotIndex].Timestamp = PlatformGetTickCount();
	// RNG calculation
	if (RTCharacterAttemptRequestCraftChance(RecipeData->SuccessRate)) {
		Character->Data.RequestCraftInfo.Slots[SlotIndex].Result = 1;
	}
	else {
		Character->Data.RequestCraftInfo.Slots[SlotIndex].Result = 0;
	}
	if (Character->Data.RequestCraftInfo.Slots[SlotIndex].Result == 1) {
		Character->Data.RequestCraftInfo.Info.Exp += RecipeData->ResultExp;
	}
	Character->Data.RequestCraftInfo.Info.SlotCount += 1;
	Character->SyncMask.RequestCraftInfo = true;
	
	return;
error:
	return;
}

Bool RTCharacterAttemptRequestCraftChance(
	float rate
) {
	Int32 workingRate = rate / 10.0f;

	if (workingRate < 0.0f) workingRate = 0.0f; 
	if (workingRate > 100.0f) workingRate = 100.0f;

	int roll = rand() % 100;
	return roll < (int)workingRate;  
}