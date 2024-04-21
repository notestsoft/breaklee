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

	// Result 14 
	// if (!RTEntityIsNull(TargetCharacter->PartyID)) return;

	/*
	IPC_W2P_DATA_PARTY_INVITE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_PARTY;
	Request->Source.CharacterIndex = Character->CharacterIndex;
	Request->Source.CharacterID = Character->ID;
	Request->Source.WorldServerID = Context->Config.WorldSvr.NodeIndex;
	Request->Source.CharacterType = 0;
	Request->Source.Level = Character->Info.Basic.Level;
	Request->Source.NameLength = strlen(Client->CharacterName);
	CStringCopySafe(Request->Source.Name, MAX_CHARACTER_NAME_LENGTH, Client->CharacterName);

	Request->Target.CharacterIndex = Packet->CharacterIndex;
	Request->Target.WorldServerID = Packet->WorldServerID;
	Request->Target.CharacterType = 0;
	Request->Target.Level = 0;
	Request->Target.NameLength = Packet->NameLength;
	CStringCopySafe(Request->Target.Name, MAX_CHARACTER_NAME_LENGTH, Packet->Name);
	*/

	IPC_W2W_DATA_PARTY_INVITE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2W, PARTY_INVITE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Index = Packet->WorldServerID;
	Request->Header.Target.Type = IPC_TYPE_WORLD;
	Request->Source.CharacterIndex = Character->CharacterIndex;
	Request->Source.CharacterID = Character->ID;
	Request->Source.WorldServerID = Context->Config.WorldSvr.NodeIndex;
	Request->Source.CharacterType = 0;
	Request->Source.Level = Character->Info.Basic.Level;
	Request->Source.NameLength = strlen(Client->CharacterName);
	CStringCopySafe(Request->Source.Name, MAX_CHARACTER_NAME_LENGTH, Client->CharacterName);

	Request->Target.CharacterIndex = Packet->CharacterIndex;
	Request->Target.WorldServerID = Packet->WorldServerID;
	Request->Target.CharacterType = 0;
	Request->Target.Level = 0;
	Request->Target.NameLength = Packet->NameLength;
	CStringCopySafe(Request->Target.Name, MAX_CHARACTER_NAME_LENGTH, Packet->Name);

	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(W2W, PARTY_INVITE) {
	ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->Target.CharacterIndex, Packet->Target.Name, Packet->Target.NameLength);
	if (!TargetClient) return;

	RTCharacterRef TargetCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->Target.CharacterIndex);
	if (!TargetCharacter) return;

	IPC_W2P_DATA_PARTY_INVITE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_PARTY;
	Request->Source = Packet->Source;
	Request->Target = Packet->Target;
	Request->Target.Level = TargetCharacter->Info.Basic.Level;
	IPCSocketUnicast(Server->IPCSocket, Request);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE) {
	if (Packet->Source.WorldServerID == Context->Config.WorldSvr.NodeIndex) {
		ClientContextRef SourceClient = ServerGetClientByIndex(Context, Packet->Source.CharacterIndex, Packet->Source.Name, Packet->Source.NameLength);
		if (!SourceClient) return;

		RTCharacterRef SourceCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->Source.CharacterIndex);
		if (!SourceCharacter) return;
		
		S2C_DATA_PARTY_INVITE* Notification = PacketBufferInit(SourceClient->Connection->PacketBuffer, S2C, PARTY_INVITE);
		Notification->CharacterIndex = Packet->Target.CharacterIndex;
		Notification->WorldServerID = Packet->Target.WorldServerID;
		Notification->CharacterType = Packet->Target.CharacterType;
		Notification->Level = Packet->Target.Level;
		Notification->NameLength = Packet->Target.NameLength;
		memcpy(Notification->Name, Packet->Target.Name, MAX_CHARACTER_NAME_LENGTH);
		SocketSend(Context->ClientSocket, SourceClient->Connection, Notification);
	}

	if (Packet->Target.WorldServerID == Context->Config.WorldSvr.NodeIndex) {
		ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->Target.CharacterIndex, Packet->Target.Name, Packet->Target.NameLength);
		if (!TargetClient) return;

		RTCharacterRef TargetCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->Target.CharacterIndex);
		if (!TargetCharacter) return;

		S2C_DATA_NFY_PARTY_INVITE* Notification = PacketBufferInit(TargetClient->Connection->PacketBuffer, S2C, NFY_PARTY_INVITE);
		Notification->CharacterIndex = Packet->Source.CharacterIndex;
		Notification->WorldServerID = Packet->Source.WorldServerID;
		Notification->CharacterType = Packet->Source.CharacterType;
		Notification->Level = Packet->Source.Level;
		Notification->NameLength = Packet->Source.NameLength;
		memcpy(Notification->Name, Packet->Source.Name, MAX_CHARACTER_NAME_LENGTH);
		SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);
	}
}

