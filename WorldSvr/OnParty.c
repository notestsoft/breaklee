#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(PARTY_INVITE) {
    if (!Character) goto error;
	if (!RTEntityIsNull(Character->PartyID)) goto error;
	if (Packet->NameLength > MAX_CHARACTER_NAME_LENGTH) goto error;

	IPC_W2P_DATA_PARTY_INVITE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_PARTY;
	Request->Source.CharacterIndex = Character->CharacterIndex;
	Request->Source.WorldServerID = Context->Config.WorldSvr.NodeIndex;
	Request->Source.CharacterType = 0;
	Request->Source.Level = Character->Info.Basic.Level;
	Request->Source.NameLength = strlen(Client->CharacterName);
	memcpy(Request->Source.Name, Client->CharacterName, MAX_CHARACTER_NAME_LENGTH);

	Request->Target.CharacterIndex = Packet->CharacterIndex;
	Request->Target.WorldServerID = Packet->WorldServerID;
	Request->Target.CharacterType = 0;
	Request->Target.Level = 0;
	Request->Target.NameLength = Packet->NameLength;
	memcpy(Request->Target.Name, Packet->Name, MAX_CHARACTER_NAME_LENGTH);

	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE) {
	ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->Target.CharacterIndex, Packet->Target.Name, Packet->Target.NameLength);
	if (!TargetClient) return;

	RTCharacterRef TargetCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->Target.CharacterIndex);
	if (!TargetCharacter) return;

	// Result 14 
	if (!RTEntityIsNull(TargetCharacter->PartyID)) return;

	IPC_DATA_WORLD_ACKPARTYINVITE* Response = PacketBufferInitExtended(Context->MasterSocket->PacketBuffer, IPC, WORLD_ACKPARTYINVITE);
	Response->Success = true;
	Response->Source.CharacterIndex = Packet->Source.CharacterIndex;
	Response->Source.WorldServerID = Packet->Source.WorldServerID;
	Response->Source.CharacterType = Packet->Source.CharacterType;
	Response->Source.Level = Packet->Source.Level;
	Response->Source.NameLength = Packet->Source.NameLength;
	memcpy(Response->Source.Name, Packet->Source.Name, MAX_CHARACTER_NAME_LENGTH);

	Response->Target.CharacterIndex = TargetCharacter->CharacterIndex;
	Response->Target.WorldServerID = Context->Config.WorldSvr.WorldID;
	Response->Target.CharacterType = 0;
	Response->Target.Level = TargetCharacter->Info.Basic.Level;
	Response->Target.NameLength = strlen(TargetClient->CharacterName);
	memcpy(Response->Target.Name, TargetClient->CharacterName, MAX_CHARACTER_NAME_LENGTH);
	SocketSendAll(Context->MasterSocket, Response);
	
	S2C_DATA_NFY_PARTY_INVITE* Notification = PacketBufferInit(TargetClient->Connection->PacketBuffer, S2C, NFY_PARTY_INVITE);
	Notification->CharacterIndex = Packet->Source.CharacterIndex;
	Notification->WorldServerID = Packet->Source.WorldServerID;
	Notification->CharacterType = Packet->Source.CharacterType;
	Notification->Level = Packet->Source.Level;
	Notification->NameLength = Packet->Source.NameLength;
	memcpy(Notification->Name, Packet->Source.Name, MAX_CHARACTER_NAME_LENGTH);
	SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);
}

IPC_PROCEDURE_BINDING(OnWorldRespondPartyInvite, IPC_WORLD_ACKPARTYINVITE, IPC_DATA_WORLD_ACKPARTYINVITE) {
	if (!ClientConnection || !Client) return;

	S2C_DATA_PARTY_INVITE* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, PARTY_INVITE);
	Response->CharacterIndex = Packet->Target.CharacterIndex;
	Response->WorldServerID = Packet->Target.WorldServerID;
	Response->NameLength = Packet->Target.NameLength;
	memcpy(Response->Name, Packet->Target.Name, MAX_CHARACTER_NAME_LENGTH);
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(PARTY_INVITE_CONFIRM) {
	if (!Character) goto error;

	// TODO: Implementation missing

	S2C_DATA_PARTY_INVITE_CONFIRM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_INVITE_CONFIRM);
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PARTY_INVITE_CANCEL) {
	if (!Character) goto error;

	// TODO: Implementation missing

	S2C_DATA_PARTY_INVITE_CANCEL* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_INVITE_CANCEL);
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PARTY_LEAVE) {
	if (!Character) goto error;

	// TODO: Implementation missing

	S2C_DATA_PARTY_LEAVE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_LEAVE);
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PARTY_EXPEL_MEMBER) {
	if (!Character) goto error;

	// TODO: Implementation missing

	S2C_DATA_PARTY_EXPEL_MEMBER* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_EXPEL_MEMBER);
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PARTY_CHANGE_LEADER) {
	if (!Character) goto error;

	// TODO: Implementation missing

	S2C_DATA_PARTY_CHANGE_LEADER* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_CHANGE_LEADER);
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
