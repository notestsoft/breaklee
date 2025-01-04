#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(PERSONAL_SHOP_OPEN) {
	if (!Character) goto error;

	Int PacketLength = sizeof(C2S_DATA_PERSONAL_SHOP_OPEN) + sizeof(struct _RTPersonalShopSlot) * Packet->ShopInfo.SlotCount + sizeof(Char) * Packet->ShopInfo.NameLength;
	if (Packet->Length != PacketLength) goto error;

	Char* ShopName = ((UInt8*)Packet) + (PacketLength - sizeof(Char) * Packet->ShopInfo.NameLength);

	S2C_DATA_PERSONAL_SHOP_OPEN* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PERSONAL_SHOP_OPEN);
	Response->Result = 1;

	if (Packet->ShopMode == C2S_PERSONAL_SHOP_OPEN_MODE_SELL) {
		if (RTCharacterPersonalShopOpen(
			Runtime,
			Character,
			&Packet->ShopInfo,
			Packet->ShopSlots,
			ShopName
		)) {
			Response->Result = 0;
			Response->PremiumShopItemID = Character->Data.PersonalShopInfo.PremiumShopItemID;
		}
	}
	
	if (Packet->ShopMode == C2S_PERSONAL_SHOP_OPEN_MODE_BUY) {

	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PERSONAL_SHOP_CLOSE) {
	if (!Character) goto error;

	S2C_DATA_PERSONAL_SHOP_CLOSE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PERSONAL_SHOP_CLOSE);
	if (!RTCharacterPersonalShopClose(Runtime, Character)) goto error;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PERSONAL_SHOP_VIEW) {
	if (!Character) goto error;

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_PERSONAL_SHOP_VIEW* Response = PacketBufferInit(PacketBuffer, S2C, PERSONAL_SHOP_VIEW);
	Response->Result = 1;
	
	RTCharacterRef TargetCharacter = RTWorldManagerGetCharacter(Runtime->WorldManager, Packet->EntityID);
	if (TargetCharacter && TargetCharacter->Data.PersonalShopInfo.IsActive) {
		Response->Result = 0;

		Response->SlotCount = TargetCharacter->Data.PersonalShopInfo.Info.SlotCount;
		for (Int Index = 0; Index < TargetCharacter->Data.PersonalShopInfo.Info.SlotCount; Index += 1) {
			RTPersonalShopSlotRef ShopSlot = &TargetCharacter->Data.PersonalShopInfo.Slots[Index];

			S2C_DATA_PERSONAL_SHOP_VIEW_SLOT* ResponseSlot = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_PERSONAL_SHOP_VIEW_SLOT);
			ResponseSlot->ShopSlotIndex = ShopSlot->ShopSlotIndex;
			ResponseSlot->ItemPrice = ShopSlot->ItemPrice;
			ResponseSlot->ItemID = ShopSlot->ItemID;
			ResponseSlot->ItemOptions = ShopSlot->ItemOptions;
			ResponseSlot->ItemDuration = 0; // TODO: Add item duration
			ResponseSlot->ItemSerial = 0;
		}
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_PERSONAL_SHOP_VIEW* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PERSONAL_SHOP_VIEW);
		Response->Result = 1;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(PERSONAL_SHOP_EXIT) {
	if (!Character) goto error;

	S2C_DATA_PERSONAL_SHOP_EXIT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PERSONAL_SHOP_EXIT);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PERSONAL_SHOP_BUY) {
	if (!Character) goto error;

	S2C_DATA_PERSONAL_SHOP_BUY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PERSONAL_SHOP_BUY);
	Response->Result = S2C_PERSONAL_SHOP_BUY_RESULT_FAIL;

	if (RTCharacterPersonalShopBuy(
		Runtime,
		Character,
		Packet->EntityID,
		Packet->ItemID,
		Packet->InventorySlotIndex,
		Packet->ShopSlotIndex,
		Packet->ItemPrice
	)) {
		Response->Result = 0;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_PERSONAL_SHOP_BUY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PERSONAL_SHOP_BUY);
		Response->Result = S2C_PERSONAL_SHOP_BUY_RESULT_FAIL;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(PERSONAL_SHOP_REGISTER_ITEM) {
	if (!Character) goto error;
	if (!RTEntityIsEqual(Character->ID, Packet->EntityID)) goto error;

	S2C_DATA_PERSONAL_SHOP_REGISTER_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PERSONAL_SHOP_REGISTER_ITEM);
	Response->Result = 1;

	if (RTCharacterPersonalShopRegisterItem(
		Runtime,
		Character,
		Packet->InventorySlotIndex,
		Packet->ItemID,
		Packet->ItemOptions,
		Packet->ShopSlotIndex,
		Packet->ItemPrice
	)) {
		Response->Result = 0;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_PERSONAL_SHOP_REGISTER_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PERSONAL_SHOP_REGISTER_ITEM);
		Response->Result = 1;
		SocketSend(Socket, Connection, Response);
	}
}

