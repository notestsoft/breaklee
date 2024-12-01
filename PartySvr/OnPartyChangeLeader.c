#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_CHANGE_LEADER) {
    RTPartyRef Party = RTPartyManagerGetParty(Context->PartyManager, Packet->PartyID);
    if (!Party) goto error;

    IPC_P2W_DATA_PARTY_CHANGE_LEADER_ACK* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_CHANGE_LEADER_ACK);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    Response->Result = RTPartyChangeLeader(Party, Packet->CharacterIndex);
    IPCSocketUnicast(Socket, Response);

    if (Response->Result) {
        for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
            RTPartyMemberInfoRef Member = &Party->Members[Index];

            IPC_P2W_DATA_PARTY_CHANGE_LEADER* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_CHANGE_LEADER);
            Response->Header.Source = Server->IPCSocket->NodeID;
            Response->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
            Response->Header.Target.Index = (UInt32)Member->WorldServerIndex;
            Response->Header.Target.Type = IPC_TYPE_WORLD;
            Response->PartyID = Party->ID;
            Response->CharacterIndex = Packet->CharacterIndex;
            IPCSocketUnicast(Socket, Response);
        }
    }

    return;

error:
    {
        IPC_P2W_DATA_PARTY_CHANGE_LEADER_ACK* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_CHANGE_LEADER_ACK);
        Response->Header.Source = Server->IPCSocket->NodeID;
        Response->Header.Target = Packet->Header.Source;
        Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
        Response->Result = 0;
        IPCSocketUnicast(Socket, Response);
    }
}
