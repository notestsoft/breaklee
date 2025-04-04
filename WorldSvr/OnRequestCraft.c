#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "Enumerations.h"
#include "RuntimeLib/RequestCraft.h"

CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT_REGISTER) {
	if (!Character) goto error;

	RTDataRequestCraftRecipeRef RecipeData = RTRuntimeDataRequestCraftRecipeGet(Runtime->Context, Packet->RequestCode);
	if (!RecipeData) goto error;

	Int PacketLength = sizeof(C2S_DATA_REQUEST_CRAFT_REGISTER) + sizeof(struct _RTRequestCraftInventorySlot) * Packet->InventorySlotCount + sizeof(C2S_DATA_REQUEST_CRAFT_REGISTER_TAIL);
	if (PacketLength != Packet->Length) goto error;

	S2C_DATA_REQUEST_CRAFT_REGISTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_REGISTER);
	Response->Result = 0;

	if (RTCharacterRegisterRequestCraftRecipe(Runtime, Character, Packet->RequestCode, Packet->InventorySlotCount, Packet->InventorySlots)) {
		Response->Result = 1;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_REQUEST_CRAFT_REGISTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_REGISTER);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT_START) {
	// sanity and cheat checks
	if (!Character) goto error;

	Int32 ArrayIndex = RTRequestGetArrayIndex(Runtime, &Character->Data.RequestCraftInfo, Packet->RequestSlotIndex);
	if (ArrayIndex < 0) {
		ArrayIndex = Character->Data.RequestCraftInfo.Info.SlotCount;
		Info("NextFreeSlotIndex: %d, ", ArrayIndex);
	}
	
	if (!RTCharacterHasAmityForRequest(Character, Runtime->Context, Packet->RequestCode)) {
		goto error;
	}
	if (!RTCharacterHasRequiredItemsForRecipe(Character, Runtime->Context, Runtime, (Int32)Packet->RequestCode, Packet->InventorySlotCount, Packet->InventorySlots)) {
		goto error;
	}
	if (ArrayIndex >= 0 && RTCharacterIsRequestSlotActive(Character, ArrayIndex)) {
		goto error;
	}
	if (!RTCharacterHasOpenRequestSlot(Character)) {
		goto error;
	}

	S2C_DATA_REQUEST_CRAFT_START* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_START);
	RTCharacterSetRequestSlotActive(Character, Runtime->Context, Runtime, Packet->RequestSlotIndex, ArrayIndex, Packet->RequestCode, Packet->InventorySlotCount, Packet->InventorySlots);
	Response->Result = 1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		Info("Error has occurred while starting a request craft.");
		S2C_DATA_REQUEST_CRAFT_START* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_START);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT_END) {
	if (!Character) goto error;

	UInt8 ReturnStatus = 0;
	UInt8 CraftStatus = RTCharacterGetRequestStatus(Character, RTRequestGetArrayIndex(Runtime, &Character->Data.RequestCraftInfo, Packet->RequestSlotIndex));

	if (CraftStatus < REQUEST_CRAFT_STATUS_SUCCESS) goto error;
	if (Packet->InventorySlotCount < 1 && CraftStatus == REQUEST_CRAFT_STATUS_SUCCESS) goto error;

	if (CraftStatus == REQUEST_CRAFT_STATUS_SUCCESS) 
	{
		Bool Success = RTCharacterClaimCraftSlot(Character, Runtime->Context, Runtime, RTRequestGetArrayIndex(Runtime, &Character->Data.RequestCraftInfo, Packet->RequestSlotIndex), Packet->RequestCode, Packet->InventorySlots[0].InventorySlotIndex);
		if (!Success) {
			ReturnStatus = 0;
		}
		else {
			ReturnStatus = 1;
		}
	}
	if (CraftStatus == REQUEST_CRAFT_STATUS_FAIL)
	{
		RTCharacterClearRequestSlot(Character, Packet->InventorySlots[0].InventorySlotIndex);
		Character->SyncMask.RequestCraftInfo = true;
		ReturnStatus = 1;
	}
	
	S2C_DATA_REQUEST_CRAFT_END* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_END);
	Response->Result = ReturnStatus;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CHANGE_RECIPE_FAVORITE) {
	if (!Character) goto error;

	//set character data for syncing
	if (Packet->FavoriteStatus[0] == 0x01) {
		RTCharacterFavoritedRequestCraftFlagSet(Character, Packet->CraftCode);
	}
	else {
		RTCharacterFavoritedRequestCraftFlagClear(Character, Packet->CraftCode);
	}

	// send response back to client confirming we got the flag update
	S2C_DATA_CHANGE_RECIPE_FAVORITE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CHANGE_RECIPE_FAVORITE);
	Response->Unknown1 = 0x01;
	memcpy(Response->FavoriteStatus, Packet->FavoriteStatus, sizeof(Packet->FavoriteStatus));
	Response->CraftCode = Packet->CraftCode;
	SocketSend(Socket, Connection, Response);
	return;
error:
	Error("Something went wrong when changing a favorite request craft recipe.");
	SocketDisconnect(Socket, Connection);
}