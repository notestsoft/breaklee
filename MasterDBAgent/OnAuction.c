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
	Response->Result = (Handle) ? 0 : 1;

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
	Response->Result = 0;

	UInt8* Memory = &Packet->Data[0];
	ReadMemory(struct _RTItemSlot, MarketInventorySlots, Packet->ItemCount);
	ReadMemory(struct _RTItemSlot, InventorySlots, Packet->InventoryInfo.SlotCount);

	if (!DatabaseCallProcedure(
		Context->Database,
		"InsertAuctionItem",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT32(Packet->CharacterIndex),
		DB_INPUT_UINT8(Packet->SlotIndex),
		DB_INPUT_UINT64(MarketInventorySlots[0].Item.Serial),
		DB_INPUT_UINT64(MarketInventorySlots[0].ItemOptions),
		DB_INPUT_UINT32(MarketInventorySlots[0].ItemDuration.Serial),
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
		Response->Result = 1;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(A2D, SEARCH) {
	IPC_D2A_DATA_SEARCH* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2A, SEARCH);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
		Context->Database,
		"SearchAuctionItems",
		DB_INPUT_UINT8(Packet->CategoryIndex2),
		DB_INPUT_UINT16(Packet->CategoryIndex3),
		DB_INPUT_UINT8(Packet->CategoryIndex4),
		DB_INPUT_UINT16(Packet->CategoryIndex5),
		DB_INPUT_UINT16(Packet->SortOrder),
		DB_PARAM_END
	);

	IPC_DATA_SEARCH_RESULT_SLOT ItemSlot = { 0 };
	while (DatabaseHandleReadNext(
		Context->Database,
		Handle,
		DB_TYPE_UINT64, &ItemSlot.ItemID,
		DB_TYPE_UINT64, &ItemSlot.ItemOptions,
		DB_TYPE_UINT32, &ItemSlot.ItemOptionExtended,
		DB_TYPE_INT16, &ItemSlot.StackSize,
		DB_TYPE_UINT64, &ItemSlot.Price,
		DB_TYPE_UINT32, &ItemSlot.AccountID,
		DB_TYPE_STRING, &ItemSlot.CharacterName[0], sizeof(ItemSlot.CharacterName),
		DB_PARAM_END
	)) {
		IPCPacketBufferAppendCopy(Connection->PacketBuffer, &ItemSlot, sizeof(IPC_DATA_SEARCH_RESULT_SLOT));
		Response->ResultCount += 1;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, AUCTION_BUY_ITEM) {
	IPC_D2W_DATA_AUCTION_BUY_ITEM* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, AUCTION_BUY_ITEM);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Result = 0;

	if (!DatabaseCallProcedure(
		Context->Database,
		"BuyAuctionItem",
		DB_INPUT_INT32(Packet->ItemAccountID),
		DB_INPUT_UINT64(Packet->ItemID),
		DB_INPUT_UINT64(Packet->ItemOptions),
		DB_INPUT_UINT32(Packet->ItemOptionExtended),
		DB_INPUT_UINT64(Packet->ItemPrice),
		DB_INPUT_UINT16(Packet->InventorySlotCount),
		DB_OUTPUT_UINT8(Response->Result),
		DB_PARAM_END
	)) {
		Response->Result = 1;
	}

	if (!Response->Result) {
		Response->ItemID = Packet->ItemID;
		Response->ItemOptions = Packet->ItemOptions;
		Response->ItemOptionExtended = Packet->ItemOptionExtended;
		Response->ItemPriceType = Packet->ItemPriceType;
		Response->ItemPrice = Packet->ItemPrice;
		Response->InventorySlotCount = Packet->InventorySlotCount;
		IPCPacketBufferAppendCopy(Connection->PacketBuffer, &Packet->InventorySlotIndex[0], Packet->InventorySlotCount * sizeof(UInt16));
	}

	IPCSocketUnicast(Socket, Response);

	// TODO: Send notification to other character
}

