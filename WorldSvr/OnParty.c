#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(PARTY_INVITE) {
    if (!Character) goto error;
	if (Packet->NameLength > MAX_CHARACTER_NAME_LENGTH) goto error;
	
	UInt8 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);

	IPC_W2P_DATA_PARTY_INVITE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_PARTY;
	Request->Source.CharacterIndex = (UInt32)Character->CharacterIndex;
	Request->Source.Level = Character->Data.Info.Level;
	Request->Source.DungeonIndex = Character->Data.Info.DungeonIndex;
	Request->Source.Unknown2 = 0;
	Request->Source.WorldServerIndex = Context->Config.WorldSvr.NodeIndex;
	Request->Source.BattleStyleIndex = BattleStyleIndex;
	Request->Source.WorldIndex = Character->Data.Info.WorldIndex;
	Request->Source.OverlordLevel = Character->Data.OverlordMasteryInfo.Info.Level;
	Request->Source.MythRebirth = Character->Data.MythMasteryInfo.Info.Rebirth;
	Request->Source.MythHolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
	Request->Source.MythLevel = Character->Data.MythMasteryInfo.Info.Level;
	Request->Source.ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
	Request->Source.ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
	Request->Source.NameLength = strlen(Character->Name) + 1;
	CStringCopySafe(Request->Source.Name, MAX_CHARACTER_NAME_LENGTH, Character->Name);

	Request->Target.WorldServerIndex = Packet->WorldServerID;
	Request->Target.CharacterIndex = Packet->CharacterIndex;
	Request->Target.NameLength = Packet->NameLength;
	CStringCopySafe(Request->Target.Name, MAX_CHARACTER_NAME_LENGTH, Packet->Name);

	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_ACK) {
	ClientContextRef SourceClient = ServerGetClientByIndex(Context, Packet->Source.CharacterIndex, Packet->Source.Name);
	if (!SourceClient) goto error;

	S2C_DATA_PARTY_INVITE* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, PARTY_INVITE);
	Notification->Result = Packet->Success ? 0 : 1;
	Notification->WorldServerID = Packet->Target.WorldServerIndex;
	Notification->CharacterIndex = (UInt32)Packet->Target.CharacterIndex;
	Notification->CharacterType = 1;
	Notification->Level = Packet->Target.Level;
	Notification->NameLength = Packet->Target.NameLength;
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

	S2C_DATA_NFY_PARTY_INVITE* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_INVITE);
	Notification->WorldServerID = Packet->Source.WorldServerIndex;
	Notification->CharacterIndex = (UInt32)Packet->Source.CharacterIndex;
	Notification->CharacterType = 1;
	Notification->Level = Packet->Source.Level;
	Notification->NameLength = Packet->Source.NameLength;
	CStringCopySafe(Notification->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Source.Name);
	SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);

	UInt8 BattleStyleIndex = TargetCharacter->Data.StyleInfo.Style.BattleStyle | (TargetCharacter->Data.StyleInfo.Style.ExtendedBattleStyle << 3);

	IPC_W2P_DATA_PARTY_INVITE_ACK* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE_ACK);
	Response->Header.SourceConnectionID = TargetClient->Connection->ID;
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Response->Header.Target.Type = IPC_TYPE_PARTY;
	Response->Source = Packet->Source;
	Response->Target = Packet->Target;
	Response->Target.Level = TargetCharacter->Data.Info.Level;
	Response->Target.DungeonIndex = TargetCharacter->Data.Info.DungeonIndex;
	Response->Target.Unknown2 = 0;
	Response->Target.WorldServerIndex = Context->Config.WorldSvr.NodeIndex;
	Response->Target.BattleStyleIndex = BattleStyleIndex;
	Response->Target.WorldIndex = TargetCharacter->Data.Info.WorldIndex;
	Response->Target.OverlordLevel = TargetCharacter->Data.OverlordMasteryInfo.Info.Level;
	Response->Target.MythRebirth = TargetCharacter->Data.MythMasteryInfo.Info.Rebirth;
	Response->Target.MythHolyPower = TargetCharacter->Data.MythMasteryInfo.Info.HolyPower;
	Response->Target.MythLevel = TargetCharacter->Data.MythMasteryInfo.Info.Level;
	Response->Target.ForceWingGrade = TargetCharacter->Data.ForceWingInfo.Info.Grade;
	Response->Target.ForceWingLevel = TargetCharacter->Data.ForceWingInfo.Info.Level;
	Response->Target.NameLength = strlen(TargetCharacter->Name) + 1;
	CStringCopySafe(Response->Target.Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, TargetCharacter->Name);
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
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_CONFIRM) {
	ClientContextRef SourceClient = ServerGetClientByIndex(Context, Packet->SourceCharacterIndex, NULL);
	if (!SourceClient) return;

	// TODO: Add member to PartyID lookup table
	if (Character && Packet->Success) {
		Character->PartyID = Packet->PartyID;
	}

	S2C_DATA_PARTY_INVITE_CONFIRM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, PARTY_INVITE_CONFIRM);
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
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_CANCEL) {
	if (!ClientConnection) return;

	S2C_DATA_PARTY_INVITE_CANCEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, PARTY_INVITE_CANCEL);
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(PARTY_LEAVE) {
	if (!Character) goto error;

	IPC_W2P_DATA_PARTY_LEAVE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_LEAVE);
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.SourceConnectionID = Client->Connection->ID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_PARTY;
	Request->CharacterIndex = Character->CharacterIndex;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_LEAVE) {
	RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Packet->PartyID);
	if (!Party) return;

	for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
		RTPartyMemberInfoRef Member = &Party->Members[Index];

		ClientContextRef Client = ServerGetClientByIndex(Context, Member->CharacterIndex, NULL);
		if (!Client) continue;

		S2C_DATA_NFY_PARTY_LEAVE* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_LEAVE);
		Notification->CharacterIndex = (UInt32)Packet->CharacterIndex;
		SocketSend(Context->ClientSocket, Client->Connection, Notification);
	}
}

