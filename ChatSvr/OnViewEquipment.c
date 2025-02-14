#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

CLIENT_PROCEDURE_BINDING(VIEW_EQUIPMENT_REQUEST)
{
    if (!Client) goto error;

	IPC_C2D_DATA_GET_CHARACTER_VIEW_EQUIPMENT* Request = IPCPacketBufferInit(Context->IPCSocket->PacketBuffer, C2D, GET_CHARACTER_VIEW_EQUIPMENT);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Context->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.ChatSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->CharacterIndex = Client->CharacterIndex;
	memcpy(Request->CharacterName, Packet->TargetName, 17);
	IPCSocketUnicast(Context->IPCSocket, Request);
	return;
error:
	Warn("Error occurred when sending packet to MasterDBServer for vieweq");
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2C, GET_CHARACTER_VIEW_EQUIPMENT)
{
	if (!Packet) {
		Info("Error: Received null packet on ChatSvr");
		return;
	}
	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);
	S2C_DATA_VIEW_EQUIPMENT_RESPONSE* Response = PacketBufferInit(PacketBuffer, S2C, VIEW_EQUIPMENT_RESPONSE);
	UInt8 OptionsData[RUNTIME_MAX_SETTINGS_DATA_LENGTH] = { 0 };
	memcpy(OptionsData, Packet->OptionsData, RUNTIME_MAX_SETTINGS_DATA_LENGTH);
	bool AllowViewEq = true;
	if (OptionsData[179] == 0) 
	{
		AllowViewEq = false;
	}
	if (AllowViewEq == false) 
	{
		Response->Result = 0x02;
		Response->Level = Packet->Level;
		Response->Class = Packet->Style;
		memcpy(Response->CharacterName, Packet->CharacterName, 17);
		ClientContextRef Client2 = ServerGetClientByIndex(Context, Packet->RequestorCharacterID);
		if (!Client2) {
			Info("Error: Could not find client by index %d", Packet->RequestorCharacterID);
			return;
		}
		SocketSend(Context->ClientSocket, Client2->Connection, Response);
		return;
	}
	Response->Result = 0x00;
	Response->Level = Packet->Level;
	Response->Class = Packet->Style;
	memcpy(Response->CharacterName, Packet->CharacterName, 17);
	Response->ItemAmount = Packet->EquipmentSlotCount;
	
	PacketBufferAppendCopy(PacketBuffer, &Packet->EquipmentData[0], sizeof(struct _RTItemSlot) * Packet->EquipmentSlotCount);
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