#include "AuthSocket.h"
#include "WorldSocket.h"
#include "IPCProtocol.h"
#include "MasterDB.h"
#include "Server.h"

ClientContextRef ServerGetWorldClient(
    ServerRef Server,
    ServerContextRef Context,
    Int32 WorldID
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->WorldSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->WorldSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->WorldSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client->World.IsWorldInitialized && Client->World.WorldID == WorldID) {
            return Client;
        }
    }

    return NULL;
}

Void ServerBroadcastWorldList(
    ServerRef Server,
    ServerContextRef Context
) {
    IPC_DATA_AUTH_NFYWORLDLIST* Notification = PacketBufferInitExtended(Context->AuthSocket->PacketBuffer, IPC, AUTH_NFYWORLDLIST);
    Notification->WorldCount = 0;

    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->WorldSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->WorldSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->WorldSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client->World.IsWorldInitialized) {
            IPC_DATA_AUTH_WORLD* World = PacketBufferAppendStruct(Context->AuthSocket->PacketBuffer, IPC_DATA_AUTH_WORLD);
            World->WorldID = Client->World.WorldID;
            memcpy(World->WorldHost, Client->World.WorldHost, sizeof(World->WorldHost));
            World->WorldPort = Client->World.WorldPort;
            World->WorldType = Client->World.WorldType;
            World->PlayerCount = Client->World.PlayerCount;
            World->MaxPlayerCount = Client->World.MaxPlayerCount;

            Notification->WorldCount += 1;
        }
    }

    SocketSendAll(Context->AuthSocket, Notification);
}
