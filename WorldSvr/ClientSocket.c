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
        RTCharacterUpdateBuffs(Context->Runtime, Character, true);
        ServerSyncCharacter(Server, Context, Client, Character);

        IPC_W2C_DATA_CLIENT_DISCONNECT* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2C, CLIENT_DISCONNECT);
        Request->Header.Source = Server->IPCSocket->NodeID;
        Request->Header.SourceConnectionID = Client->Connection->ID;
        Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
        Request->Header.Target.Type = IPC_TYPE_CHAT;
        Request->CharacterIndex = Character->CharacterIndex;
        IPCSocketUnicast(Server->IPCSocket, Request);

        // TODO: @DungeonCleanUp Delete character dungeon instance and respawn to global world
        RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Context->Runtime, Character);
        RTWorldDespawnCharacter(WorldContext->WorldManager->Runtime, WorldContext, Character->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);
        RTWorldManagerDestroyCharacter(WorldContext->WorldManager, Client->CharacterIndex);

        Client->CharacterIndex = 0;
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
