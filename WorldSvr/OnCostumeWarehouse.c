#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_LIST) {
	if (!Character) goto error;

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_COSTUME_WAREHOUSE_LIST* Response = PacketBufferInit(PacketBuffer, S2C, COSTUME_WAREHOUSE_LIST);
	if (Context->Config.Environment.IsCostumeWarehouseEnabled) {
		Response->IsActive = 1;
		Response->ItemCount = Runtime->Context->CostumeWarehousePriceCount;

		for (Int Index = 0; Index < Runtime->Context->CostumeWarehousePriceCount; Index += 1) {
			RTDataCostumeWarehousePriceRef Item = &Runtime->Context->CostumeWarehousePriceList[Index];

			S2C_DATA_COSTUME_WAREHOUSE_ITEM* ResponseItem = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_COSTUME_WAREHOUSE_ITEM);
			ResponseItem->ItemID = Item->ItemID;
			ResponseItem->Price1 = Item->Price1;
			ResponseItem->Price2 = Item->Price2;
			ResponseItem->Price3 = Item->Price3;
		}
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_NAME_LIST) {
	if (!Character) goto error;

	S2C_DATA_COSTUME_WAREHOUSE_NAME_LIST* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, COSTUME_WAREHOUSE_NAME_LIST);
	Response->ItemCount = 0;

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_NAME_SET) {
	if (!Character) goto error;

	S2C_DATA_COSTUME_WAREHOUSE_NAME_SET* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, COSTUME_WAREHOUSE_NAME_SET);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}