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
            Response->Members[Index].CharacterIndex = Party->Members[Index].CharacterIndex;
            Response->Members[Index].Level = Party->Members[Index].Level;
            Response->Members[Index].OverlordLevel = Party->Members[Index].OverlordLevel;
            Response->Members[Index].MythRebirth = Party->Members[Index].MythRebirth;
            Response->Members[Index].MythHolyPower = Party->Members[Index].MythHolyPower;
            Response->Members[Index].MythLevel = Party->Members[Index].MythLevel;
            Response->Members[Index].ForceWingGrade = Party->Members[Index].ForceWingGrade;
            Response->Members[Index].ForceWingLevel = Party->Members[Index].ForceWingLevel;
            CStringCopySafe(Response->Members[Index].Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Party->Members[Index].Name);
        }

        Response->SoloDungeonIndex = 0;
        Response->SoloDungeonTimeout = 0;
    }
  
    IPCSocketUnicast(Socket, Response);
}
