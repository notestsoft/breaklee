#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SET_CHARACTER_SLOT_ORDER) {
	if (Client->AccountID < 1) goto error;

	Client->AccountInfo.CharacterSlotOrder = Packet->CharacterSlotOrder;

	IPC_W2D_DATA_SET_CHARACTER_SLOT_ORDER* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, SET_CHARACTER_SLOT_ORDER);
	Request->Header.SourceConnectionID = Client->Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->CharacterSlotOrder = Packet->CharacterSlotOrder;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, SET_CHARACTER_SLOT_ORDER) {
	if (!ClientConnection) return;

	S2C_DATA_SET_CHARACTER_SLOT_ORDER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, SET_CHARACTER_SLOT_ORDER);
	Response->Result = Packet->Result;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}