IPC_PROCEDURE_BINDING(P2W, PARTY_LEAVE_ACK) {
	if (!ClientConnection || !Character) return;

	if (Packet->Result) {
		RTPartyManagerRemoveMember(Runtime->PartyManager, Character->CharacterIndex);
		Character->PartyID = kEntityIDNull;
	}

	S2C_DATA_PARTY_LEAVE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, PARTY_LEAVE);
	Response->Result = Packet->Result;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(PARTY_EXPEL_MEMBER) {
	if (!Character) goto error;

	// TODO: Implementation missing

	S2C_DATA_PARTY_EXPEL_MEMBER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PARTY_EXPEL_MEMBER);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PARTY_CHANGE_LEADER) {
	if (!Character) goto error;

	RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Runtime->PartyManager, Character->CharacterIndex);
	if (!Party) goto error;

	if (Party->LeaderCharacterIndex == Character->CharacterIndex) {
		IPC_W2P_DATA_PARTY_CHANGE_LEADER* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_CHANGE_LEADER);
		Request->Header.Source = Server->IPCSocket->NodeID;
		Request->Header.SourceConnectionID = Client->Connection->ID;
		Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
		Request->Header.Target.Type = IPC_TYPE_PARTY;
		Request->PartyID = Party->ID;
		Request->CharacterIndex = Packet->CharacterIndex;
		IPCSocketUnicast(Server->IPCSocket, Request);
	}
	else {
		S2C_DATA_PARTY_CHANGE_LEADER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PARTY_CHANGE_LEADER);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}

	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_CHANGE_LEADER) {
	RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Packet->PartyID);
	if (!Party) return;

	RTPartyChangeLeader(Party, Packet->CharacterIndex);

	for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
		RTPartyMemberInfoRef Member = &Party->Members[Index];

		ClientContextRef Client = ServerGetClientByIndex(Context, Member->CharacterIndex, NULL);
		if (!Client) continue;

		S2C_DATA_NFY_PARTY_CHANGE_LEADER* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_CHANGE_LEADER);
		Notification->CharacterIndex = (UInt32)Packet->CharacterIndex;
		SocketSend(Context->ClientSocket, Client->Connection, Notification);
	}
}

