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

/*

CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_START, 2250, 13133,
	C2S_DATA_SIGNATURE;
	UInt32 RequestCode;
	UInt8 RequestSlotIndex;
	Int32 Unknown1; // - Maybe AuthCaptcha
	Int32 InventorySlotCount;
	C2S_REQUEST_CRAFT_INVENTORY_SLOT InventorySlots[0];
	// UInt8 Unknown2[4168]; - Maybe AuthCaptcha
)
*/

CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT_START) {
	S2C_DATA_REQUEST_CRAFT_START* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_START);
	Response->Result = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

/*
CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_END, 2251, 13133,
    C2S_DATA_SIGNATURE;
	UInt32 RequestCode;
	UInt8 RequestSlotIndex;
	Int32 Unknown1; // - Maybe AuthCaptcha
	Int32 InventorySlotCount;
	C2S_REQUEST_CRAFT_INVENTORY_SLOT InventorySlots[0];
	// UInt8 Unknown2[4168]; - Maybe AuthCaptcha
)
*/

CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT_END) {
	S2C_DATA_REQUEST_CRAFT_END* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_END);
	Response->Result = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

/*
CLIENT_PROTOCOL(S2C, REQUEST_CRAFT_UPDATE, 2252, 13133,
    S2C_DATA_SIGNATURE;
	UInt32 ItemID;
	UInt8 RequestSlotIndex;
	UInt32 RequestCode;
	UInt32 Amity;
	UInt8 Success;
)
*/

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