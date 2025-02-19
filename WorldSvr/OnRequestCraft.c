#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
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
	S2C_DATA_REQUEST_CRAFT_START* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_START);
	if (!RTCharacterHasRequiredItemsForRecipe(Character, Runtime->Context, Runtime, (Int32)Packet->RequestCode, Packet->InventorySlotCount, Packet->InventorySlots)) {
		return;
	}
	if (RTCharacterIsRequestSlotActive(Character, Packet->RequestSlotIndex)) {
		goto error;
	}
	if (!RTCharacterHasOpenRequestSlot(Character)) {
		return;
	}
	RTCharacterSetRequestSlotActive(Character, Runtime->Context, Runtime, Packet->RequestSlotIndex, Packet->RequestCode, Packet->InventorySlotCount, Packet->InventorySlots);
	Response->Result = 1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	Info("Error has occurred while starting a request craft.");
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT_END) {
	UInt8 ReturnStatus = 0;
	UInt8 CraftStatus = RTCharacterGetRequestStatus(Character, Packet->RequestSlotIndex);
	if (CraftStatus < 2) goto error;
	if (Packet->InventorySlotCount < 1 && CraftStatus == 2) goto error;

	if (CraftStatus == 2) 
	{
		Bool success = RTCharacterClaimCraftSlot(Character, Runtime->Context, Runtime, Packet->RequestSlotIndex, Packet->RequestCode, Packet->InventorySlots[0].InventorySlotIndex);
		if (!success) {
			ReturnStatus = 0;
		}
		else {
			ReturnStatus = 1;
		}
	}
	if (CraftStatus == 3)
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
	//set character data for syncing
	if (Packet->FavoriteStatus[0] == 0x01) {
		RTCharacterFavoritedRequestCraftFlagSet(Character, Packet->CraftCode);
	}
	else {
		RTCharacterFavoritedRequestCraftFlagClear(Character, Packet->CraftCode);
	}

	// send response back to client confirming we got the flag update
	PacketBufferRef ClientPacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_CHANGE_RECIPE_FAVORITE* Response = PacketBufferInit(ClientPacketBuffer, S2C, CHANGE_RECIPE_FAVORITE);
	Response->Unknown1 = 0x01;
	memcpy(Response->FavoriteStatus, Packet->FavoriteStatus, sizeof(Packet->FavoriteStatus));
	Response->CraftCode = Packet->CraftCode;
	SocketSend(Socket, Connection, Response);
	return;
error:
	Error("Something went wrong when changing a favorite request craft recipe.");
}

Void ServerRequestCountdownCharacter(
	ServerRef Server,
	ServerContextRef Context,
	RTRuntimeRef Runtime,
	ClientContextRef Client,
	SocketRef Socket,
	RTCharacterRef Character
) {
	
	for (int i = 0; i < Character->Data.RequestCraftInfo.Info.SlotCount; i++) 
	{
		if (Character->Data.RequestCraftInfo.Slots[i].Result == 1) {
			RTDataRequestCraftRecipeRef RecipeData = RTRuntimeDataRequestCraftRecipeGet(Runtime->Context, Character->Data.RequestCraftInfo.Slots[i].RequestCode);
			Character->Data.RequestCraftInfo.Slots[i].Timestamp -= (Context->Config.WorldSvr.RequestCountdownTimer / 1000.0f);
			Character->SyncMask.RequestCraftInfo = true;
			if (Character->Data.RequestCraftInfo.Slots[i].Timestamp <= 0) {
				Character->Data.RequestCraftInfo.Slots[i].Timestamp = 0;
				
				// RNG calculation
				if (ServerAttemptRollByChance(RecipeData->SuccessRate / 10.0f)) {
					Character->Data.RequestCraftInfo.Slots[i].Result = 2;
				}
				else {
					Character->Data.RequestCraftInfo.Slots[i].Result = 3;
				}
				if (Character->Data.RequestCraftInfo.Slots[i].Result == 2) {
					Character->Data.RequestCraftInfo.Info.Exp += RecipeData->ResultExp;
				}
				// send update packet back
				PacketBufferRef ClientPacketBuffer = SocketGetNextPacketBuffer(Socket);
				S2C_DATA_REQUEST_CRAFT_UPDATE* Response = PacketBufferInit(ClientPacketBuffer, S2C, REQUEST_CRAFT_UPDATE);
				if (Character->Data.RequestCraftInfo.Slots[i].Result == 2)
				{
					Response->Success = 1;
				}
				else {
					Response->Success = 0;
				}
				Response->ItemID = Client->CharacterIndex;
				Response->RequestSlotIndex = Character->Data.RequestCraftInfo.Slots[i].SlotIndex;
				Response->RequestCode = Character->Data.RequestCraftInfo.Slots[i].RequestCode;
				Response->RequestCraftExp = RecipeData->ResultExp;

				SocketSend(Socket, Client->Connection, Response);
			}
		}
	}
}

Void ServerRequestCountdown(
	ServerRef Server,
	ServerContextRef Context,
	RTRuntimeRef Runtime
) {
	Timestamp Timestamp = GetTimestampMs();

	Bool PerformSync =
		(Timestamp - RequestSyncTimestamp) >= Context->Config.WorldSvr.RequestCountdownTimer;

	if (!PerformSync) return;
	RequestSyncTimestamp = GetTimestampMs();

	SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
	while (Iterator) {
		SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
		
		Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);
		if (Connection == NULL) continue;

		ClientContextRef Client = (ClientContextRef)Connection->Userdata;
		if (Client->CharacterIndex < 1) continue;

		RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, Client->CharacterIndex);
		if (!Character) continue;

		if (PerformSync) {
			ServerRequestCountdownCharacter(Server, Context, Runtime, Client, Context->ClientSocket, Character);
		}
	}
}

Bool ServerAttemptRollByChance(
	float rate
) {
	Int32 workingRate = rate;

	if (workingRate < 0.0f) workingRate = 0.0f;
	if (workingRate > 100.0f) workingRate = 100.0f;

	int roll = rand() % 100;
	return roll < (int)workingRate;
}