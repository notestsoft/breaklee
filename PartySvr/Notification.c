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

    for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
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
    RTPartyRef Party
) {
    IPC_P2W_DATA_PARTY_INFO* Notification = IPCPacketBufferInit(Socket->PacketBuffer, P2W, PARTY_INFO);
    Notification->Header.Source = Server->IPCSocket->NodeID;
    Notification->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
    Notification->Header.Target.Type = IPC_TYPE_WORLD;
    memcpy(&Notification->Party, Party, sizeof(struct _RTParty));
    IPCSocketBroadcast(Socket, Notification);
}

Void BroadcastCreateParty(
    ServerRef Server,
    ServerContextRef Context,
    IPCSocketRef Socket,
    RTPartyRef Party
) {
    IPC_P2W_DATA_CREATE_PARTY* Notification = IPCPacketBufferInit(Socket->PacketBuffer, P2W, CREATE_PARTY);
    Notification->Header.Source = Server->IPCSocket->NodeID;
    Notification->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
    Notification->Header.Target.Type = IPC_TYPE_WORLD;
    memcpy(&Notification->Party, Party, sizeof(struct _RTParty));
    IPCSocketBroadcast(Socket, Notification);
}

Void BroadcastDestroyParty(
    ServerRef Server,
    ServerContextRef Context,
    IPCSocketRef Socket,
    RTPartyRef Party
) {
    IPC_P2W_DATA_DESTROY_PARTY* Notification = IPCPacketBufferInit(Socket->PacketBuffer, P2W, DESTROY_PARTY);
    Notification->Header.Source = Server->IPCSocket->NodeID;
    Notification->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
    Notification->Header.Target.Type = IPC_TYPE_WORLD;
    memcpy(&Notification->Party, Party, sizeof(struct _RTParty));
    IPCSocketBroadcast(Socket, Notification);
}

Void BroadcastPartyData(
    ServerRef Server,
    ServerContextRef Context,
    IPCSocketRef Socket,
    RTPartyRef Party
) {
    IPC_P2W_DATA_PARTY_DATA* Response = IPCPacketBufferInit(Socket->PacketBuffer, P2W, PARTY_DATA);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
    Response->Header.Target.Type = IPC_TYPE_WORLD;
    Response->MemberCount = Party->MemberCount;

    for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
        memcpy(&Response->MemberInfo[Index], &Party->Members[Index], sizeof(struct _RTPartyMemberInfo));
    }

    IPCSocketBroadcast(Socket, Response);
}
