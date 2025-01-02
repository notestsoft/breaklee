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
		if (RTCharacterOpenPersonalShop(
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
