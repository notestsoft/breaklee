#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, CLIENT_CONNECT) {
    Index* CharacterWorldServerIndex = DictionaryLookup(Context->CharacterToWorldServer, &Packet->CharacterIndex);
    if (CharacterWorldServerIndex) {
        *CharacterWorldServerIndex = Packet->CharacterIndex;
    }
    else {
        DictionaryInsert(Context->CharacterToWorldServer, &Packet->CharacterIndex, &Packet->Header.Source.Index, sizeof(Index));
    }
    
    IPC_P2W_DATA_CLIENT_CONNECT* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, CLIENT_CONNECT);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

    RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Context->PartyManager, Packet->CharacterIndex);
    if (Party) {
        Response->Result = 1;
        Response->DungeonIndex = 0;
        Response->PartyID = Party->ID;
        Response->PartyLeaderIndex = Party->LeaderCharacterIndex;
        Response->WorldServerIndex = Party->WorldServerIndex;
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
    if (!DictionaryLookup(Context->CharacterToWorldServer, &Packet->CharacterIndex)) return;

    DictionaryRemove(Context->CharacterToWorldServer, &Packet->CharacterIndex);
    
    RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Context->PartyManager, Packet->CharacterIndex);
    if (!Party) return;

    Index OnlineMemberCount = 0;
    for (Index MemberIndex = 0; MemberIndex < Party->MemberCount; MemberIndex += 1) {
        Index CharacterIndex = Party->Members[MemberIndex].Info.CharacterIndex;
        if (DictionaryLookup(Context->CharacterToWorldServer, &CharacterIndex)) {
            OnlineMemberCount += 1;
        }
    }
    
    if (OnlineMemberCount < 1) {
        // TODO: Notify world to close the dungeon instance
        RTPartyManagerDestroyParty(Context->PartyManager, Party);
    }
}
