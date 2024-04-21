#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_INVITE_CONFIRM) {
	IPC_P2W_DATA_PARTY_INVITE_CONFIRM* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_CONFIRM);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Success = false;
	Response->CharacterIndex = Packet->CharacterIndex;
	Response->CharacterID = Packet->CharacterID;
    Response->WorldServerID = Packet->WorldServerID;
	Response->TargetCharacterIndex = Packet->TargetCharacterIndex;
	Response->TargetWorldServerID = Packet->TargetWorldServerID;

    RTPartyRef Party = ServerGetPartyByCharacter(Context, Packet->TargetCharacterIndex);
    if (!Party) goto error;

    Bool Success = false;
    for (Index Index = 0; Index < Party->InvitationCount; Index += 1) {
        RTPartyInvitationRef Invitation = &Party->Invitations[Index];
        if (Invitation->CharacterIndex != Packet->CharacterIndex) continue;

        Int32 TailLength = Party->InvitationCount - Index - 1;
        if (TailLength > 0) {
            memmove(&Party->Invitations[Index], &Party->Invitations[Index + 1], TailLength * sizeof(struct _RTPartyInvitation));
        }
        Party->InvitationCount -= 1;

        Success = true;
        break;
    }
    if (!Success) goto error;

    if (Party->PartyType == RUNTIME_PARTY_TYPE_TEMPORARY) {
        Party->PartyType = RUNTIME_PARTY_TYPE_NORMAL;
    }

    RTPartyAddMember(Party, Packet->TargetCharacterIndex, kEntityIDNull);
    Response->Success = true;
	IPCSocketBroadcast(Socket, Response);

    IPC_P2W_DATA_PARTY_INFO* Notification = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INFO);
    Notification->Header.Source = Server->IPCSocket->NodeID;
    Notification->Header.Target = Packet->Header.Source;
    memcpy(&Notification->Party, Party, sizeof(struct _RTParty));
    IPCSocketBroadcast(Socket, Notification);

    return;

error:
    Response->Success = false;
    IPCSocketBroadcast(Socket, Response);
}
