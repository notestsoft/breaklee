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
	Request->Source.NodeIndex = Context->Config.WorldSvr.NodeIndex;
	Request->Source.Info.CharacterIndex = Character->CharacterIndex;
	Request->Source.Info.Level = Character->Info.Basic.Level;
	Request->Source.Info.OverlordLevel = Character->Info.Overlord.Level;
	Request->Source.Info.MythRebirth = 0;
	Request->Source.Info.MythHolyPower = 0;
	Request->Source.Info.MythLevel = 0;
	Request->Source.Info.ForceWingGrade = Character->ForceWingInfo.Grade;
	Request->Source.Info.ForceWingLevel = Character->ForceWingInfo.Level;
	CStringCopySafe(Request->Source.Info.Name, MAX_CHARACTER_NAME_LENGTH + 1, Client->CharacterName);

	Request->Target.NodeIndex = Packet->WorldServerID;
	Request->Target.Info.CharacterIndex = Packet->CharacterIndex;
	CStringCopySafe(Request->Target.Info.Name, MAX_CHARACTER_NAME_LENGTH + 1, Packet->Name);

	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_ACK) {
	ClientContextRef SourceClient = ServerGetClientByIndex(Context, Packet->Source.Info.CharacterIndex, Packet->Source.Info.Name);
	if (!SourceClient) goto error;

	S2C_DATA_PARTY_INVITE* Notification = PacketBufferInit(SourceClient->Connection->PacketBuffer, S2C, PARTY_INVITE);
	Notification->Result = Packet->Success ? 0 : 1;
	Notification->WorldServerID = Packet->Target.NodeIndex;
	Notification->CharacterIndex = Packet->Target.Info.CharacterIndex;
	Notification->CharacterType = 0;
	Notification->Level = Packet->Target.Info.Level;
	Notification->NameLength = strlen(Packet->Target.Info.Name) + 1;
	CStringCopySafe(Notification->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Target.Info.Name);
	SocketSend(Context->ClientSocket, SourceClient->Connection, Notification);
	return;

error:
	return;
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE) {
	ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->Target.Info.CharacterIndex, Packet->Target.Info.Name);
	if (!TargetClient) goto error;

	RTCharacterRef TargetCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->Target.Info.CharacterIndex);
	if (!TargetCharacter) goto error;

	S2C_DATA_NFY_PARTY_INVITE* Notification = PacketBufferInit(TargetClient->Connection->PacketBuffer, S2C, NFY_PARTY_INVITE);
	Notification->WorldServerID = Packet->Source.NodeIndex;
	Notification->CharacterIndex = Packet->Source.Info.CharacterIndex;
	Notification->CharacterType = 0;
	Notification->Level = Packet->Source.Info.Level;
	Notification->NameLength = strlen(Packet->Source.Info.Name) + 1;
	CStringCopySafe(Notification->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Source.Info.Name);
	SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);

	IPC_W2P_DATA_PARTY_INVITE_ACK* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE_ACK);
	Response->Header.SourceConnectionID = TargetClient->Connection->ID;
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Response->Header.Target.Type = IPC_TYPE_PARTY;
	Response->Source = Packet->Source;
	Response->Target = Packet->Target;
	Response->Target.Info.Level = TargetCharacter->Info.Basic.Level;
	Response->Target.Info.OverlordLevel = TargetCharacter->Info.Overlord.Level;
	Response->Target.Info.MythRebirth = 0;
	Response->Target.Info.MythHolyPower = 0;
	Response->Target.Info.MythLevel = 0;
	Response->Target.Info.ForceWingGrade = TargetCharacter->ForceWingInfo.Grade;
	Response->Target.Info.ForceWingLevel = TargetCharacter->ForceWingInfo.Level;
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

		ClientContextRef Client = ServerGetClientByIndex(Context, Slot->Info.CharacterIndex, NULL);
		if (!Client) continue;

		// NOTE: This is just some bullshit to make other shit work clean it up...
		RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Client->CharacterIndex);
		if (Character) {
			Character->PartyID = Packet->Party.ID;
		}

		S2C_DATA_NFY_PARTY_INFO* Notification = PacketBufferInit(Client->Connection->PacketBuffer, S2C, NFY_PARTY_INFO);
		Notification->Result = 0;
		Notification->LeaderCharacterIndex = Packet->Party.LeaderCharacterIndex;
		Notification->MemberCount = Packet->Party.MemberCount;

		for (Int32 MemberIndex = 0; MemberIndex < Packet->Party.MemberCount; MemberIndex += 1) {
			S2C_DATA_PARTY_MEMBER* Member = &Notification->Members[MemberIndex];
			Member->CharacterIndex = Packet->Party.Members[MemberIndex].Info.CharacterIndex;
			Member->Level = Packet->Party.Members[MemberIndex].Info.Level;
			Member->OverlordLevel = Packet->Party.Members[MemberIndex].Info.OverlordLevel;
			Member->MythRebirth = Packet->Party.Members[MemberIndex].Info.MythRebirth;
			Member->MythHolyPower = Packet->Party.Members[MemberIndex].Info.MythHolyPower;
			Member->MythLevel = Packet->Party.Members[MemberIndex].Info.MythLevel;
			Member->ForceWingGrade = Packet->Party.Members[MemberIndex].Info.ForceWingGrade;
			Member->ForceWingLevel = Packet->Party.Members[MemberIndex].Info.ForceWingLevel;
			Member->NameLength = strlen(Packet->Party.Members[MemberIndex].Info.Name) + 1;
			CStringCopySafe(Member->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Party.Members[MemberIndex].Info.Name);
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

IPC_PROCEDURE_BINDING(P2W, PARTY_DATA) {
	S2C_DATA_NFY_PARTY_UPDATE* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_PARTY_UPDATE);
	Notification->MemberCount = Packet->MemberCount;

	for (Int32 MemberIndex = 0; MemberIndex < Packet->MemberCount; MemberIndex += 1) {
		S2C_DATA_PARTY_UPDATE_MEMBER* Member = &Notification->Members[MemberIndex];
		Member->Info.CharacterIndex = Packet->MemberInfo[MemberIndex].CharacterIndex;
		Member->Info.Level = Packet->MemberInfo[MemberIndex].Level;
		Member->Info.OverlordLevel = Packet->MemberInfo[MemberIndex].OverlordLevel;
		Member->Info.MythRebirth = Packet->MemberInfo[MemberIndex].MythRebirth;
		Member->Info.MythHolyPower = Packet->MemberInfo[MemberIndex].MythHolyPower;
		Member->Info.MythLevel = Packet->MemberInfo[MemberIndex].MythLevel;
		Member->Info.ForceWingGrade = Packet->MemberInfo[MemberIndex].ForceWingGrade;
		Member->Info.ForceWingLevel = Packet->MemberInfo[MemberIndex].ForceWingLevel;
		Member->Info.NameLength = strlen(Packet->MemberInfo[MemberIndex].Name) + 1;
		CStringCopySafe(Member->Info.Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->MemberInfo[MemberIndex].Name);

		Member->Data.MaxHP = Packet->MemberData[MemberIndex].MaxHP;
		Member->Data.CurrentHP = Packet->MemberData[MemberIndex].CurrentHP;
		Member->Data.MaxMP = Packet->MemberData[MemberIndex].MaxMP;
		Member->Data.CurrentMP = Packet->MemberData[MemberIndex].CurrentMP;
		Member->Data.PositionX = Packet->MemberData[MemberIndex].PositionX;
		Member->Data.PositionY = Packet->MemberData[MemberIndex].PositionY;
		Member->Data.MaxSP = Packet->MemberData[MemberIndex].MaxSP;
		Member->Data.CurrentSP = Packet->MemberData[MemberIndex].CurrentSP;

		// TODO: Resolve all unknown fields...
		Member->Info.BattleStyleIndex = 9;
	}

	for (Int32 Index = 0; Index < Packet->MemberCount; Index += 1) {
		ClientContextRef Client = ServerGetClientByIndex(Context, Packet->MemberInfo[Index].CharacterIndex, NULL);
		if (!Client) continue;

		SocketSend(Context->ClientSocket, Client->Connection, Notification);
	}
}