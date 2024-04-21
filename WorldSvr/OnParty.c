#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(PARTY_INVITE) {
    if (!Character) goto error;
	if (Packet->NameLength > MAX_CHARACTER_NAME_LENGTH) goto error;

	IPC_W2P_DATA_PARTY_INVITE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_PARTY;
	Request->Source.CharacterIndex = Character->CharacterIndex;
	Request->Source.CharacterID = Character->ID;
	Request->Source.NodeIndex = Context->Config.WorldSvr.NodeIndex;
	Request->Source.CharacterType = 0;
	Request->Source.Level = Character->Info.Basic.Level;
	Request->Source.OverlordLevel = Character->Info.Overlord.Level;
	Request->Source.MythRebirth = 0;
	Request->Source.MythHolyPower = 0;
	Request->Source.MythLevel = 0;
	Request->Source.ForceWingGrade = Character->ForceWingInfo.Grade;
	Request->Source.ForceWingLevel = Character->ForceWingInfo.Level;
	CStringCopySafe(Request->Source.Name, MAX_CHARACTER_NAME_LENGTH + 1, Client->CharacterName);

	Request->Target.CharacterIndex = Packet->CharacterIndex;
	Request->Target.NodeIndex = Packet->WorldServerID;
	CStringCopySafe(Request->Target.Name, MAX_CHARACTER_NAME_LENGTH + 1, Packet->Name);

	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_ACK) {
	ClientContextRef SourceClient = ServerGetClientByIndex(Context, Packet->Source.CharacterIndex, Packet->Source.Name);
	if (!SourceClient) goto error;

	S2C_DATA_PARTY_INVITE* Notification = PacketBufferInit(SourceClient->Connection->PacketBuffer, S2C, PARTY_INVITE);
	Notification->Result = Packet->Success ? 0 : 1;
	Notification->CharacterIndex = Packet->Target.CharacterIndex;
	Notification->WorldServerID = Packet->Target.NodeIndex;
	Notification->CharacterType = Packet->Target.CharacterType;
	Notification->Level = Packet->Target.Level;
	Notification->NameLength = strlen(Packet->Target.Name) + 1;
	CStringCopySafe(Notification->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Target.Name);
	SocketSend(Context->ClientSocket, SourceClient->Connection, Notification);
	return;

error:
	return;
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE) {
	ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->Target.CharacterIndex, Packet->Target.Name);
	if (!TargetClient) goto error;

	RTCharacterRef TargetCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->Target.CharacterIndex);
	if (!TargetCharacter) goto error;

	S2C_DATA_NFY_PARTY_INVITE* Notification = PacketBufferInit(TargetClient->Connection->PacketBuffer, S2C, NFY_PARTY_INVITE);
	Notification->CharacterIndex = Packet->Source.CharacterIndex;
	Notification->WorldServerID = Packet->Source.NodeIndex;
	Notification->CharacterType = Packet->Source.CharacterType;
	Notification->Level = Packet->Source.Level;
	Notification->NameLength = strlen(Packet->Source.Name) + 1;
	CStringCopySafe(Notification->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Source.Name);
	SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);

	IPC_W2P_DATA_PARTY_INVITE_ACK* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE_ACK);
	Response->Header.SourceConnectionID = TargetClient->Connection->ID;
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Response->Header.Target.Type = IPC_TYPE_PARTY;
	Response->Source = Packet->Source;
	Response->Target = Packet->Target;
	Response->Target.Level = TargetCharacter->Info.Basic.Level;
	Response->Target.OverlordLevel = TargetCharacter->Info.Overlord.Level;
	Response->Target.MythRebirth = 0;
	Response->Target.MythHolyPower = 0;
	Response->Target.MythLevel = 0;
	Response->Target.ForceWingGrade = TargetCharacter->ForceWingInfo.Grade;
	Response->Target.ForceWingLevel = TargetCharacter->ForceWingInfo.Level;
	Response->Success = true;
	IPCSocketUnicast(Server->IPCSocket, Response);
	return;

error:
	{
		IPC_W2P_DATA_PARTY_INVITE_ACK* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE_ACK);
		Response->Header.Source = Server->IPCSocket->NodeID;
		Response->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
		Response->Header.Target.Type = IPC_TYPE_PARTY;
		Response->Source = Packet->Source;
		Response->Target = Packet->Target;
		Response->Success = false;
		IPCSocketUnicast(Server->IPCSocket, Response);
	}
}

CLIENT_PROCEDURE_BINDING(PARTY_INVITE_CONFIRM) {
	if (!Character) goto error;

	IPC_W2P_DATA_PARTY_INVITE_CONFIRM* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE_CONFIRM);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_PARTY;
	Request->IsAccept = Packet->IsAccept;
	Request->SourceCharacterIndex = Character->CharacterIndex;
	Request->SourceNodeIndex = Context->Config.WorldSvr.NodeIndex;
	Request->TargetCharacterIndex = Packet->CharacterIndex;
	Request->TargetNodeIndex = Packet->WorldServerID;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	return SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_CONFIRM) {
	ClientContextRef SourceClient = ServerGetClientByIndex(Context, Packet->SourceCharacterIndex, NULL);
	if (!SourceClient) return;

	S2C_DATA_PARTY_INVITE_CONFIRM* Response = PacketBufferInit(SourceClient->Connection->PacketBuffer, S2C, PARTY_INVITE_CONFIRM);
	Response->Result = Packet->Success ? 0 : 1;
	SocketSend(Context->ClientSocket, SourceClient->Connection, Response);
}

