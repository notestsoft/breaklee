#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_UNKNOWN_SHOP_ITEM_PRICE) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	S2C_DATA_GET_UNKNOWN_SHOP_ITEM_PRICE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_UNKNOWN_SHOP_ITEM_PRICE);
	Response->Count = 1;

	S2C_DATA_GET_UNKNOWN_SHOP_ITEM_PRICE_INDEX *ItemPrice = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_GET_UNKNOWN_SHOP_ITEM_PRICE_INDEX);
	ItemPrice->SlotIndex = 0;
	ItemPrice->ItemID.ID = 1;
	ItemPrice->ItemOptions = 0;
	ItemPrice->Amount = 10;

	// TODO: Add implementation!

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
