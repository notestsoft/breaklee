#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, CLIENT_CONNECT) {
    Int* CharacterWorldServerIndex = DictionaryLookup(Context->CharacterToWorldServer, &Packet->CharacterIndex);
    if (!CharacterWorldServerIndex) {
        DictionaryInsert(Context->CharacterToWorldServer, &Packet->CharacterIndex, &Packet->Header.Source.Index, sizeof(Int));
        CharacterWorldServerIndex = DictionaryLookup(Context->CharacterToWorldServer, &Packet->CharacterIndex);
        assert(CharacterWorldServerIndex);
    }
    else {
        *CharacterWorldServerIndex = Packet->Header.Source.Index;
    }
    
    IPC_P2W_DATA_CLIENT_CONNECT* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, CLIENT_CONNECT);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

    RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Context->PartyManager, Packet->CharacterIndex);
    if (Party) {
        RTPartyMemberInfoRef Member = RTPartyGetMember(Party, Packet->CharacterIndex);
        Member->WorldServerIndex = (CharacterWorldServerIndex) ? *CharacterWorldServerIndex : 0;

        Response->Result = 1;
        Response->DungeonIndex = 0;
        Response->PartyID = Party->ID;
        Response->PartyLeaderIndex = Party->LeaderCharacterIndex;
        Response->WorldServerIndex = Party->WorldServerIndex;
        Response->MemberCount = Party->MemberCount;

        for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
            memcpy(&Response->Members[Index], &Party->Members[Index], sizeof(struct _RTPartyMemberInfo));
        }

        Response->SoloDungeonIndex = 0;
        Response->SoloDungeonTimeout = 0;
    }
  
    IPCSocketUnicast(Socket, Response);

    if (Party) {
        BroadcastPartyData(Server, Context, Server->IPCSocket, Party);
    }
}

IPC_PROCEDURE_BINDING(W2P, CLIENT_DISCONNECT) {
    if (!DictionaryLookup(Context->CharacterToWorldServer, &Packet->CharacterIndex)) return;

    DictionaryRemove(Context->CharacterToWorldServer, &Packet->CharacterIndex);
    
    RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Context->PartyManager, Packet->CharacterIndex);
    if (!Party) return;

    Int OnlineMemberCount = 0;
    for (Int MemberIndex = 0; MemberIndex < Party->MemberCount; MemberIndex += 1) {
        UInt32 CharacterIndex = Party->Members[MemberIndex].CharacterIndex;
        if (DictionaryLookup(Context->CharacterToWorldServer, &CharacterIndex)) {
            OnlineMemberCount += 1;
        }
    }
    
    if (OnlineMemberCount < 1) {
        // TODO: Notify world to close the dungeon instance
        BroadcastDestroyParty(Server, Context, Server->IPCSocket, Party);
        RTPartyManagerDestroyParty(Context->PartyManager, Party);
    }
    else {
        RTPartyMemberInfoRef Member = RTPartyGetMember(Party, Packet->CharacterIndex);
        Member->WorldServerIndex = 0;
        BroadcastPartyData(Server, Context, Server->IPCSocket, Party);
    }
}
