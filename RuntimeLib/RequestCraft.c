#include "RequestCraft.h"
#include "Runtime.h"

enum {
	REQUEST_CRAFT_STATUS_INACTIVE,
	REQUEST_CRAFT_STATUS_IN_PROGRESS,
	REQUEST_CRAFT_STATUS_SUCCESS,
	REQUEST_CRAFT_STATUS_FAIL,
};

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
	if (!RTCharacterHasAmityForRequest(Character, Runtime->Context, RequestCode)) return false;

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

Bool RTCharacterIsRequestCraftRecipeFavorited(RTCharacterRef Character, Int32 RequestCode) {
	return RTCharacterFavoritedRequestCraftFlagIsSet(Character, RequestCode);
}

Bool RTCharacterAddRequestCraftFavorite(
	RTCharacterRef Character,
	Int32 RequestCode
) {
	if (RequestCode < 0 || RequestCode >= RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_RECIPE_COUNT) return false;

	RTCharacterFavoritedRequestCraftFlagSet(Character, RequestCode);
	return true;
}

Bool RTCharacterRemoveRequestCraftFavorite(
	RTCharacterRef Character,
	Int32 RequestCode
) {
	RTCharacterFavoritedRequestCraftFlagClear(Character, RequestCode);
	return true;
}


struct MaterialOptionCodeTracker {
	Int32 ItemID;
	Int32 OptionCode;
	Int32 CurrentEncounteredAmount;
	Int32 MaxEncounterAmount;
};

Bool RTCharacterHasRequiredItemsForRecipe(
	RTCharacterRef Character,
	RTRuntimeDataContextRef Context,
	RTRuntimeRef Runtime,
	Int32 RequestCode,
	Int32 InventorySlotCount,
	struct _RTRequestCraftInventorySlot* InventoryItemIndexes
) {
	Bool HasItems = true;
	RTDataRequestCraftRecipeRef RecipeData = RTRuntimeDataRequestCraftRecipeGet(Context, RequestCode);

	Int32 EncounteredOptionIndex = 0;
	Int32 CurrentEncounteredAmount = 0;

	for (Int LoopIndex = 0; LoopIndex < InventorySlotCount; LoopIndex++) {
		Int32 InvSlotIndex = InventoryItemIndexes[LoopIndex].InventorySlotIndex;
		RTItemSlotRef InventoryItem = RTInventoryGetSlot(Runtime, &(Character->Data.InventoryInfo), InvSlotIndex);

		if (EncounteredOptionIndex >= RecipeData->RequestCraftRecipeMaterialCount) {
			return false;
		}

		struct _RTDataRequestCraftRecipeMaterial* CurrentMaterial = &RecipeData->RequestCraftRecipeMaterialList[EncounteredOptionIndex];

		if ((InventoryItem->Item.ID & RUNTIME_ITEM_MASK_INDEX) != CurrentMaterial->ItemIndex) {
			return false;
		}

		CurrentEncounteredAmount += InventoryItemIndexes[LoopIndex].Count;

		if (RTInventoryGetConsumableItemCount(Runtime, &Character->Data.InventoryInfo,
			InventoryItem->Item.ID & RUNTIME_ITEM_MASK_INDEX, CurrentMaterial->ItemOption, InvSlotIndex) < InventoryItemIndexes[LoopIndex].Count) {
			HasItems = false;
			break;
		}

		if (CurrentEncounteredAmount == CurrentMaterial->ItemCount) {
			EncounteredOptionIndex++;
			CurrentEncounteredAmount = 0;
		}
		else if (CurrentEncounteredAmount > CurrentMaterial->ItemCount) {
			return false;
		}
	}
	return HasItems;
}

