#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_FORCEGEM_PRICE_POOL) {
	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_GET_FORCEGEM_PRICE_POOL* Response = PacketBufferInit(PacketBuffer, S2C, GET_FORCEGEM_PRICE_POOL);
	Response->Count = Runtime->Context->ForceGemPriceCount;

	for (Int Index = 0; Index < Runtime->Context->ForceGemPriceCount; Index++) {
		S2C_DATA_GET_FORCEGEM_PRICE* ResponseData = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_GET_FORCEGEM_PRICE);
		ResponseData->PoolIndex = ResponseData->PoolIndex;
		ResponseData->PriceIndex = ResponseData->PriceIndex;
		ResponseData->Price = ResponseData->Price;
	}

	// Temporary Myth resurrect price insert
	// TODO: Remove this garbage
	Int32 MaxMythLevel = RTCharacterMythMasteryGetMaximumLevel(Runtime);
	RTDataMythRebirthPenaltyRef MythRebirthPenaltyRef = RTRuntimeDataMythRebirthPenaltyGet(Runtime->Context);

	for (Int Index = 0; Index < 5; Index++) {
		Response->Count += 1;
		S2C_DATA_GET_FORCEGEM_PRICE* ResponseData = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_GET_FORCEGEM_PRICE);
		ResponseData->PoolIndex = 32;
		ResponseData->PriceIndex = 95 + Index;
		ResponseData->Price = (100 - ResponseData->PriceIndex) * MythRebirthPenaltyRef->PenaltyPerMissingLevel;

	}

	SocketSend(Socket, Connection, Response);
}