IPC_PROCEDURE_BINDING(W2D, AUCTION_PROCEED_ITEM) {
	IPC_D2W_DATA_AUCTION_PROCEED_ITEM* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, AUCTION_PROCEED_ITEM);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Result = 0;

	Int32 CommissionRate = Packet->IsCommissionFree ? 0 : Context->Config.AuctionSvr.CommissionRateForSell;

	if (!DatabaseCallProcedure(
		Context->Database,
		"ProceedAuctionItem",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT8(Packet->SlotIndex),
		DB_INPUT_INT32(CommissionRate),
		DB_OUTPUT_UINT8(Response->Result),
		DB_OUTPUT_INT32(Response->SoldItemCount),
		DB_OUTPUT_INT64(Response->ReceivedAlz),
		DB_PARAM_END
	)) {
		Response->Result = 1;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, AUCTION_UNREGISTER_ITEM) {
	IPC_D2W_DATA_AUCTION_UNREGISTER_ITEM* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, AUCTION_UNREGISTER_ITEM);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Result = 0;

	if (!DatabaseCallProcedure(
		Context->Database,
		"DeleteAuctionItem",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT8(Packet->SlotIndex),
		DB_INPUT_INT16(Packet->InventorySlotCount),
		DB_OUTPUT_UINT8(Response->Result),
		DB_OUTPUT_UINT64(Response->ItemSlot.Item.Serial),
		DB_OUTPUT_UINT64(Response->ItemSlot.ItemOptions),
		DB_OUTPUT_UINT32(Response->ItemSlot.ItemDuration.Serial),
		DB_OUTPUT_INT16(Response->InventorySlotCount),
		DB_PARAM_END
	)) {
		Response->Result = 1;
	}

	if (Response->Result == 0) {
		assert(Response->InventorySlotCount <= Packet->InventorySlotCount);
		IPCPacketBufferAppendCopy(Connection->PacketBuffer, &Packet->InventorySlotIndex[0], Response->InventorySlotCount);
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, AUCTION_UPDATE_ITEM) {
	IPC_D2W_DATA_AUCTION_UPDATE_ITEM* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, AUCTION_UPDATE_ITEM);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Result = 0;
	Response->IsDecrease = Packet->IsDecrease;

	if (Packet->IsDecrease) {
		IPC_W2D_DATA_AUCTION_UPDATE_ITEM_DECREASE* PacketPayload = (IPC_W2D_DATA_AUCTION_UPDATE_ITEM_DECREASE*)&Packet->Data[0];
		IPC_D2W_DATA_AUCTION_UPDATE_ITEM_DECREASE* ResponsePayload = IPCPacketBufferAppendStruct(Connection->PacketBuffer, IPC_D2W_DATA_AUCTION_UPDATE_ITEM_DECREASE);
		ResponsePayload->InventorySlotCount = PacketPayload->InventorySlotCount;

		if (!DatabaseCallProcedure(
			Context->Database,
			"UpdateAuctionItemDecrease",
			DB_INPUT_INT32(Packet->AccountID),
			DB_INPUT_INT32(Packet->CharacterIndex),
			DB_INPUT_INT8(Packet->SlotIndex),
			DB_INPUT_INT8(Packet->ItemPriceType),
			DB_INPUT_UINT64(Packet->ItemPrice),
			DB_INPUT_UINT16(Packet->ItemCount),
			DB_INPUT_UINT16(PacketPayload->InventorySlotCount),
			DB_INPUT_UINT64(Packet->ExpirationTime),
			DB_OUTPUT_UINT8(Response->Result),
			DB_OUTPUT_UINT64(Response->ExpirationDate),
			DB_OUTPUT_UINT64(ResponsePayload->ItemSlot.Item.Serial),
			DB_OUTPUT_UINT64(ResponsePayload->ItemSlot.ItemOptions),
			DB_OUTPUT_UINT32(ResponsePayload->ItemSlot.ItemDuration.Serial),
			DB_PARAM_END
		)) {
			Response->Result = 1;
		}

		if (Response->Result == 0) {
			IPCPacketBufferAppendCopy(Connection->PacketBuffer, &PacketPayload->InventorySlotIndex[0], sizeof(UInt16) * PacketPayload->InventorySlotCount);
		}
	}
	else {
		IPC_W2D_DATA_AUCTION_UPDATE_ITEM_INCREASE* PacketPayload = (IPC_W2D_DATA_AUCTION_UPDATE_ITEM_INCREASE*)&Packet->Data[0];
		IPC_D2W_DATA_AUCTION_UPDATE_ITEM_INCREASE* ResponsePayload = IPCPacketBufferAppendStruct(Connection->PacketBuffer, IPC_D2W_DATA_AUCTION_UPDATE_ITEM_INCREASE);

		UInt8* Memory = (UInt8*)&PacketPayload->Data[0];
		ReadMemory(struct _RTItemSlot, MarketInventorySlots, PacketPayload->InventorySlotCount);
		ReadMemory(struct _RTItemSlot, InventorySlots, PacketPayload->InventoryInfo.SlotCount);

		if (!DatabaseCallProcedure(
			Context->Database,
			"UpdateAuctionItemIncrease",
			DB_INPUT_INT32(Packet->AccountID),
			DB_INPUT_INT32(Packet->CharacterIndex),
			DB_INPUT_INT8(Packet->SlotIndex),
			DB_INPUT_UINT64(MarketInventorySlots[0].Item.Serial),
			DB_INPUT_UINT64(MarketInventorySlots[0].ItemOptions),
			DB_INPUT_UINT32(MarketInventorySlots[0].ItemDuration.Serial),
			DB_INPUT_INT8(Packet->ItemPriceType),
			DB_INPUT_UINT64(Packet->ItemPrice),
			DB_INPUT_UINT16(Packet->ItemCount),
			DB_INPUT_UINT64(Packet->ExpirationTime),
			DB_INPUT_UINT16(PacketPayload->InventoryInfo.SlotCount),
			DB_INPUT_DATA(InventorySlots, InventorySlotsLength),
			DB_OUTPUT_UINT8(Response->Result),
			DB_OUTPUT_UINT64(Response->ExpirationDate),
			DB_PARAM_END
		)) {
			Response->Result = 1;
		}
	}

	IPCSocketUnicast(Socket, Response);
}
