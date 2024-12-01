#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

CLIENT_PROCEDURE_BINDING(GET_BOOKMARK) {
	if (Client->AccountID < 1) goto error;

	IPC_A2D_DATA_GET_BOOKMARK* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, A2D, GET_BOOKMARK);
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

IPC_PROCEDURE_BINDING(D2A, GET_BOOKMARK) {
	if (!ClientConnection) return;

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);

	S2C_DATA_GET_BOOKMARK* Response = PacketBufferInit(PacketBuffer, S2C, GET_BOOKMARK);
	Response->Result = Packet->Result;
	Response->Count = Packet->Count;

	for (Int Index = 0; Index < Packet->Count; Index += 1) {
		S2C_DATA_GET_BOOKMARK_SLOT* ResponseSlot = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_GET_BOOKMARK_SLOT);
		ResponseSlot->SlotIndex = Packet->Slots[Index].SlotIndex;
		ResponseSlot->CategoryIndex[0] = Packet->Slots[Index].CategoryIndex[0];
		ResponseSlot->CategoryIndex[1] = Packet->Slots[Index].CategoryIndex[1];
		ResponseSlot->CategoryIndex[2] = Packet->Slots[Index].CategoryIndex[2];
		ResponseSlot->CategoryIndex[3] = Packet->Slots[Index].CategoryIndex[3];
		ResponseSlot->CategoryIndex[4] = Packet->Slots[Index].CategoryIndex[4];
		ResponseSlot->SubCategoryIndex[0] = Packet->Slots[Index].SubCategoryIndex[0];
		ResponseSlot->SubCategoryIndex[1] = Packet->Slots[Index].SubCategoryIndex[1];
		CStringCopySafe(ResponseSlot->Description, MAX_AUCTION_BOOKMARK_DESCRIPTION_LENGTH, Packet->Slots[Index].Description);
	}

	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(SET_BOOKMARK) {
	if (Client->AccountID < 1) goto error;

	IPC_A2D_DATA_SET_BOOKMARK* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, A2D, SET_BOOKMARK);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.AuctionSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->Bookmark.SlotIndex = Packet->Bookmark.SlotIndex;
	Request->Bookmark.CategoryIndex[0] = Packet->Bookmark.CategoryIndex[0];
	Request->Bookmark.CategoryIndex[1] = Packet->Bookmark.CategoryIndex[1];
	Request->Bookmark.CategoryIndex[2] = Packet->Bookmark.CategoryIndex[2];
	Request->Bookmark.CategoryIndex[3] = Packet->Bookmark.CategoryIndex[3];
	Request->Bookmark.CategoryIndex[4] = Packet->Bookmark.CategoryIndex[4];
	Request->Bookmark.SubCategoryIndex[0] = Packet->Bookmark.SubCategoryIndex[0];
	Request->Bookmark.SubCategoryIndex[1] = Packet->Bookmark.SubCategoryIndex[1];
	CStringCopySafe(Request->Bookmark.Description, MAX_AUCTION_BOOKMARK_DESCRIPTION_LENGTH, Packet->Bookmark.Description);
	IPCSocketUnicast(Context->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2A, SET_BOOKMARK) {
	if (!ClientConnection) return;

	S2C_DATA_SET_BOOKMARK* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, SET_BOOKMARK);
	Response->Result = Packet->Result;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(DELETE_BOOKMARK) {
	if (Client->AccountID < 1) goto error;

	IPC_A2D_DATA_DELETE_BOOKMARK* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, A2D, DELETE_BOOKMARK);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.AuctionSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->SlotIndex = Packet->SlotIndex;
	IPCSocketUnicast(Context->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2A, DELETE_BOOKMARK) {
	if (!ClientConnection) return;

	S2C_DATA_DELETE_BOOKMARK* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, DELETE_BOOKMARK);
	Response->Result = Packet->Result;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}
