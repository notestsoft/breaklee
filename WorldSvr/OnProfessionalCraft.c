#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(PROF_CRAFT_REGISTER)
{
	if (!Character) goto error;

	if (!RTCharacterIsCraftSlotEmpty(Runtime, Character, Packet->Slot)) goto error;

	if (RTCharacterIsCraftCategoryRegistered(Runtime, Character, Packet->Category)) goto error;
	S2C_DATA_PROF_CRAFT_REGISTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PROF_CRAFT_REGISTER);
	if (!RTCharacterSetCraftSlot(Runtime, Character, Packet->Slot, Packet->Category, Packet->ItemInventorySlot)) {
		Response->Result = 0;
	}
	else {
		Response->Result = 1;
	}
	
	SocketSend(Socket, Connection, Response);
	return;

error:
	Error("Error occurred when registering a professional craft.");
	SocketDisconnect(Socket, Connection);
}