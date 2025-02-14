#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

CLIENT_PROCEDURE_BINDING(VIEW_EQUIPMENT_REQUEST)
{
    if (!Client) goto error;

	// TODO: Implement settings saving from client packet allow view equipment
	//if (Client->Settings->AllowViewEq == false) {
	//	// send 00 result packet back to client to say blocked.
	//	return;
	//}

	IPC_C2D_DATA_GET_CHARACTER_VIEW_EQUIPMENT* Request = IPCPacketBufferInit(Context->IPCSocket->PacketBuffer, C2D, GET_CHARACTER_VIEW_EQUIPMENT);
	Request->Header.SourceConnectionID = Connection->ID;
	Info("NodeID: Index=%d, Group=%d, Type=%d",
		Context->IPCSocket->NodeID.Index,
		Context->IPCSocket->NodeID.Group,
		Context->IPCSocket->NodeID.Type);
	Request->Header.Source = Context->IPCSocket->NodeID;
	Info("NodeID: Index=%d, Group=%d, Type=%d",
		Context->IPCSocket->NodeID.Index,
		Context->IPCSocket->NodeID.Group,
		Context->IPCSocket->NodeID.Type);
	Request->Header.Target.Group = Context->Config.ChatSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->CharacterIndex = Client->CharacterIndex;
	memcpy(Request->CharacterName, Packet->TargetName, 17);
	Info("Sending packet to MasterDBServer for vieweq");
	IPCSocketUnicast(Context->IPCSocket, Request);
	return;
error:
	Warn("dc on first packet");
    //SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2C, GET_CHARACTER_VIEW_EQUIPMENT)
{
	Info("ChatSvr got packet from MasterDBServer for vieweq 1");
	if (!Packet) {
		Info("Error: Received null packet on ChatSvr");
		return;
	}
	Info("Packet Details: Success: %d, Level: %d, Style: %d, EquipmentSlotCount: %d",
		Packet->Success, Packet->Level, Packet->Style, Packet->EquipmentSlotCount);
	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);
	S2C_DATA_VIEW_EQUIPMENT_RESPONSE* Response = PacketBufferInit(PacketBuffer, S2C, VIEW_EQUIPMENT_RESPONSE);
	Response->Result = 0x00;
	Response->Level = Packet->Level;
	Response->Class = Packet->Style;
	memcpy(Response->CharacterName, Packet->CharacterName, 17);
	Response->ItemAmount = Packet->EquipmentSlotCount;
	Info("Size of Equipment data from packet: %d", sizeof(struct _RTItemSlot) * Packet->EquipmentSlotCount);
	PacketBufferAppendCopy(PacketBuffer, &Packet->EquipmentData[0], sizeof(struct _RTItemSlot) * Packet->EquipmentSlotCount);
	Info("ChatSvr got packet from MasterDBServer for vieweq");
	ClientContextRef Client2 = ServerGetClientByIndex(Context, Packet->RequestorCharacterID);
	if (!Client2) {
		Info("Error: Could not find client by index %d", Packet->RequestorCharacterID);
		return;
	}
	SocketSend(Context->ClientSocket, Client2->Connection, Response);
	return;
error:
	Info("Error occurred on chat server when receiving packet from MasterDBServer for vieweq");
	SocketDisconnect(Server->ClientSocket, Client2->Connection);
}