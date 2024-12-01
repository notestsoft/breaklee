#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

CLIENT_PROCEDURE_BINDING(GET_ITEM_LIST) {
	if (Client->AccountID < 1) goto error;

	IPC_A2D_DATA_GET_ITEM_LIST* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, A2D, GET_ITEM_LIST);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.AuctionSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	IPCSocketUnicast(Context->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2A, GET_ITEM_LIST) {
	if (!ClientConnection) return;

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);

	S2C_DATA_GET_ITEM_LIST* Response = PacketBufferInit(PacketBuffer, S2C, GET_ITEM_LIST);
	Response->Result = Packet->Result;
	Response->ItemCount = Packet->ItemCount;
	
	for (Int Index = 0; Index < Packet->ItemCount; Index += 1) {
		S2C_DATA_GET_ITEM_LIST_INDEX* ResponseItem = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_GET_ITEM_LIST_INDEX);
		ResponseItem->SlotIndex = Packet->Items[Index].SlotIndex;
		ResponseItem->ItemID = Packet->Items[Index].ItemID;
		ResponseItem->ItemOptions = Packet->Items[Index].ItemOptions;
		ResponseItem->ItemCount = Packet->Items[Index].ItemCount;
		ResponseItem->ItemPrice = Packet->Items[Index].ItemPrice;
		ResponseItem->ItemPriceType = 0;
		ResponseItem->SoldItemCount = Packet->Items[Index].SoldItemCount;
		ResponseItem->ExpirationDate = Packet->Items[Index].ExpirationDate;
		ResponseItem->RegistrationDate = Packet->Items[Index].RegistrationDate;
	}

	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(GET_ITEM_AVERAGE_PRICE) {
	if (Client->AccountID < 1) goto error;

	IPC_A2D_DATA_GET_ITEM_AVERAGE_PRICE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, A2D, GET_ITEM_AVERAGE_PRICE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.AuctionSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->ItemID = Packet->ItemID;
	Request->ItemOptions = Packet->ItemOptions;
	IPCSocketUnicast(Context->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2A, GET_ITEM_AVERAGE_PRICE) {
	if (!ClientConnection) return;

	S2C_DATA_GET_ITEM_AVERAGE_PRICE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, GET_ITEM_AVERAGE_PRICE);
	Response->Price = Packet->Price;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(GET_ITEM_MINIMUM_PRICE) {
	if (Client->AccountID < 1) goto error;

	IPC_A2D_DATA_GET_ITEM_MINIMUM_PRICE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, A2D, GET_ITEM_MINIMUM_PRICE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.AuctionSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->ItemID = Packet->ItemID;
	Request->ItemOptions = Packet->ItemOptions;
	IPCSocketUnicast(Context->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2A, GET_ITEM_MINIMUM_PRICE) {
	if (!ClientConnection) return;

	S2C_DATA_GET_ITEM_MINIMUM_PRICE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, GET_ITEM_MINIMUM_PRICE);
	Response->Result = 0;
	Response->Price = Packet->Price;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}
