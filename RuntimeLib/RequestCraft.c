#include "RequestCraft.h"
#include "Runtime.h"

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