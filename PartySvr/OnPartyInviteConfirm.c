#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_INVITE_CONFIRM) {
    Index* PartyInvitationPoolIndex = DictionaryLookup(Context->PartyManager->CharacterToPartyInvite, &Packet->SourceCharacterIndex);
    if (!PartyInvitationPoolIndex) goto error;

    RTPartyInvitationRef Invitation = (RTPartyInvitationRef)MemoryPoolFetch(Context->PartyManager->PartyInvitationPool, *PartyInvitationPoolIndex);
    if (!Invitation) goto error;

    RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Context->PartyManager, Packet->TargetCharacterIndex);
    if (!Party) goto error;

    if (Packet->IsAccept) {
        RTPartySlotRef Member = RTPartyAddMember(Party, Invitation->Member.Info.CharacterIndex, kEntityIDNull);
        DictionaryInsert(Context->PartyManager->CharacterToPartyEntity, &Invitation->Member.Info.CharacterIndex, &Party->ID, sizeof(struct _RTEntityID));

        Member->MemberID = Invitation->Member.MemberID;
        Member->Info.WorldServerIndex = Invitation->Member.Info.WorldServerIndex;
        memcpy(&Member->Info, &Invitation->Member.Info, sizeof(struct _RTPartyMemberInfo));

        BroadcastPartyInfo(Server, Context, Socket, Party);
    }

    IPC_P2W_DATA_PARTY_INVITE_TIMEOUT* Notification = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_TIMEOUT);
    Notification->Header.Source = Server->IPCSocket->NodeID;
    Notification->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
    Notification->Header.Target.Index = Packet->TargetNodeIndex;
    Notification->Header.Target.Type = IPC_TYPE_WORLD;
    Notification->CharacterIndex = Packet->TargetCharacterIndex;
    Notification->IsAccept = Packet->IsAccept;
    IPCSocketUnicast(Socket, Notification);

    MemoryPoolRelease(Context->PartyManager->PartyInvitationPool, *PartyInvitationPoolIndex);
    DictionaryRemove(Context->PartyManager->CharacterToPartyInvite, &Packet->SourceCharacterIndex);

    if (!Packet->IsAccept && Party->PartyType == RUNTIME_PARTY_TYPE_NORMAL && Party->MemberCount < 2) {
        BroadcastDestroyParty(Server, Context, Server->IPCSocket, Party);
        RTPartyManagerDestroyParty(Context->PartyManager, Party);
    }

    IPC_P2W_DATA_PARTY_INVITE_CONFIRM* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_CONFIRM);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    Response->SourceCharacterIndex = Packet->SourceCharacterIndex;
    Response->SourceNodeIndex = Packet->SourceNodeIndex;
    Response->TargetCharacterIndex = Packet->TargetCharacterIndex;
    Response->TargetNodeIndex = Packet->TargetNodeIndex;
    Response->Success = Packet->IsAccept;
    IPCSocketUnicast(Socket, Response);

    BroadcastPartyData(Server, Context, Socket, Party);
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