CLIENT_PROCEDURE_BINDING(PARTY_INVITE_CONFIRM) {
	if (!Character) goto error;

	IPC_W2P_DATA_PARTY_INVITE_CONFIRM* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE_CONFIRM);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_PARTY;
	Request->CharacterIndex = Character->CharacterIndex;
	Request->CharacterID = Character->ID;
	Request->WorldServerID = Context->Config.WorldSvr.NodeIndex;
	Request->TargetCharacterIndex = Packet->CharacterIndex;
	Request->TargetWorldServerID = Packet->WorldServerID;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	return SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_CONFIRM) {
	if (Packet->WorldServerID == Context->Config.WorldSvr.NodeIndex) {
		ClientContextRef SourceClient = ServerGetClientByIndex(Context, Packet->CharacterIndex, NULL, 0);
		if (!SourceClient) return;

		RTCharacterRef SourceCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->CharacterIndex);
		if (!SourceCharacter) return;

		S2C_DATA_PARTY_INVITE_CONFIRM* Response = PacketBufferInit(SourceClient->Connection->PacketBuffer, S2C, PARTY_INVITE_CONFIRM);
		Response->Result = (Packet->Success) ? 0 : 14;
		SocketSend(Context->ClientSocket, SourceClient->Connection, Response);
	}
	/*
	if (Packet->TargetWorldServerID == Context->Config.WorldSvr.NodeIndex) {
		ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->TargetCharacterIndex, NULL, 0);
		if (!TargetClient) return;

		RTCharacterRef TargetCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->TargetCharacterIndex);
		if (!TargetCharacter) return;


	}

	S2C_DATA_PARTY_INVITE_CONFIRM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_INVITE_CONFIRM);
	return SocketSend(Socket, Connection, Response);
	*/
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

IPC_PROCEDURE_BINDING(P2W, PARTY_INFO) {
	for (Int32 Index = 0; Index < Packet->Party.MemberCount; Index += 1) {
		RTPartySlotRef Slot = &Packet->Party.Members[Index];
		if (RTEntityIsNull(Slot->MemberID)) continue;

		ClientContextRef Client = ServerGetClientByIndex(Context, Slot->CharacterIndex, NULL, 0);
		if (!Client) continue;

		S2C_DATA_NFY_PARTY_INFO* Notification = PacketBufferInit(Client->Connection->PacketBuffer, S2C, NFY_PARTY_INFO);
		Notification->Result = 0;
		Notification->LeaderCharacterIndex = Packet->Party.LeaderCharacterIndex;
		Notification->MemberCount = Packet->Party.MemberCount;

		for (Int32 MemberIndex = 0; MemberIndex < Packet->Party.MemberCount; MemberIndex += 1) {
			S2C_DATA_PARTY_MEMBER* Member = &Notification->Members[MemberIndex];
			Member->CharacterIndex = Packet->Party.Members[MemberIndex].CharacterIndex;
			Member->Level = Packet->Party.Members[MemberIndex].Level;
			Member->OverlordLevel = Packet->Party.Members[MemberIndex].OverlordLevel;
			Member->MythRebirth = Packet->Party.Members[MemberIndex].MythRebirth;
			Member->MythHolyPower = Packet->Party.Members[MemberIndex].MythHolyPower;
			Member->MythLevel = Packet->Party.Members[MemberIndex].MythLevel;
			Member->ForceWingGrade = Packet->Party.Members[MemberIndex].ForceWingGrade;
			Member->ForceWingLevel = Packet->Party.Members[MemberIndex].ForceWingLevel;
			Member->NameLength = Packet->Party.Members[MemberIndex].NameLength;
			CStringCopySafe(Member->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH, Packet->Party.Members[MemberIndex].Name);
		}

		SocketSend(Context->ClientSocket, Client->Connection, Notification);
	}
}