IPC_PROCEDURE_BINDING(P2W, PARTY_CHANGE_LEADER_ACK) {
	if (!ClientConnection || !Character) return;

	S2C_DATA_PARTY_CHANGE_LEADER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, PARTY_CHANGE_LEADER);
	Response->Result = Packet->Result;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(PARTY_CHANGE_LOOTING_RULE) {
	if (!Character) goto error;

	// TODO: Implementation missing

	S2C_DATA_NFY_PARTY_CHANGE_LOOTING_RULE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, NFY_PARTY_CHANGE_LOOTING_RULE);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_TIMEOUT) {
	ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->CharacterIndex, NULL);
	if (!TargetClient) return;

	if (Packet->IsCancel) {
		S2C_DATA_NFY_PARTY_INVITE_CANCEL* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_INVITE_CANCEL);
		SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);
	}
	else {
		S2C_DATA_NFY_PARTY_INVITE_TIMEOUT* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_INVITE_TIMEOUT);
		Notification->IsAccept = Packet->IsAccept;
		SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);
	}
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INFO) {
	Int PartyPoolIndex = Packet->Party.ID.EntityIndex;
	if (MemoryPoolIsReserved(Runtime->PartyManager->PartyPool, PartyPoolIndex)) {
		RTPartyRef LocalParty = (RTPartyRef)MemoryPoolFetch(Runtime->PartyManager->PartyPool, PartyPoolIndex);
		memcpy(LocalParty, &Packet->Party, sizeof(struct _RTParty));
	}

	for (Int Index = 0; Index < Packet->Party.MemberCount; Index += 1) {
		RTPartyMemberInfoRef Member = &Packet->Party.Members[Index];

		ClientContextRef Client = ServerGetClientByIndex(Context, Member->CharacterIndex, NULL);
		if (!Client) continue;

		S2C_DATA_NFY_PARTY_INFO* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_INFO);
		Notification->Result = 0;
		Notification->LeaderCharacterIndex = (UInt32)Packet->Party.LeaderCharacterIndex;
		Notification->MemberCount = Packet->Party.MemberCount;

		for (Int MemberIndex = 0; MemberIndex < Packet->Party.MemberCount; MemberIndex += 1) {
			Notification->Members[MemberIndex] = Packet->Party.Members[MemberIndex];
		}

		SocketSend(Context->ClientSocket, Client->Connection, Notification);
	}
}

IPC_PROCEDURE_BINDING(P2W, CLIENT_CONNECT) {
	if (!ClientConnection) return;

	Character->PartyID = Packet->PartyID;

	S2C_DATA_NFY_PARTY_INIT* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_INIT);
	Notification->Result = Packet->Result;
	Notification->DungeonIndex = Packet->DungeonIndex;
	Notification->Unknown1 = 0;
	Notification->PartyID = Packet->PartyID;
	Notification->PartyLeaderIndex = (UInt32)Packet->PartyLeaderIndex;
	Notification->Unknown2 = 0;
	Notification->Unknown3 = 0;
	Notification->WorldServerIndex = Packet->WorldServerIndex;
	Notification->Unknown4 = 0;
	Notification->MemberCount = Packet->MemberCount;

	for (Int Index = 0; Index < Packet->MemberCount; Index += 1) {
		Notification->Members[Index] = Packet->Members[Index];
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
	S2C_DATA_NFY_PARTY_UPDATE* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_UPDATE);
	Notification->MemberCount = Packet->MemberCount;

	for (Int MemberIndex = 0; MemberIndex < Packet->MemberCount; MemberIndex += 1) {
		S2C_DATA_PARTY_UPDATE_MEMBER* Member = &Notification->Members[MemberIndex];
		Member->Info = Packet->MemberInfo[MemberIndex];

		RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Member->Info.CharacterIndex);
		if (Character) {
			Member->Data.WorldIndex = Character->Data.Info.WorldIndex;
			Member->Data.MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
			Member->Data.CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
			Member->Data.MaxMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
			Member->Data.CurrentMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
			Member->Data.PositionX = Character->Movement.PositionCurrent.X;
			Member->Data.PositionY = Character->Movement.PositionCurrent.Y;
			Member->Data.MaxSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX];
			Member->Data.CurrentSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
		}

		// TODO: Resolve all unknown fields...
	}

	for (Int Index = 0; Index < Packet->MemberCount; Index += 1) {
		ClientContextRef Client = ServerGetClientByIndex(Context, Packet->MemberInfo[Index].CharacterIndex, NULL);
		if (!Client) continue;

		SocketSend(Context->ClientSocket, Client->Connection, Notification);
	}
}