Bool RTCharacterIsRequestSlotActive(
	RTCharacterRef Character,
	Int SlotIndex
) {
	if (Character->Data.RequestCraftInfo.Info.SlotCount > SlotIndex) {
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

	Int32 EncounteredOptionIndex = 0;
	Int32 CurrentEncounteredAmount = 0;

	for (Int LoopIndex = 0; LoopIndex < InventorySlotCount; LoopIndex++) {
		Int32 InvSlotIndex = InventoryItemIndexes[LoopIndex].InventorySlotIndex;
		RTItemSlotRef InventoryItem = RTInventoryGetSlot(Runtime, &(Character->Data.InventoryInfo), InvSlotIndex);

		// Get the current required material
		if (EncounteredOptionIndex >= RecipeData->RequestCraftRecipeMaterialCount) {
			return; // More items in inventory than required
		}

		struct _RTDataRequestCraftRecipeMaterial* CurrentMaterial = &RecipeData->RequestCraftRecipeMaterialList[EncounteredOptionIndex];

		// Check if current inventory item matches the required material
		if ((InventoryItem->Item.ID & RUNTIME_ITEM_MASK_INDEX) != CurrentMaterial->ItemIndex) {
			return;
		}

		CurrentEncounteredAmount += InventoryItemIndexes[LoopIndex].Count;

		// Consume the required amount from inventory
		RTInventoryConsumeItem(Runtime, &(Character->Data.InventoryInfo),
			InventoryItem->Item.ID & RUNTIME_ITEM_MASK_INDEX,
			CurrentMaterial->ItemOption,
			InventoryItemIndexes[LoopIndex].Count,
			InvSlotIndex);

		// Check if we have met or exceeded the required amount for this material
		if (CurrentEncounteredAmount == CurrentMaterial->ItemCount) {
			EncounteredOptionIndex++;
			CurrentEncounteredAmount = 0; // Reset counter for next material
		}
		else if (CurrentEncounteredAmount > CurrentMaterial->ItemCount) {
			return; // More items consumed than required
		}
	}

	Character->SyncMask.InventoryInfo = true;
	
	// 2. Set data in the request slot.
	Character->Data.RequestCraftInfo.Slots[SlotIndex].RequestCode = RequestCode;
	Character->Data.RequestCraftInfo.Slots[SlotIndex].SlotIndex = SlotIndex;
	Character->Data.RequestCraftInfo.Slots[SlotIndex].Timestamp = RecipeData->Time;
	Character->Data.RequestCraftInfo.Slots[SlotIndex].Result = REQUEST_CRAFT_STATUS_IN_PROGRESS;
	
	Character->Data.RequestCraftInfo.Info.SlotCount += 1;
	Character->SyncMask.RequestCraftInfo = true;
	
	return;
error:
	return;
}

Bool RTCharacterClaimCraftSlot(
	RTCharacterRef Character,
	RTRuntimeDataContextRef Context,
	RTRuntimeRef Runtime,
	UInt32 SlotIndex,
	Int32 RequestCode,
	Int32 InventorySlotIndex
) {
	// make sure slot is complete
	RTDataRequestCraftRecipeRef RecipeData = RTRuntimeDataRequestCraftRecipeGet(Context, RequestCode);

	if (Character->Data.RequestCraftInfo.Slots[SlotIndex].Result != REQUEST_CRAFT_STATUS_SUCCESS) {
		return false;
	}
	if (Character->Data.RequestCraftInfo.Info.SlotCount <= SlotIndex) {
		return false;
	}
	// slot is complete, add item to inventory data
	struct _RTItemSlot ItemSlot = { 0 };
	ItemSlot.Item.Serial = RecipeData->ResultItem[0];
	ItemSlot.ItemOptions = RecipeData->ResultItem[1];
	ItemSlot.Item.IsBroken = 0;
	struct _RTItemDuration ItemDuration = { 0 };
	ItemDuration.Serial = RecipeData->ResultItem[2];
	ItemSlot.ItemDuration = ItemDuration;
	ItemSlot.SlotIndex = InventorySlotIndex;
	if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &ItemSlot)) return false;
	

	Character->SyncMask.InventoryInfo = true;

	// clear the request slot
	RTCharacterClearRequestSlot(Character, SlotIndex);
	Character->SyncMask.RequestCraftInfo = true;

	return true;
}

UInt8 RTCharacterGetRequestStatus(
	RTCharacterRef Character,
	UInt32 SlotIndex
) {
	if (SlotIndex < 0 || SlotIndex >= RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_SLOT_COUNT) return 0;

	return Character->Data.RequestCraftInfo.Slots[SlotIndex].Result;
}

Bool RTCharacterClearRequestSlot(
	RTCharacterRef Character,
	UInt32 SlotIndex
) {
	if (SlotIndex < 0 || SlotIndex >= RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_SLOT_COUNT) return false;

	Int32 TailLength = Character->Data.RequestCraftInfo.Info.SlotCount - SlotIndex - 1;
	if (TailLength > 0) {
		memmove(
			&Character->Data.RequestCraftInfo.Slots[SlotIndex],
			&Character->Data.RequestCraftInfo.Slots[SlotIndex + 1],
			TailLength * sizeof(struct _RTRequestCraftSlot)
		);
	}

	Character->Data.RequestCraftInfo.Info.SlotCount -= 1;
	Character->SyncMask.RequestCraftInfo = true;
	return true;
}

Bool RTCharacterHasAmityForRequest(
	RTCharacterRef Character,
	RTRuntimeDataContextRef Context,
	Int32 RequestCode
) {
	RTDataRequestCraftRecipeRef RecipeData = RTRuntimeDataRequestCraftRecipeGet(Context, RequestCode);
	if (RecipeData->RegisterExp > Character->Data.RequestCraftInfo.Info.Exp) return false;

	return true;
}