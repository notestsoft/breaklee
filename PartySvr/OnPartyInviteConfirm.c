#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_INVITE_CONFIRM) {
    Index* PartyInvitationPoolIndex = DictionaryLookup(Context->CharacterToPartyInvite, &Packet->SourceCharacterIndex);
    if (!PartyInvitationPoolIndex) goto error;

    RTPartyInvitationRef Invitation = (RTPartyInvitationRef)MemoryPoolFetch(Context->PartyInvitationPool, *PartyInvitationPoolIndex);
    if (!Invitation) goto error;

    RTPartyRef Party = ServerGetPartyByCharacter(Context, Packet->TargetCharacterIndex);
    if (!Party) goto error;

    if (Packet->IsAccept) {
        RTPartySlotRef Member = RTPartyAddMember(Party, Invitation->Member.CharacterIndex, kEntityIDNull);
        DictionaryInsert(Context->CharacterToPartyEntity, &Invitation->Member.CharacterIndex, &Party->ID, sizeof(struct _RTEntityID));

        Member->NodeIndex = Invitation->Member.NodeIndex;
        Member->Level = Invitation->Member.Level;
        Member->OverlordLevel = Invitation->Member.OverlordLevel;
        Member->MythRebirth = Invitation->Member.MythRebirth;
        Member->MythHolyPower = Invitation->Member.MythHolyPower;
        Member->MythLevel = Invitation->Member.MythLevel;
        Member->ForceWingGrade = Invitation->Member.ForceWingGrade;
        Member->ForceWingLevel = Invitation->Member.ForceWingLevel;
        CStringCopySafe(Member->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Invitation->Member.Name);

        if (Party->PartyType == RUNTIME_PARTY_TYPE_TEMPORARY) {
            Party->PartyType = RUNTIME_PARTY_TYPE_NORMAL;
        }

        BroadcastPartyInfo(Server, Context, Socket, Connection, Party);
    }

    IPC_P2W_DATA_PARTY_INVITE_TIMEOUT* Notification = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_TIMEOUT);
    Notification->Header.Source = Server->IPCSocket->NodeID;
    Notification->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
    Notification->Header.Target.Index = Packet->TargetNodeIndex;
    Notification->Header.Target.Type = IPC_TYPE_WORLD;
    Notification->CharacterIndex = Packet->TargetCharacterIndex;
    Notification->IsAccept = Packet->IsAccept;
    IPCSocketUnicast(Socket, Notification);

    MemoryPoolRelease(Context->PartyInvitationPool, *PartyInvitationPoolIndex);
    DictionaryRemove(Context->CharacterToPartyInvite, &Packet->SourceCharacterIndex);

    if (!Packet->IsAccept && Party->PartyType == RUNTIME_PARTY_TYPE_TEMPORARY) {
        ServerDestroyParty(Context, Party);
    }

    IPC_P2W_DATA_PARTY_INVITE_CONFIRM* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_CONFIRM);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    Response->SourceCharacterIndex = Packet->SourceCharacterIndex;
    Response->SourceNodeIndex = Packet->SourceNodeIndex;
    Response->TargetCharacterIndex = Packet->TargetCharacterIndex;
    Response->TargetNodeIndex = Packet->TargetNodeIndex;
    Response->Success = true;
    IPCSocketUnicast(Socket, Response);
    return;

error:
    {
        IPC_P2W_DATA_PARTY_INVITE_CONFIRM* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_CONFIRM);
        Response->Header.Source = Server->IPCSocket->NodeID;
        Response->Header.Target = Packet->Header.Source;
        Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
        Response->SourceCharacterIndex = Packet->SourceCharacterIndex;
        Response->SourceNodeIndex = Packet->SourceNodeIndex;
        Response->TargetCharacterIndex = Packet->TargetCharacterIndex;
        Response->TargetNodeIndex = Packet->TargetNodeIndex;
        Response->Success = false;
        IPCSocketUnicast(Socket, Response);
    }
}
