#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// TODO: Add character connect, disconnect to update party states..

IPC_PROCEDURE_BINDING(W2P, PARTY_INIT) {
    IPC_P2W_DATA_PARTY_INIT* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INIT);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

    RTPartyRef Party = ServerGetPartyByCharacter(Context, Packet->CharacterIndex);
    if (Party) {
        Response->Result = 1;
        Response->DungeonIndex = 0;
        Response->PartyID = Party->ID;
        Response->PartyLeaderIndex = Party->LeaderCharacterIndex;
        Response->WorldServerIndex = 0;
        Response->MemberCount = Party->MemberCount;

        for (Index Index = 0; Index < Party->MemberCount; Index += 1) {
            memcpy(&Response->Members[Index], &Party->Members[Index], sizeof(struct _RTPartyMemberInfo));
        }

        Response->SoloDungeonIndex = 0;
        Response->SoloDungeonTimeout = 0;
    }
  
    IPCSocketUnicast(Socket, Response);
}