CLIENT_PROCEDURE_BINDING(PARTY_INVITE_CANCEL) {
	if (!Character) goto error;

	IPC_W2P_DATA_PARTY_INVITE_CANCEL* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE_CANCEL);
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.SourceConnectionID = Client->Connection->ID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_PARTY;
	Request->SourceCharacterIndex = Character->CharacterIndex;
	Request->SourceNodeIndex = Context->Config.WorldSvr.NodeIndex;
	Request->TargetCharacterIndex = Packet->CharacterIndex;
	Request->TargetNodeIndex = Packet->WorldServerID;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	return SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_CANCEL) {
	if (!ClientConnection) return;

	S2C_DATA_PARTY_INVITE_CANCEL* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, PARTY_INVITE_CANCEL);
	SocketSend(Context->ClientSocket, ClientConnection, Response);
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

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_TIMEOUT) {
	ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->CharacterIndex, NULL);
	if (!TargetClient) return;

	if (Packet->IsCancel) {
		S2C_DATA_NFY_PARTY_INVITE_CANCEL* Notification = PacketBufferInit(TargetClient->Connection->PacketBuffer, S2C, NFY_PARTY_INVITE_CANCEL);
		SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);
	}
	else {
		S2C_DATA_NFY_PARTY_INVITE_TIMEOUT* Notification = PacketBufferInit(TargetClient->Connection->PacketBuffer, S2C, NFY_PARTY_INVITE_TIMEOUT);
		Notification->IsAccept = Packet->IsAccept;
		SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);
	}
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INFO) {
	for (Int32 Index = 0; Index < Packet->Party.MemberCount; Index += 1) {
		RTPartySlotRef Slot = &Packet->Party.Members[Index];

		ClientContextRef Client = ServerGetClientByIndex(Context, Slot->CharacterIndex, NULL);
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
			Member->NameLength = strlen(Packet->Party.Members[MemberIndex].Name) + 1;
			CStringCopySafe(Member->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Party.Members[MemberIndex].Name);
		}

		SocketSend(Context->ClientSocket, Client->Connection, Notification);
	}
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INIT) {
	if (!ClientConnection) return;

	S2C_DATA_NFY_PARTY_INIT* Notification = PacketBufferInit(ClientConnection->PacketBuffer, S2C, NFY_PARTY_INIT);
	Notification->Result = Packet->Result;
	Notification->DungeonIndex = Packet->DungeonIndex;
	Notification->Unknown1 = 0;
	Notification->PartyID = Packet->PartyID;
	Notification->PartyLeaderIndex = Packet->PartyLeaderIndex;
	Notification->Unknown2 = 0;
	Notification->Unknown3 = 0;
	Notification->WorldServerIndex = Packet->WorldServerIndex;
	Notification->Unknown4 = 0;
	Notification->MemberCount = Packet->MemberCount;

	for (Index Index = 0; Index < Packet->MemberCount; Index += 1) {
		Notification->Members[Index].CharacterIndex = Packet->Members[Index].CharacterIndex;
		Notification->Members[Index].Level = Packet->Members[Index].Level;
		Notification->Members[Index].Unknown1 = 0;
		Notification->Members[Index].Unknown2 = 0;
		Notification->Members[Index].Unknown3 = 0;
		Notification->Members[Index].Unknown4 = 0;
		Notification->Members[Index].Unknown5 = 0;
		Notification->Members[Index].OverlordLevel = Packet->Members[Index].OverlordLevel;
		Notification->Members[Index].MythRebirth = Packet->Members[Index].MythRebirth;
		Notification->Members[Index].MythHolyPower = Packet->Members[Index].MythHolyPower;
		Notification->Members[Index].MythLevel = Packet->Members[Index].MythLevel;
		Notification->Members[Index].ForceWingGrade = Packet->Members[Index].ForceWingGrade;
		Notification->Members[Index].ForceWingLevel = Packet->Members[Index].ForceWingLevel;
		Notification->Members[Index].NameLength = strlen(Packet->Members[Index].Name) + 1;
		CStringCopySafe(Notification->Members[Index].Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Members[Index].Name);
//		Notification->Members[Index].Unknown6[36]
	}

	// Notification->Padding[100] = 0;

	Notification->Unknown5 = 0;
	Notification->SoloDungeonIndex = Packet->SoloDungeonIndex;
	Notification->SoloDungeonTimeout = Packet->SoloDungeonTimeout;
	Notification->Unknown6[0] = 0;
	Notification->Unknown6[1] = 0;
	Notification->Unknown6[2] = 0;
	SocketSend(Context->ClientSocket, ClientConnection, Notification);
}