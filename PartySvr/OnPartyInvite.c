#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_INVITE) {
	RTPartySlotRef Source = &Packet->Source;
	RTPartySlotRef Target = &Packet->Target;

	Bool IsTargetInParty = DictionaryLookup(Context->PartyManager->CharacterToPartyEntity, &Target->Info.CharacterIndex) != NULL;
	if (IsTargetInParty) goto error;

	Bool HasTargetInvitation = DictionaryLookup(Context->PartyManager->CharacterToPartyInvite, &Target->Info.CharacterIndex) != NULL;
	if (HasTargetInvitation) goto error;

	RTPartyRef Party = ServerGetPartyByCharacter(Context, Source->Info.CharacterIndex);
	if (!Party) {
		Party = ServerCreateParty(Context, Source->Info.CharacterIndex, kEntityIDNull, RUNTIME_PARTY_TYPE_NORMAL);
		if (!Party) goto error;

		RTPartySlotRef Member = RTPartyGetMember(Party, Source->Info.CharacterIndex);
		Member->NodeIndex = Packet->Source.NodeIndex;
		memcpy(&Member->Info, &Source->Info, sizeof(struct _RTPartyMemberInfo));
	}

	assert(Party);
	if (Party->MemberCount >= RUNTIME_PARTY_MAX_MEMBER_COUNT) goto error;

	Index PartyInvitationPoolIndex = 0;
	RTPartyInvitationRef Invitation = (RTPartyInvitationRef)MemoryPoolReserveNext(Context->PartyManager->PartyInvitationPool, &PartyInvitationPoolIndex);
	DictionaryInsert(Context->PartyManager->CharacterToPartyInvite, &Target->Info.CharacterIndex, &PartyInvitationPoolIndex, sizeof(Index));

	Invitation->InviterCharacterIndex = Packet->Source.Info.CharacterIndex;
	Invitation->PartyID = Party->ID;
	Invitation->Member.NodeIndex = Packet->Target.NodeIndex;
	memcpy(&Invitation->Member.Info, &Target->Info, sizeof(struct _RTPartyMemberInfo));
	Invitation->InvitationTimestamp = GetTimestampMs();

	IPC_P2W_DATA_PARTY_INVITE* Request = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE);
	Request->Header.Source = Packet->Header.Source;
	Request->Header.SourceConnectionID = Packet->Header.SourceConnectionID;
	Request->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
	Request->Header.Target.Index = Packet->Target.NodeIndex;
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
	RTPartySlotRef Source = &Packet->Source;
	RTPartySlotRef Target = &Packet->Target;

	Index* PartyInvitationPoolIndex = DictionaryLookup(Context->PartyManager->CharacterToPartyInvite, &Target->Info.CharacterIndex);
	if (!PartyInvitationPoolIndex) goto error;

	RTPartyInvitationRef Invitation = (RTPartyInvitationRef)MemoryPoolFetch(Context->PartyManager->PartyInvitationPool, *PartyInvitationPoolIndex);
	if (!Invitation) goto error;

	RTPartyRef Party = ServerGetPartyByCharacter(Context, Source->Info.CharacterIndex);
	if (!Party) goto error;

	if (Packet->Success) {
		Invitation->Member.NodeIndex = Packet->Target.NodeIndex;
		memcpy(&Invitation->Member.Info, &Target->Info, sizeof(struct _RTPartyMemberInfo));
	}
	else if (Party->PartyType == RUNTIME_PARTY_TYPE_NORMAL) {
		MemoryPoolRelease(Context->PartyManager->PartyInvitationPool, *PartyInvitationPoolIndex);
		DictionaryRemove(Context->PartyManager->CharacterToPartyInvite, &Target->Info.CharacterIndex);
		ServerDestroyParty(Context, Party);
	}

	IPC_P2W_DATA_PARTY_INVITE_ACK* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_ACK);
	Response->Header.Source = Packet->Header.Source;
	Response->Header.SourceConnectionID = Packet->Header.SourceConnectionID;
	Response->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
	Response->Header.Target.Index = Packet->Source.NodeIndex;
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

	for (Index Index = 0; Index < Party->MemberCount; Index += 1) {
		memcpy(&Notification->MemberInfo[Index], &Party->Members[Index].Info, sizeof(struct _RTPartyMemberInfo));
	}

	IPCSocketBroadcast(Socket, Notification);
	return;

error:
	{
		IPC_P2W_DATA_PARTY_INVITE_ACK* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_ACK);
		Response->Header.Source = Packet->Header.Source;
		Response->Header.SourceConnectionID = Packet->Header.SourceConnectionID;
		Response->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
		Response->Header.Target.Index = Packet->Source.NodeIndex;
		Response->Header.Target.Type = IPC_TYPE_WORLD;
		Response->Success = Packet->Success;
		Response->Source = Packet->Source;
		Response->Target = Packet->Target;
		Response->Success = false;
		IPCSocketUnicast(Socket, Response);
	}
}
