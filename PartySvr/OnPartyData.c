#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// TODO: Check if the client notification takes a dynamic list of members which are changed instead of sending all
IPC_PROCEDURE_BINDING(W2P, PARTY_DATA) {
    RTPartyRef Party = ServerGetPartyByCharacter(Context, Packet->MemberInfo.CharacterIndex);
    if (!Party) return;

    for (Index Index = 0; Index < Party->MemberCount; Index += 1) {
        if (Party->Members[Index].Info.CharacterIndex != Packet->MemberInfo.CharacterIndex) continue;

        memcpy(&Party->Members[Index].Info, &Packet->MemberInfo, sizeof(struct _RTPartyMemberInfo));
        break;
    }

    BroadcastPartyData(Server, Context, Socket, Connection, Party);
}
