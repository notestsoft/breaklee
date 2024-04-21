#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_INVITE) {
	Bool IsTargetInParty = DictionaryLookup(Context->CharacterToPartyEntity, &Packet->Target.CharacterIndex) != NULL;
	if (IsTargetInParty) goto error;

	Bool HasTargetInvitation = DictionaryLookup(Context->CharacterToPartyInvite, &Packet->Target.CharacterIndex) != NULL;
	if (HasTargetInvitation) goto error;

	RTPartyRef Party = ServerGetPartyByCharacter(Context, Packet->Source.CharacterIndex);
	if (!Party) {
		Party = ServerCreateParty(Context, Packet->Source.CharacterIndex, Packet->Source.CharacterID, RUNTIME_PARTY_TYPE_TEMPORARY);
		if (!Party) goto error;

		RTPartySlotRef Member = RTPartyGetMember(Party, Packet->Source.CharacterIndex);
		Member->NodeIndex = Packet->Source.NodeIndex;
		Member->Level = Packet->Source.Level;
		Member->OverlordLevel = Packet->Source.OverlordLevel;
		Member->MythRebirth = Packet->Source.MythRebirth;
		Member->MythHolyPower = Packet->Source.MythHolyPower;
		Member->MythLevel = Packet->Source.MythLevel;
		Member->ForceWingGrade = Packet->Source.ForceWingGrade;
		Member->ForceWingLevel = Packet->Source.ForceWingLevel;
		CStringCopySafe(Member->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Source.Name);
	}

	assert(Party);
	if (Party->MemberCount >= RUNTIME_PARTY_MAX_MEMBER_COUNT) goto error;

	Index PartyInvitationPoolIndex = 0;
	RTPartyInvitationRef Invitation = (RTPartyInvitationRef)MemoryPoolReserveNext(Context->PartyInvitationPool, &PartyInvitationPoolIndex);
	DictionaryInsert(Context->CharacterToPartyInvite, &Packet->Target.CharacterIndex, &PartyInvitationPoolIndex, sizeof(Index));

	Invitation->Member.CharacterIndex = Packet->Target.CharacterIndex;
	Invitation->Member.NodeIndex = Packet->Target.NodeIndex;
	CStringCopySafe(Invitation->Member.Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Target.Name);
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
	Index* PartyInvitationPoolIndex = DictionaryLookup(Context->CharacterToPartyInvite, &Packet->Target.CharacterIndex);
	if (!PartyInvitationPoolIndex) goto error;

	RTPartyInvitationRef Invitation = (RTPartyInvitationRef)MemoryPoolFetch(Context->PartyInvitationPool, *PartyInvitationPoolIndex);
	if (!Invitation) goto error;

	RTPartyRef Party = ServerGetPartyByCharacter(Context, Packet->Source.CharacterIndex);
	if (!Party) goto error;

	if (Packet->Success) {
		Invitation->Member.NodeIndex = Packet->Target.NodeIndex;
		Invitation->Member.Level = Packet->Target.Level;
		Invitation->Member.OverlordLevel = Packet->Target.OverlordLevel;
		Invitation->Member.MythRebirth = Packet->Target.MythRebirth;
		Invitation->Member.MythHolyPower = Packet->Target.MythHolyPower;
		Invitation->Member.MythLevel = Packet->Target.MythLevel;
		Invitation->Member.ForceWingGrade = Packet->Target.ForceWingGrade;
		Invitation->Member.ForceWingLevel = Packet->Target.ForceWingLevel;
	}
	else if (Party->PartyType == RUNTIME_PARTY_TYPE_TEMPORARY) {
		MemoryPoolRelease(Context->PartyInvitationPool, *PartyInvitationPoolIndex);
		DictionaryRemove(Context->CharacterToPartyInvite, &Packet->Target.CharacterIndex);
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
