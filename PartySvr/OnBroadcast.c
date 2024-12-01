#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, BROADCAST_TO_PARTY) {
    RTPartyRef Party = RTPartyManagerGetParty(Context->PartyManager, Packet->PartyID);
    if (!Party) return;

    for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
        RTPartyMemberInfoRef Member = &Party->Members[Index];

        IPC_P2W_DATA_BROADCAST_TO_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, BROADCAST_TO_CHARACTER);
        Response->Header.Source = Server->IPCSocket->NodeID;
        Response->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
        Response->Header.Target.Index = (UInt32)Member->WorldServerIndex;
        Response->Header.Target.Type = IPC_TYPE_WORLD;
        Response->CharacterIndex = Member->CharacterIndex;
        Response->Length = Packet->Length;
        IPCPacketBufferAppendCopy(Connection->PacketBuffer, Packet->Data, Packet->Length);
        IPCSocketUnicast(Socket, Response);
    }
}
