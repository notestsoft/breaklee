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
	Request->Source.MemberID.Serial = Character->ID.Serial;
	Request->Source.Info.CharacterIndex = Character->CharacterIndex;
	Request->Source.Info.Level = Character->Data.Info.Level;
	Request->Source.Info.DungeonIndex = Character->Data.Info.DungeonIndex;
	Request->Source.Info.Unknown2 = 0;
	Request->Source.Info.WorldServerIndex = Context->Config.WorldSvr.NodeIndex;
	Request->Source.Info.BattleStyleIndex = BattleStyleIndex;
	Request->Source.Info.WorldIndex = Character->Data.Info.WorldIndex;
	Request->Source.Info.OverlordLevel = Character->Data.OverlordMasteryInfo.Info.Level;
	Request->Source.Info.MythRebirth = Character->Data.MythMasteryInfo.Info.Rebirth;
	Request->Source.Info.MythHolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
	Request->Source.Info.MythLevel = Character->Data.MythMasteryInfo.Info.Level;
	Request->Source.Info.ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
	Request->Source.Info.ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
	Request->Source.Info.NameLength = strlen(Character->Name) + 1;
	CStringCopySafe(Request->Source.Info.Name, MAX_CHARACTER_NAME_LENGTH, Character->Name);

	Request->Target.Info.WorldServerIndex = Packet->WorldServerID;
	Request->Target.Info.CharacterIndex = Packet->CharacterIndex;
	Request->Target.Info.NameLength = Packet->NameLength;
	CStringCopySafe(Request->Target.Info.Name, MAX_CHARACTER_NAME_LENGTH, Packet->Name);

	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(P2W, PARTY_INVITE_ACK) {
	ClientContextRef SourceClient = ServerGetClientByIndex(Context, Packet->Source.Info.CharacterIndex, Packet->Source.Info.Name);
	if (!SourceClient) goto error;

	S2C_DATA_PARTY_INVITE* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, PARTY_INVITE);
	Notification->Result = Packet->Success ? 0 : 1;
	Notification->WorldServerID = Packet->Target.Info.WorldServerIndex;
	Notification->CharacterIndex = (UInt32)Packet->Target.Info.CharacterIndex;
	Notification->CharacterType = 1;
	Notification->Level = Packet->Target.Info.Level;
	Notification->NameLength = Packet->Target.Info.NameLength;
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

	S2C_DATA_NFY_PARTY_INVITE* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_INVITE);
	Notification->WorldServerID = Packet->Source.Info.WorldServerIndex;
	Notification->CharacterIndex = (UInt32)Packet->Source.Info.CharacterIndex;
	Notification->CharacterType = 1;
	Notification->Level = Packet->Source.Info.Level;
	Notification->NameLength = Packet->Source.Info.NameLength;
	CStringCopySafe(Notification->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Source.Info.Name);
	SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);

	UInt8 BattleStyleIndex = TargetCharacter->Data.StyleInfo.Style.BattleStyle | (TargetCharacter->Data.StyleInfo.Style.ExtendedBattleStyle << 3);

	IPC_W2P_DATA_PARTY_INVITE_ACK* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, PARTY_INVITE_ACK);
	Response->Header.SourceConnectionID = TargetClient->Connection->ID;
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Response->Header.Target.Type = IPC_TYPE_PARTY;
	Response->Source = Packet->Source;
	Response->Target = Packet->Target;
	Response->Target.MemberID.Serial = TargetCharacter->ID.Serial;
	Response->Target.Info.Level = TargetCharacter->Data.Info.Level;
	Response->Target.Info.DungeonIndex = TargetCharacter->Data.Info.DungeonIndex;
	Response->Target.Info.Unknown2 = 0;
	Response->Target.Info.WorldServerIndex = Context->Config.WorldSvr.NodeIndex;
	Response->Target.Info.BattleStyleIndex = BattleStyleIndex;
	Response->Target.Info.WorldIndex = TargetCharacter->Data.Info.WorldIndex;
	Response->Target.Info.OverlordLevel = TargetCharacter->Data.OverlordMasteryInfo.Info.Level;
	Response->Target.Info.MythRebirth = TargetCharacter->Data.MythMasteryInfo.Info.Rebirth;
	Response->Target.Info.MythHolyPower = TargetCharacter->Data.MythMasteryInfo.Info.HolyPower;
	Response->Target.Info.MythLevel = TargetCharacter->Data.MythMasteryInfo.Info.Level;
	Response->Target.Info.ForceWingGrade = TargetCharacter->Data.ForceWingInfo.Info.Grade;
	Response->Target.Info.ForceWingLevel = TargetCharacter->Data.ForceWingInfo.Info.Level;
	Response->Target.Info.NameLength = strlen(TargetCharacter->Name) + 1;
	CStringCopySafe(Response->Target.Info.Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, TargetCharacter->Name);
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

	for (Int32 Index = 0; Index < Party->MemberCount; Index += 1) {
		RTPartySlotRef Slot = &Party->Members[Index];

		ClientContextRef Client = ServerGetClientByIndex(Context, Slot->Info.CharacterIndex, NULL);
		if (!Client) continue;

		S2C_DATA_NFY_PARTY_LEAVE* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_LEAVE);
		Notification->CharacterIndex = Packet->CharacterIndex;
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

	for (Int32 Index = 0; Index < Party->MemberCount; Index += 1) {
		RTPartySlotRef Slot = &Party->Members[Index];

		ClientContextRef Client = ServerGetClientByIndex(Context, Slot->Info.CharacterIndex, NULL);
		if (!Client) continue;

		S2C_DATA_NFY_PARTY_CHANGE_LEADER* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_CHANGE_LEADER);
		Notification->CharacterIndex = Packet->CharacterIndex;
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
	Index PartyPoolIndex = Packet->Party.ID.EntityIndex;
	if (MemoryPoolIsReserved(Runtime->PartyManager->PartyPool, PartyPoolIndex)) {
		RTPartyRef LocalParty = (RTPartyRef)MemoryPoolFetch(Runtime->PartyManager->PartyPool, PartyPoolIndex);
		memcpy(LocalParty, &Packet->Party, sizeof(struct _RTParty));
	}

	for (Int32 Index = 0; Index < Packet->Party.MemberCount; Index += 1) {
		RTPartySlotRef Slot = &Packet->Party.Members[Index];

		ClientContextRef Client = ServerGetClientByIndex(Context, Slot->Info.CharacterIndex, NULL);
		if (!Client) continue;

		// NOTE: This is just some bullshit to make other shit work clean it up...
		RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Client->CharacterIndex);
		if (Character) {
			Character->PartyID = Packet->Party.ID;
		}

		S2C_DATA_NFY_PARTY_INFO* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PARTY_INFO);
		Notification->Result = 0;
		Notification->LeaderCharacterIndex = (UInt32)Packet->Party.LeaderCharacterIndex;
		Notification->MemberCount = Packet->Party.MemberCount;

		for (Int32 MemberIndex = 0; MemberIndex < Packet->Party.MemberCount; MemberIndex += 1) {
			Notification->Members[MemberIndex] = Packet->Party.Members[MemberIndex].Info;
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

	for (Index Index = 0; Index < Packet->MemberCount; Index += 1) {
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

	for (Int32 MemberIndex = 0; MemberIndex < Packet->MemberCount; MemberIndex += 1) {
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

	for (Int32 Index = 0; Index < Packet->MemberCount; Index += 1) {
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
	RTPartyManagerCreatePartyRemote(Runtime->PartyManager, &Packet->Party);
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

	for (Int32 MemberIndex = 0; MemberIndex < Party->MemberCount; MemberIndex += 1) {
		RTPartySlotRef MemberSlot = &Party->Members[MemberIndex];

		RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, Party->Members[MemberIndex].Info.CharacterIndex);
		if (Character) {
			UInt8 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);

			MemberSlot->Info.CharacterIndex = Character->CharacterIndex;
			MemberSlot->Info.Level = Character->Data.Info.Level;
			MemberSlot->Info.DungeonIndex = Character->Data.Info.DungeonIndex;
			MemberSlot->Info.Unknown2 = 0;
			MemberSlot->Info.WorldServerIndex = Context->Config.WorldSvr.NodeIndex;
			MemberSlot->Info.BattleStyleIndex = BattleStyleIndex;
			MemberSlot->Info.WorldIndex = Character->Data.Info.WorldIndex;
			MemberSlot->Info.OverlordLevel = Character->Data.OverlordMasteryInfo.Info.Level;
			MemberSlot->Info.MythRebirth = Character->Data.MythMasteryInfo.Info.Rebirth;
			MemberSlot->Info.MythHolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
			MemberSlot->Info.MythLevel = Character->Data.MythMasteryInfo.Info.Level;
			MemberSlot->Info.ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
			MemberSlot->Info.ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
			MemberSlot->Info.NameLength = strlen(Character->Name) + 1;
			CStringCopySafe(MemberSlot->Info.Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH, Character->Name);
		}

		S2C_DATA_PARTY_UPDATE_MEMBER* Member = &Notification->Members[MemberIndex];
		Member->Info = MemberSlot->Info;

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

	for (Int32 Index = 0; Index < Party->MemberCount; Index += 1) {
		ClientContextRef Client = ServerGetClientByIndex(Context, Party->Members[Index].Info.CharacterIndex, NULL);
		if (!Client) continue;
		SocketSend(Context->ClientSocket, Client->Connection, Notification);
	}
}