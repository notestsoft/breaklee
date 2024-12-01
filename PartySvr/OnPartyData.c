#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// TODO: Check if the client notification takes a dynamic list of members which are changed instead of sending all
IPC_PROCEDURE_BINDING(W2P, PARTY_DATA) {
    RTPartyRef Party = RTPartyManagerGetPartyByCharacter(Context->PartyManager, Packet->MemberInfo.CharacterIndex);
    if (!Party) return;

    for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
        if (Party->Members[Index].CharacterIndex != Packet->MemberInfo.CharacterIndex) continue;

        memcpy(&Party->Members[Index], &Packet->MemberInfo, sizeof(struct _RTPartyMemberInfo));
        break;
    }

    BroadcastPartyData(Server, Context, Socket, Party);
}
