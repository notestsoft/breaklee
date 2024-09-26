#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(A2D, GET_BOOKMARK) {
	IPC_D2A_DATA_GET_BOOKMARK* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2A, GET_BOOKMARK);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
		Context->Database,
		"GetAuctionBookmarks",
		DB_INPUT_INT32(Packet->AccountID),
		DB_PARAM_END
	);
	Response->Result = (Handle) ? 1 : 0;

	IPC_DATA_GET_BOOKMARK_SLOT BookmarkSlot = { 0 };
	while (DatabaseHandleReadNext(
		Context->Database,
		Handle,
		DB_TYPE_INT8, &BookmarkSlot.SlotIndex,
		DB_TYPE_INT16, &BookmarkSlot.CategoryIndex[0],
		DB_TYPE_INT16, &BookmarkSlot.CategoryIndex[1],
		DB_TYPE_INT16, &BookmarkSlot.CategoryIndex[2],
		DB_TYPE_INT16, &BookmarkSlot.CategoryIndex[3],
		DB_TYPE_INT16, &BookmarkSlot.CategoryIndex[4],
		DB_TYPE_INT8, &BookmarkSlot.SubCategoryIndex[0],
		DB_TYPE_INT8, &BookmarkSlot.SubCategoryIndex[1],
		DB_TYPE_STRING, &BookmarkSlot.Description[0], sizeof(BookmarkSlot.Description),
		DB_PARAM_END
	)) {
		IPCPacketBufferAppendCopy(Connection->PacketBuffer, &BookmarkSlot, sizeof(IPC_DATA_GET_BOOKMARK_SLOT));
		Response->Count += 1;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(A2D, SET_BOOKMARK) {
	IPC_D2A_DATA_SET_BOOKMARK* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2A, SET_BOOKMARK);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Result = false;

	if (!DatabaseCallProcedure(
		Context->Database,
		"SetAuctionBookmark",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT8(Packet->Bookmark.SlotIndex),
		DB_INPUT_INT16(Packet->Bookmark.CategoryIndex[0]),
		DB_INPUT_INT16(Packet->Bookmark.CategoryIndex[1]),
		DB_INPUT_INT16(Packet->Bookmark.CategoryIndex[2]),
		DB_INPUT_INT16(Packet->Bookmark.CategoryIndex[3]),
		DB_INPUT_INT16(Packet->Bookmark.CategoryIndex[4]),
		DB_INPUT_INT8(Packet->Bookmark.SubCategoryIndex[0]),
		DB_INPUT_INT8(Packet->Bookmark.SubCategoryIndex[1]),
		DB_INPUT_STRING(Packet->Bookmark.Description, sizeof(Packet->Bookmark.Description)),
		DB_OUTPUT_INT8(Response->Result),
		DB_PARAM_END
	)) {
		Response->Result = false;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(A2D, DELETE_BOOKMARK) {
	IPC_D2A_DATA_DELETE_BOOKMARK* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2A, DELETE_BOOKMARK);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Result = true;

	if (!DatabaseCallProcedure(
		Context->Database,
		"DeleteAuctionBookmark",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT8(Packet->SlotIndex),
		DB_PARAM_END
	)) {
		Response->Result = false;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(A2D, GET_ITEM_LIST) {
	IPC_D2A_DATA_GET_ITEM_LIST* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2A, GET_ITEM_LIST);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
		Context->Database,
		"GetAuctionItems",
		DB_INPUT_INT32(Packet->AccountID),
		DB_PARAM_END
	);
	Response->Result = (Handle) ? 1 : 0;

	IPC_DATA_GET_ITEM_LIST_INDEX ItemSlot = { 0 };
	while (DatabaseHandleReadNext(
		Context->Database,
		Handle,
		DB_TYPE_UINT8, &ItemSlot.SlotIndex,
		DB_TYPE_UINT64, &ItemSlot.ItemID,
		DB_TYPE_UINT64, &ItemSlot.ItemOptions,
		DB_TYPE_UINT64, &ItemSlot.ItemPrice,
		DB_TYPE_INT32, &ItemSlot.ItemCount,
		DB_TYPE_INT32, &ItemSlot.SoldItemCount,
		DB_TYPE_UINT64, &ItemSlot.ExpirationDate,
		DB_TYPE_UINT64, &ItemSlot.RegistrationDate,
		DB_PARAM_END
	)) {
		IPCPacketBufferAppendCopy(Connection->PacketBuffer, &ItemSlot, sizeof(IPC_DATA_GET_ITEM_LIST_INDEX));
		Response->ItemCount += 1;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(A2D, GET_ITEM_AVERAGE_PRICE) {
	IPC_D2A_DATA_GET_ITEM_AVERAGE_PRICE* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2A, GET_ITEM_AVERAGE_PRICE);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	if (!DatabaseCallProcedure(
		Context->Database,
		"GetAverageItemPrice",
		DB_INPUT_UINT64(Packet->ItemID),
		DB_INPUT_UINT64(Packet->ItemOptions),
		DB_OUTPUT_UINT64(Response->Price),
		DB_PARAM_END
	)) {
		Response->Price = 0;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(A2D, GET_ITEM_MINIMUM_PRICE) {
	IPC_D2A_DATA_GET_ITEM_MINIMUM_PRICE* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2A, GET_ITEM_MINIMUM_PRICE);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	if (!DatabaseCallProcedure(
		Context->Database,
		"GetMinimumItemPrice",
		DB_INPUT_UINT64(Packet->ItemID),
		DB_INPUT_UINT64(Packet->ItemOptions),
		DB_OUTPUT_UINT64(Response->Price),
		DB_PARAM_END
	)) {
		Response->Price = 0;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, AUCTION_REGISTER_ITEM) {
	IPC_D2W_DATA_AUCTION_REGISTER_ITEM* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, AUCTION_REGISTER_ITEM);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	UInt8* Memory = &Packet->Data[0];
	ReadMemory(struct _RTItemSlot, MarketInventorySlots, Packet->ItemCount);
	ReadMemory(struct _RTItemSlot, InventorySlots, Packet->InventoryInfo.SlotCount);

	if (!DatabaseCallProcedure(
		Context->Database,
		"InsertAuctionItem",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT32(Packet->CharacterID),
		DB_INPUT_UINT8(Packet->SlotIndex),
		DB_INPUT_UINT64(MarketInventorySlots[0].Item.Serial),
		DB_INPUT_UINT64(MarketInventorySlots[0].ItemOptions),
		DB_INPUT_UINT32(MarketInventorySlots[0].ItemDuration),
		DB_INPUT_UINT64(Packet->ItemPrice),
		DB_INPUT_UINT16(Packet->ItemCount),
		DB_INPUT_UINT16(Packet->CategoryIndex[0]),
		DB_INPUT_UINT16(Packet->CategoryIndex[1]),
		DB_INPUT_UINT16(Packet->CategoryIndex[2]),
		DB_INPUT_UINT16(Packet->CategoryIndex[3]),
		DB_INPUT_UINT16(Packet->CategoryIndex[4]),
		DB_INPUT_UINT64(Packet->ExpirationTime),
		DB_INPUT_UINT16(Packet->InventoryInfo.SlotCount),
		DB_INPUT_DATA(InventorySlots, InventorySlotsLength),
		DB_OUTPUT_UINT8(Response->Result),
		DB_OUTPUT_UINT64(Response->ExpirationDate),
		DB_PARAM_END
	)) {
		Response->Result = 0;
	}

	IPCSocketUnicast(Socket, Response);
}