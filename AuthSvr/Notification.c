#include "ClientCommands.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
#include "Notification.h"

S2C_DATA_SERVER_LIST* BuildWorldListNotification(
    ServerRef Server,
    ServerContextRef Context,
    Bool IsLocalHost
) {
    S2C_DATA_SERVER_LIST* Notification = PacketInit(S2C_DATA_SERVER_LIST);
    Notification->Command = S2C_SERVER_LIST;
    Notification->ServerCount = 0;

    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->MasterSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->MasterSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->MasterSocket, Iterator);

        MasterContextRef Master = (MasterContextRef)Connection->Userdata;

        S2C_DATA_SERVER_LIST_INDEX* NotificationServer = PacketAppendStruct(S2C_DATA_SERVER_LIST_INDEX);
        NotificationServer->ServerID = Master->ServerID;
        NotificationServer->WorldCount = 0;

        for (Int32 WorldIndex = 0; WorldIndex < ArrayGetElementCount(Master->Worlds); WorldIndex += 1) {
            MasterContextWorldRef MasterWorld = (MasterContextWorldRef)ArrayGetElementAtIndex(Master->Worlds, WorldIndex);

            S2C_DATA_SERVER_LIST_WORLD* NotificationWorld = PacketAppendStruct(S2C_DATA_SERVER_LIST_WORLD);
            NotificationWorld->ServerID = NotificationServer->ServerID;
            NotificationWorld->WorldID = MasterWorld->WorldID;
            NotificationWorld->PlayerCount = MasterWorld->PlayerCount;
            NotificationWorld->MaxPlayerCount = MasterWorld->MaxPlayerCount;
            memcpy(NotificationWorld->WorldHost, MasterWorld->WorldHost, sizeof(MasterWorld->WorldHost));
            NotificationWorld->WorldPort = MasterWorld->WorldPort;
            NotificationWorld->WorldType = MasterWorld->WorldType;

            if (IsLocalHost) {
                Char LocalHost[] = "127.0.0.1";
                memcpy(NotificationWorld->WorldHost, LocalHost, sizeof(LocalHost));
            }

            NotificationServer->WorldCount += 1;
        }

        NotificationServer->WorldCount += 1;
        S2C_DATA_SERVER_LIST_WORLD* ZeroWorld = PacketAppendStruct(S2C_DATA_SERVER_LIST_WORLD);

        Notification->ServerCount += 1;
    }

    return Notification;
}

S2C_DATA_UNKNOWN_124* BuildMessageUnknown124(
    ServerRef Server
) {
    S2C_DATA_UNKNOWN_124* Response = PacketInit(S2C_DATA_UNKNOWN_124);
    Response->Command = S2C_UNKNOWN_124;
    Response->Unknown1 = 0;
    Response->Unknown2[0] = 100;
    Response->Unknown2[1] = 200;
    Response->Unknown2[2] = 300;
    Response->Unknown2[3] = 400;
    Response->Unknown3 = 1;
    Response->Unknown4[0] = 500;
    Response->Unknown4[1] = 600;
    Response->Unknown4[2] = 700;
    Response->Unknown4[3] = 800;
    return Response;
}

Void BroadcastWorldList(
    ServerRef Server,
    ServerContextRef Context
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        Bool IsLocalHost = strcmp(Client->Connection->AddressIP, "127.0.0.1") == 0;
        S2C_DATA_SERVER_LIST* Notification = BuildWorldListNotification(Server, Context, IsLocalHost);

        Bool Authorized = (
            (Client->Flags & CLIENT_FLAGS_AUTHENTICATED) &&
            !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED)
        );
        if (Authorized) {
            SocketSend(Context->ClientSocket, Connection, Notification);
        }
    }

    S2C_DATA_UNKNOWN_124* Response = BuildMessageUnknown124(Server);

    Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;

        Bool Authorized = (
            (Client->Flags & CLIENT_FLAGS_AUTHENTICATED) &&
            !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED)
        );
        if (Authorized) {
            SocketSend(Context->ClientSocket, Connection, Response);
        }
    }
}

Void BroadcastWorldListToConnection(
    ServerRef Server,
    ServerContextRef Context,
    SocketConnectionRef Connection,
    ClientContextRef Client
) {
    Bool Authorized = (
        (Client->Flags & CLIENT_FLAGS_AUTHENTICATED) &&
        !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED)
    );
    if (Authorized) {
        Bool IsLocalHost = strcmp(Client->Connection->AddressIP, "127.0.0.1") == 0;
        S2C_DATA_SERVER_LIST* Notification = BuildWorldListNotification(Server, Context, IsLocalHost);
        SocketSend(Context->ClientSocket, Connection, Notification);

        S2C_DATA_UNKNOWN_124* Response = BuildMessageUnknown124(Server);
        SocketSend(Context->ClientSocket, Connection, Notification);
    }
}
