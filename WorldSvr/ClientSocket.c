#include "ClientCommands.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
#include "Server.h"

Void ClientSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ClientContextRef Client = (ClientContextRef)ConnectionContext;
    Client->Connection = Connection;
    Client->AccountID = -1;
    Client->CharacterIndex = 0;
    Client->DiceSeed = (UInt32)PlatformGetTickCount();
}

Void ClientSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    ClientContextRef Client = (ClientContextRef)ConnectionContext;
    if (Client->CharacterIndex > 0) {
        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, Client->CharacterIndex);
        if (Character) {
            RTCharacterUpdateBuffs(Context->Runtime, Character, true);
            ServerSyncCharacter(Server, Context, Client, Character);

            // TODO: @DungeonCleanUp Delete character dungeon instance and respawn to global world
            RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Context->Runtime, Character);
            RTWorldDespawnCharacter(WorldContext->WorldManager->Runtime, WorldContext, Character->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);
            RTWorldManagerDestroyCharacter(WorldContext->WorldManager, Client->CharacterIndex);
        }

        IPC_W2P_DATA_CLIENT_DISCONNECT* RequestParty = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, CLIENT_DISCONNECT);
        RequestParty->Header.Source = Server->IPCSocket->NodeID;
        RequestParty->Header.SourceConnectionID = Client->Connection->ID;
        RequestParty->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
        RequestParty->Header.Target.Type = IPC_TYPE_PARTY;
        RequestParty->CharacterIndex = Client->CharacterIndex;
        IPCSocketUnicast(Server->IPCSocket, RequestParty);

        IPC_W2C_DATA_CLIENT_DISCONNECT* RequestChat = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2C, CLIENT_DISCONNECT);
        RequestChat->Header.Source = Server->IPCSocket->NodeID;
        RequestChat->Header.SourceConnectionID = Client->Connection->ID;
        RequestChat->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
        RequestChat->Header.Target.Type = IPC_TYPE_CHAT;
        RequestChat->CharacterIndex = Client->CharacterIndex;
        IPCSocketUnicast(Server->IPCSocket, RequestChat);
    }
    
    if (Client->AccountID > 0) {
        IPC_N2M_DATA_CLIENT_DISCONNECT* Notification = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, N2M, CLIENT_DISCONNECT);
        Notification->Header.Source = Server->IPCSocket->NodeID;
        Notification->Header.Target.Group = Server->IPCSocket->NodeID.Group;
        Notification->Header.Target.Type = IPC_TYPE_MASTER;
        Notification->AccountID = Client->AccountID;
        IPCSocketUnicast(Server->IPCSocket, Notification);
    }

    Info("Client disconnected...");
}
