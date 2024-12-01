#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_LEAVE) {
    RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Context->PartyManager, Packet->CharacterIndex);
    if (!Party) goto error;

    IPC_P2W_DATA_PARTY_LEAVE_ACK* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_LEAVE_ACK);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    Response->Result = RTPartyManagerRemoveMember(Context->PartyManager, Packet->CharacterIndex);
    IPCSocketUnicast(Socket, Response);

    if (Response->Result) {
        for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
            RTPartyMemberInfoRef Member = &Party->Members[Index];

            IPC_P2W_DATA_PARTY_LEAVE* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_LEAVE);
            Response->Header.Source = Server->IPCSocket->NodeID;
            Response->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
            Response->Header.Target.Index = (UInt32)Member->WorldServerIndex;
            Response->Header.Target.Type = IPC_TYPE_WORLD;
            Response->PartyID = Party->ID;
            Response->CharacterIndex = Member->CharacterIndex;
            IPCSocketUnicast(Socket, Response);
        }
    }

    Bool IsDestroyed = (
        (Party->PartyType == RUNTIME_PARTY_TYPE_SOLO_DUNGEON && Party->MemberCount < 1) ||
        (Party->PartyType == RUNTIME_PARTY_TYPE_NORMAL && Party->MemberCount < 2)
    );

    if (IsDestroyed) {
        BroadcastDestroyParty(Server, Context, Server->IPCSocket, Party);
        RTPartyManagerDestroyParty(Context->PartyManager, Party);
    }
    else {
        BroadcastPartyData(Server, Context, Socket, Party);
    }

    return;

error:
    {
        IPC_P2W_DATA_PARTY_LEAVE_ACK* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_LEAVE_ACK);
        Response->Header.Source = Server->IPCSocket->NodeID;
        Response->Header.Target = Packet->Header.Source;
        Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
        Response->Result = 0;
        IPCSocketUnicast(Socket, Response);
    }
}
