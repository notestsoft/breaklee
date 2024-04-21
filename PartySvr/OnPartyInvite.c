#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_INVITE) {
	IPC_P2W_DATA_PARTY_INVITE* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Success = false;
	Response->Source = Packet->Source;
	Response->Target = Packet->Target;
	
	Bool IsTargetInParty = DictionaryLookup(Context->CharacterToPartyEntity, &Packet->Target.CharacterIndex) != NULL;
	if (IsTargetInParty) goto error;

	RTPartyRef Party = ServerGetPartyByCharacter(Context, Packet->Source.CharacterIndex);
	if (!Party) Party = ServerCreateParty(Context, Packet->Source.CharacterIndex, Packet->Source.CharacterID, RUNTIME_PARTY_TYPE_TEMPORARY);
	if (!Party) goto error;

	if (Party->MemberCount >= RUNTIME_PARTY_MAX_MEMBER_COUNT) goto error;
	if (Party->InvitationCount >= RUNTIME_PARTY_MAX_MEMBER_COUNT) goto error;

	RTPartyInvitationRef Invitation = &Party->Invitations[Party->InvitationCount];
	Party->InvitationCount += 1;

	Invitation->CharacterIndex = Packet->Target.CharacterIndex;
	Invitation->WorldServerID = Packet->Target.WorldServerID;
	Invitation->NameLength = Packet->Target.NameLength;
	CStringCopySafe(Invitation->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH, Packet->Target.Name);
	Invitation->InvitationTimestamp = GetTimestampMs();
	DictionaryInsert(Context->CharacterToPartyEntity, &Packet->Target.CharacterIndex, &Party->ID, sizeof(RTEntityID));

	Response->Success = true;
	IPCSocketBroadcast(Socket, Response);
	return;

error:
	IPCSocketBroadcast(Socket, Response);
}
