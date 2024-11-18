#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
/*

CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_REGISTER, 2249, 13133,
	C2S_DATA_SIGNATURE;
UInt32 RequestCode;
Int32 InventorySlotCount;
C2S_REQUEST_CRAFT_INVENTORY_SLOT InventorySlots[0];
// UInt8 Unknown2[4168]; - Maybe AuthCaptcha
)

*/
CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT_REGISTER) {
	S2C_DATA_REQUEST_CRAFT_REGISTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_REGISTER);
	Response->Result = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

/*

CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_START, 2250, 13133,
	C2S_DATA_SIGNATURE;
UInt32 RequestCode;
UInt8 RequestSlotIndex;
Int32 Unknown1; // - Maybe AuthCaptcha
Int32 InventorySlotCount;
C2S_REQUEST_CRAFT_INVENTORY_SLOT InventorySlots[0];
// UInt8 Unknown2[4168]; - Maybe AuthCaptcha
)
*/

CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT_START) {
	S2C_DATA_REQUEST_CRAFT_START* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_START);
	Response->Result = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
/*
CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_END, 2251, 13133,
    C2S_DATA_SIGNATURE;
UInt32 RequestCode;
UInt8 RequestSlotIndex;
Int32 Unknown1; // - Maybe AuthCaptcha
Int32 InventorySlotCount;
C2S_REQUEST_CRAFT_INVENTORY_SLOT InventorySlots[0];
// UInt8 Unknown2[4168]; - Maybe AuthCaptcha
)
*/
CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT_END) {
	S2C_DATA_REQUEST_CRAFT_END* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REQUEST_CRAFT_END);
	Response->Result = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
/*
CLIENT_PROTOCOL(S2C, REQUEST_CRAFT_UPDATE, 2252, 13133,
    S2C_DATA_SIGNATURE;
UInt32 ItemID;
UInt8 RequestSlotIndex;
UInt32 RequestCode;
UInt32 Amity;
UInt8 Success;
)
*/
