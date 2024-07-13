#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_PRICE_LIST) {
	if (!Character) goto error;

	RTRuntimeDataCostumeWarehousePriceHotReload(Runtime->Context, Context->Config.WorldSvr.RuntimeDataPath, Context->Config.WorldSvr.ServerDataPath, "CostumeWarehouse.xml");

	S2C_DATA_COSTUME_WAREHOUSE_PRICE_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, COSTUME_WAREHOUSE_PRICE_LIST);
	if (Context->Config.Environment.IsCostumeWarehouseEnabled) {
		Response->IsActive = 1;
		Response->ItemCount = Runtime->Context->CostumeWarehousePriceCount;

		for (Int32 Index = 0; Index < Runtime->Context->CostumeWarehousePriceCount; Index += 1) {
			RTDataCostumeWarehousePriceRef Item = &Runtime->Context->CostumeWarehousePriceList[Index];

			S2C_DATA_COSTUME_WAREHOUSE_PRICE_ITEM* ResponseItem = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_COSTUME_WAREHOUSE_PRICE_ITEM);
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
