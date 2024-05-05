#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, CLIENT_CONNECT) {
    IPC_P2W_DATA_CLIENT_CONNECT* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, CLIENT_CONNECT);
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
            memcpy(&Response->Members[Index], &Party->Members[Index].Info, sizeof(struct _RTPartyMemberInfo));
        }

        Response->SoloDungeonIndex = 0;
        Response->SoloDungeonTimeout = 0;
    }
  
    IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2P, CLIENT_DISCONNECT) {
    RTPartyRef Party = ServerGetPartyByCharacter(Context, Packet->CharacterIndex);
    if (!Party) return;

    // TODO: Check if party has to be disbanded...
}