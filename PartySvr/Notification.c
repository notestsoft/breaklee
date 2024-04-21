#include "ClientProtocol.h"
#include "IPCProtocol.h"
#include "Notification.h"
#include "Server.h"

Void BroadcastToParty(
    ServerContextRef Context,
    RTEntityID PartyID,
    Void *Notification
) {
    /*
    RTPartyRef Party = RTRuntimeGetParty(Context->Runtime, PartyID);
    if (!Party) return;

    for (Int32 Index = 0; Index < Party->MemberCount; Index += 1) {
        RTPartySlotRef Slot = &Party->Members[Index];
        if (RTEntityIsNull(Slot->MemberID)) continue;

        RTCharacterRef Character = RTWorldManagerGetCharacter(Context->Runtime->WorldManager, Slot->MemberID);
        if (!Character) continue;

        ClientContextRef Client = ServerGetClientByEntity(Context, Character->ID);
        if (!Client) continue;

        SocketSend(Context->ClientSocket, Client->Connection, Notification);
    }
    */
}

Void BroadcastPartyInfo(
    ServerRef Server,
    ServerContextRef Context,
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    RTPartyRef Party
) {
    IPC_P2W_DATA_PARTY_INFO* Notification = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INFO);
    Notification->Header.Source = Server->IPCSocket->NodeID;
    Notification->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
    Notification->Header.Target.Type = IPC_TYPE_WORLD;
    memcpy(&Notification->Party, Party, sizeof(struct _RTParty));
    IPCSocketBroadcast(Socket, Notification);
}