IPC_PROCEDURE_BINDING(P2W, BROADCAST_TO_CHARACTER) {
	ClientContextRef SourceClient = ServerGetClientByIndex(Context, Packet->CharacterIndex, NULL);
	if (!SourceClient) return;

	SocketSend(Context->ClientSocket, SourceClient->Connection, Packet->Data);
}

IPC_PROCEDURE_BINDING(P2W, CREATE_PARTY) {
	RTPartyRef Party = RTPartyManagerCreatePartyRemote(Runtime->PartyManager, &Packet->Party);
	if (Party) {
		for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
			RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Party->Members[Index].CharacterIndex);
			if (!Character) continue;

			Character->PartyID = Party->ID;
		}
	}
}

IPC_PROCEDURE_BINDING(P2W, DESTROY_PARTY) {
	// TODO: Cleanup Character->PartyID
	RTPartyManagerDestroyPartyRemote(Runtime->PartyManager, &Packet->Party);
}

Void SendPartyData(
	ServerContextRef Context,
	SocketRef Socket,
	RTPartyRef Party
) {
	if (!Party) return;

	S2C_DATA_NFY_PARTY_UPDATE* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_UPDATE);
	Notification->MemberCount = Party->MemberCount;

	for (Int MemberIndex = 0; MemberIndex < Party->MemberCount; MemberIndex += 1) {
		RTPartyMemberInfoRef MemberInfo = &Party->Members[MemberIndex];

		RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, Party->Members[MemberIndex].CharacterIndex);
		if (Character) {
			UInt8 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);

			MemberInfo->CharacterIndex = (UInt32)Character->CharacterIndex;
			MemberInfo->Level = Character->Data.Info.Level;
			MemberInfo->DungeonIndex = Character->Data.Info.DungeonIndex;
			MemberInfo->Unknown2 = 0;
			MemberInfo->WorldServerIndex = Context->Config.WorldSvr.NodeIndex;
			MemberInfo->BattleStyleIndex = BattleStyleIndex;
			MemberInfo->WorldIndex = Character->Data.Info.WorldIndex;
			MemberInfo->OverlordLevel = Character->Data.OverlordMasteryInfo.Info.Level;
			MemberInfo->MythRebirth = Character->Data.MythMasteryInfo.Info.Rebirth;
			MemberInfo->MythHolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
			MemberInfo->MythLevel = Character->Data.MythMasteryInfo.Info.Level;
			MemberInfo->ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
			MemberInfo->ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
			MemberInfo->NameLength = strlen(Character->Name) + 1;
			CStringCopySafe(MemberInfo->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH, Character->Name);
		}

		S2C_DATA_PARTY_UPDATE_MEMBER* Member = &Notification->Members[MemberIndex];
		Member->Info = *MemberInfo;

		if (Character) {
			Member->Data.WorldIndex = Character->Data.Info.WorldIndex;
			Member->Data.MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
			Member->Data.CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
			Member->Data.MaxMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
			Member->Data.CurrentMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
			Member->Data.PositionX = Character->Movement.PositionCurrent.X;
			Member->Data.PositionY = Character->Movement.PositionCurrent.Y;
			Member->Data.MaxSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX];
			Member->Data.CurrentSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
			Member->Data.SpiritRaiseCooldown = Character->Data.CooldownInfo.Info.SpiritRaiseCooldown;
			Member->Data.HasBlessingBeadPlus = false;
		}
	}

	for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
		ClientContextRef Client = ServerGetClientByIndex(Context, Party->Members[Index].CharacterIndex, NULL);
		if (!Client) continue;
		SocketSend(Context->ClientSocket, Client->Connection, Notification);
	}
}
