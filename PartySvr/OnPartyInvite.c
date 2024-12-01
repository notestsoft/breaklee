#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_INVITE) {
	RTPartyMemberInfoRef Source = &Packet->Source;
	RTPartyMemberInfoRef Target = &Packet->Target;

	UInt32 CharacterIndex = Target->CharacterIndex;
	Bool IsTargetInParty = DictionaryLookup(Context->PartyManager->CharacterToPartyEntity, &CharacterIndex) != NULL;
	if (IsTargetInParty) goto error;

	Bool HasTargetInvitation = DictionaryLookup(Context->PartyManager->CharacterToPartyInvite, &CharacterIndex) != NULL;
	if (HasTargetInvitation) goto error;

	Bool Created = false;
	RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Context->PartyManager, Source->CharacterIndex);
	if (!Party) {
		Party = RTPartyManagerCreateParty(Context->PartyManager, Source, RUNTIME_PARTY_TYPE_NORMAL);
		if (!Party) goto error;
		Created = true;
	}
	assert(Party);

	if (Party->MemberCount >= RUNTIME_PARTY_MAX_MEMBER_COUNT) goto error;

	Int PartyInvitationPoolIndex = 0;
	RTPartyInvitationRef Invitation = (RTPartyInvitationRef)MemoryPoolReserveNext(Context->PartyManager->PartyInvitationPool, &PartyInvitationPoolIndex);
	DictionaryInsert(Context->PartyManager->CharacterToPartyInvite, &CharacterIndex, &PartyInvitationPoolIndex, sizeof(Int));

	Invitation->InviterCharacterIndex = Packet->Source.CharacterIndex;
	Invitation->PartyID = Party->ID;
	memcpy(&Invitation->Member, Target, sizeof(struct _RTPartyMemberInfo));
	Invitation->InvitationTimestamp = GetTimestampMs();

	if (Created) {
		BroadcastCreateParty(Server, Context, Socket, Party);
	}

	IPC_P2W_DATA_PARTY_INVITE* Request = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE);
	Request->Header.Source = Packet->Header.Source;
	Request->Header.SourceConnectionID = Packet->Header.SourceConnectionID;
	Request->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
	Request->Header.Target.Index = (UInt32)Packet->Target.WorldServerIndex;
	Request->Header.Target.Type = IPC_TYPE_WORLD;
	Request->Source = Packet->Source;
	Request->Target = Packet->Target;
	IPCSocketUnicast(Socket, Request);
	return;

error:
	{
		IPC_P2W_DATA_PARTY_INVITE_ACK* Request = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_ACK);
		Request->Header.Source = Server->IPCSocket->NodeID;
		Request->Header.Target = Packet->Header.Source;
		Request->Source = Packet->Source;
		Request->Target = Packet->Target;
		Request->Success = false;
		IPCSocketUnicast(Socket, Request);
	}
}

IPC_PROCEDURE_BINDING(W2P, PARTY_INVITE_ACK) {
	RTPartyMemberInfoRef Source = &Packet->Source;
	RTPartyMemberInfoRef Target = &Packet->Target;

	UInt32 CharacterIndex = Target->CharacterIndex;
	Int* PartyInvitationPoolIndex = DictionaryLookup(Context->PartyManager->CharacterToPartyInvite, &CharacterIndex);
	if (!PartyInvitationPoolIndex) goto error;

	RTPartyInvitationRef Invitation = (RTPartyInvitationRef)MemoryPoolFetch(Context->PartyManager->PartyInvitationPool, *PartyInvitationPoolIndex);
	if (!Invitation) goto error;

	RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Context->PartyManager, Source->CharacterIndex);
	if (!Party) goto error;

	if (Packet->Success) {
		memcpy(&Invitation->Member, Target, sizeof(struct _RTPartyMemberInfo));
	}
	else if (Party->PartyType == RUNTIME_PARTY_TYPE_NORMAL) {
		MemoryPoolRelease(Context->PartyManager->PartyInvitationPool, *PartyInvitationPoolIndex);
		DictionaryRemove(Context->PartyManager->CharacterToPartyInvite, &CharacterIndex);
		BroadcastDestroyParty(Server, Context, Server->IPCSocket, Party);
		RTPartyManagerDestroyParty(Context->PartyManager, Party);
	}

	IPC_P2W_DATA_PARTY_INVITE_ACK* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_ACK);
	Response->Header.Source = Packet->Header.Source;
	Response->Header.SourceConnectionID = Packet->Header.SourceConnectionID;
	Response->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
	Response->Header.Target.Index = (UInt32)Packet->Source.WorldServerIndex;
	Response->Header.Target.Type = IPC_TYPE_WORLD;
	Response->Success = Packet->Success;
	Response->Source = Packet->Source;
	Response->Target = Packet->Target;
	Response->Success = Packet->Success;
	IPCSocketUnicast(Socket, Response);

	IPC_P2W_DATA_PARTY_DATA* Notification = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_DATA);
	Notification->Header.Source = Server->IPCSocket->NodeID;
	Notification->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
	Notification->Header.Target.Type = IPC_TYPE_WORLD;
	Notification->MemberCount = Party->MemberCount;

	for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
		memcpy(&Notification->MemberInfo[Index], &Party->Members[Index], sizeof(struct _RTPartyMemberInfo));
	}

	IPCSocketBroadcast(Socket, Notification);
	return;

error:
	{
		IPC_P2W_DATA_PARTY_INVITE_ACK* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_ACK);
		Response->Header.Source = Packet->Header.Source;
		Response->Header.SourceConnectionID = Packet->Header.SourceConnectionID;
		Response->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
		Response->Header.Target.Index = (UInt32)Packet->Source.WorldServerIndex;
		Response->Header.Target.Type = IPC_TYPE_WORLD;
		Response->Success = Packet->Success;
		Response->Source = Packet->Source;
		Response->Target = Packet->Target;
		Response->Success = false;
		IPCSocketUnicast(Socket, Response);
	}
}
