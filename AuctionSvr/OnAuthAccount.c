#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

CLIENT_PROCEDURE_BINDING(AUTH_ACCOUNT) {
    if (!Client) goto error;
    
	Client->WorldServerIndex = Packet->WorldServerIndex;
	Client->AccountID = -1;
	Client->CharacterIndex = Packet->CharacterIndex;
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(W2A, VERIFY_LINKS) {
	Client = ServerGetClientByIndex(Context, Packet->CharacterIndex, Packet->Header.Source.Index);
	if (!Client) return;
	ClientConnection = Client->Connection;

	Client->AccountID = Packet->AccountID;
	Client->CharacterIndex = Packet->CharacterIndex;

	S2C_DATA_AUTH_ACCOUNT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, AUTH_ACCOUNT);
	SocketSend(Context->ClientSocket, ClientConnection, Response);
	SendEnvironmentNotification(Context, ClientConnection);

	IPC_N2M_DATA_CLIENT_CONNECT* Notification = IPCPacketBufferInit(Socket->PacketBuffer, N2M, CLIENT_CONNECT);
	Notification->Header.Source = Socket->NodeID;
	Notification->Header.Target.Group = Socket->NodeID.Group;
	Notification->Header.Target.Type = IPC_TYPE_MASTER;
	Notification->AccountID = Client->AccountID;
	IPCSocketUnicast(Socket, Notification);
}

IPC_PROCEDURE_BINDING(W2A, DISCONNECT_CLIENT) {
	Client = ServerGetClientByIndex(Context, Packet->CharacterIndex, Packet->Header.Source.Index);
	if (Client) {
		// TODO: Ensure graceful cleanup of client state before 
		SocketDisconnect(Context->ClientSocket, Client->Connection);
	}

	IPC_A2W_DATA_DISCONNECT_CLIENT* Response = IPCPacketBufferInit(Socket->PacketBuffer, A2W, DISCONNECT_CLIENT);
	Response->Header.Source = Socket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Result = 1;
	IPCSocketUnicast(Socket, Response);
}
