#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(TRADE_REQUEST) {
	if (!Character) return;

	RTTradeManagerRequestTrade(Runtime->TradeManager, Character, Packet->TargetIndex);
}

CLIENT_PROCEDURE_BINDING(TRADE_REQUEST_ACCEPT) {
	if (!Character) return;

	S2C_DATA_TRADE_REQUEST_ACCEPT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, TRADE_REQUEST_ACCEPT);
	Response->EventType = RTTradeManagerRequestTradeResponse(Runtime->TradeManager, Character, Packet->EventType);
	SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(TRADE_CLOSE) {
	if (!Character) return;

	S2C_DATA_TRADE_CLOSE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, TRADE_CLOSE);
	Response->EventType = RTTradeManagerCloseTrade(Runtime->TradeManager, Character, Packet->EventType);
	SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(TRADE_ADD_ITEMS) {
	if (!Character) goto error;

	S2C_DATA_TRADE_ADD_ITEMS* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, TRADE_ADD_ITEMS);
	Response->EventType = RTTradeManagerAddItems(
		Runtime->TradeManager,
		Character,
		Packet->InventorySlotCount,
		Packet->FromInventorySlotIndex,
		Packet->ToInventorySlotIndex
	);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(TRADE_ADD_CURRENCY) {
	if (!Character) goto error;

	S2C_DATA_TRADE_ADD_CURRENCY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, TRADE_ADD_CURRENCY);
	Response->EventType = RTTradeManagerAddCurrency(
		Runtime->TradeManager,
		Character,
		Packet->Currency
	);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(TRADE_INVENTORY_SLOT_LIST) {
	if (!Character) goto error;

	Int PacketLength = sizeof(C2S_DATA_TRADE_INVENTORY_SLOT_LIST) + sizeof(UInt16) * Packet->InventorySlotCount;
	if (Packet->Length != PacketLength) goto error;

	if (!RTTradeManagerSetInventorySlots(
		Runtime->TradeManager,
		Character,
		Packet->InventorySlotCount,
		Packet->InventorySlotIndex
	)) {
		RTTradeManagerCloseTrade(
			Runtime->TradeManager,
			Character,
			NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE
		);
	}

	return;

error:
	SocketDisconnect(Socket, Connection);
}
