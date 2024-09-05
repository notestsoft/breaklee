#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_FORCEGEM_PRICE_POOL) {
	S2C_DATA_GET_FORCEGEM_PRICE_POOL* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_FORCEGEM_PRICE_POOL);
	Response->Count = Runtime->Context->ForceGemPriceCount;

	for (Int32 Index = 0; Index < Runtime->Context->ForceGemPriceCount; Index++) {
		S2C_DATA_GET_FORCEGEM_PRICE* ResponseData = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_GET_FORCEGEM_PRICE);
		ResponseData->PoolIndex = ResponseData->PoolIndex;
		ResponseData->PriceIndex = ResponseData->PriceIndex;
		ResponseData->Price = ResponseData->Price;
	}

	SocketSend(Socket, Connection, Response);
}
