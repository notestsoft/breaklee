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
	struct _RTRequestCraftInventorySlot InventoryItemIndexes[]
) {
	Bool HasItems = true;
	RTDataRequestCraftRecipeRef RecipeData = RTRuntimeDataRequestCraftRecipeGet(Context, RequestCode);
	struct MaterialOptionCodeTracker InventoryIndexOptionCodes[RUNTIME_INVENTORY_PAGE_SIZE * RUNTIME_INVENTORY_PAGE_COUNT] = { 0 };

	// build option code array from recipe
	for (int j = 0; j < RecipeData->RequestCraftRecipeMaterialCount; j++) {
		InventoryIndexOptionCodes[j].ItemID = RecipeData->RequestCraftRecipeMaterialList[j].ItemIndex;
		InventoryIndexOptionCodes[j].OptionCode = RecipeData->RequestCraftRecipeMaterialList[j].ItemOption;
		InventoryIndexOptionCodes[j].CurrentEncounteredAmount = 0;
		InventoryIndexOptionCodes[j].MaxEncounterAmount = RecipeData->RequestCraftRecipeMaterialList[j].ItemCount;
	}

	Int32 EncounteredOptionIndex = 0;
	for (int i = 0; i < InventorySlotCount; i++) {
		Int32 InvSlotIndex = InventoryItemIndexes[i].InventorySlotIndex;
		RTItemSlotRef InventoryItem = RTInventoryGetSlot(Runtime, &(Character->Data.InventoryInfo), InvSlotIndex);
		if (InventoryIndexOptionCodes[EncounteredOptionIndex].ItemID != (InventoryItem->Item.ID & RUNTIME_ITEM_MASK_INDEX)) {
			return false;
		}
		InventoryIndexOptionCodes[EncounteredOptionIndex].CurrentEncounteredAmount += InventoryItemIndexes[i].Count;

		if (RTInventoryGetConsumableItemCount(Runtime, &(Character->Data.InventoryInfo), InventoryItem->Item.ID & RUNTIME_ITEM_MASK_INDEX, InventoryIndexOptionCodes[EncounteredOptionIndex].OptionCode, InvSlotIndex) < InventoryItemIndexes[i].Count) {
			HasItems = false;
			break;
		}
		if (InventoryIndexOptionCodes[EncounteredOptionIndex].CurrentEncounteredAmount == InventoryIndexOptionCodes[EncounteredOptionIndex].MaxEncounterAmount)
		{
			EncounteredOptionIndex++;
		}
		else if (InventoryIndexOptionCodes[EncounteredOptionIndex].CurrentEncounteredAmount > InventoryIndexOptionCodes[EncounteredOptionIndex].MaxEncounterAmount) {
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
	struct MaterialOptionCodeTracker InventoryIndexOptionCodes[RUNTIME_INVENTORY_PAGE_SIZE * RUNTIME_INVENTORY_PAGE_COUNT] = { 0 };
	
	// build option code array from recipe
	for (int j = 0; j < RecipeData->RequestCraftRecipeMaterialCount; j++) {
		InventoryIndexOptionCodes[j].ItemID = RecipeData->RequestCraftRecipeMaterialList[j].ItemIndex;
		InventoryIndexOptionCodes[j].OptionCode = RecipeData->RequestCraftRecipeMaterialList[j].ItemOption;
		InventoryIndexOptionCodes[j].CurrentEncounteredAmount = 0;
		InventoryIndexOptionCodes[j].MaxEncounterAmount = RecipeData->RequestCraftRecipeMaterialList[j].ItemCount;
	}

	Int32 EncounteredOptionIndex = 0;
	for (int i = 0; i < InventorySlotCount; i++) {
		Int32 InvSlotIndex = InventoryItemIndexes[i].InventorySlotIndex;
		RTItemSlotRef InventoryItem = RTInventoryGetSlot(Runtime, &(Character->Data.InventoryInfo), InvSlotIndex);
		if (InventoryIndexOptionCodes[EncounteredOptionIndex].ItemID != (InventoryItem->Item.ID & RUNTIME_ITEM_MASK_INDEX)) {
			return;
		}
		InventoryIndexOptionCodes[EncounteredOptionIndex].CurrentEncounteredAmount += InventoryItemIndexes[i].Count;

		RTInventoryConsumeItem(Runtime, &(Character->Data.InventoryInfo), InventoryItem->Item.ID & RUNTIME_ITEM_MASK_INDEX, InventoryIndexOptionCodes[EncounteredOptionIndex].OptionCode, InventoryItemIndexes[i].Count, InvSlotIndex);
		if (InventoryIndexOptionCodes[EncounteredOptionIndex].CurrentEncounteredAmount == InventoryIndexOptionCodes[EncounteredOptionIndex].MaxEncounterAmount)
		{
			EncounteredOptionIndex++;
		}
		else if (InventoryIndexOptionCodes[EncounteredOptionIndex].CurrentEncounteredAmount > InventoryIndexOptionCodes[EncounteredOptionIndex].MaxEncounterAmount) {
			return;
		}
	}

	Character->SyncMask.InventoryInfo = true;
	
	// 2. Set data in the request slot.
	Character->Data.RequestCraftInfo.Slots[SlotIndex].RequestCode = RequestCode;
	Character->Data.RequestCraftInfo.Slots[SlotIndex].SlotIndex = SlotIndex;
	Character->Data.RequestCraftInfo.Slots[SlotIndex].Timestamp = RecipeData->Time;
	Character->Data.RequestCraftInfo.Slots[SlotIndex].Result = 1;
	
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

	if (Character->Data.RequestCraftInfo.Slots[SlotIndex].Result != 2) {
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

	Info("RequestClearSlot(%d)", SlotIndex);

	/*Int32 RequestIndex = RTInventoryGetSlotIndex(
		Runtime,
		Inventory,
		SlotIndex
	);*/
	Int32 RequestIndex = SlotIndex;
	if (Character->Data.RequestCraftInfo.Slots[RequestIndex].SlotIndex != RequestIndex) {
		Info("Memory misaligned request craft Slots array RequestIndex: %d, SlotIndex in slot: %d", RequestIndex, Character->Data.RequestCraftInfo.Slots[RequestIndex].SlotIndex);
	}
	if (RequestIndex < 0) return false;

	Int32 TailLength = Character->Data.RequestCraftInfo.Info.SlotCount - RequestIndex - 1;
	if (TailLength > 0) {
		memmove(
			&Character->Data.RequestCraftInfo.Slots[RequestIndex],
			&Character->Data.RequestCraftInfo.Slots[RequestIndex + 1],
			TailLength * sizeof(struct _RTRequestCraftSlot)
		);
	}

	Character->Data.RequestCraftInfo.Info.SlotCount -= 1;
	//struct _RTRequestCraftSlot Slot = Character->Data.RequestCraftInfo.Slots[RequestIndex];
	//Slot.RequestCode = 0;
	//Slot.Result = 0;
	//Slot.Timestamp = 0;
	Character->SyncMask.RequestCraftInfo = true;
	return true;
}