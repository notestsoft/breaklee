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

    IPC_P2W_DATA_PARTY_DATA* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_DATA);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
    Response->Header.Target.Type = IPC_TYPE_WORLD;
    Response->MemberCount = Party->MemberCount;

    for (Index Index = 0; Index < Party->MemberCount; Index += 1) {
        memcpy(&Response->MemberInfo[Index], &Party->Members[Index].Info, sizeof(struct _RTPartyMemberInfo));
    }

    IPCSocketBroadcast(Socket, Response);
}
