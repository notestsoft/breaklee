#include "AuthDB.h"
#include "ClientSocket.h"
#include "MasterSocket.h"
#include "Notification.h"
#include "Server.h"

MasterContextRef ServerGetMaster(
    ServerRef Server,
    ServerContextRef Context,
    Int32 ServerID
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->MasterSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->MasterSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        MasterContextRef Master = (MasterContextRef)Connection->Userdata;

        if (Master->ServerID == ServerID) return Master;
    }

    return NULL;
}

MasterContextWorldRef ServerGetWorld(
    ServerRef Server,
    ServerContextRef Context,
    Int32 ServerID,
    Int32 WorldID
) {
    MasterContextRef Master = ServerGetMaster(Server, Context, ServerID);
    if (!Master) return NULL;

    for (Int32 WorldIndex = 0; WorldIndex < ArrayGetElementCount(Master->Worlds); WorldIndex += 1) {
        MasterContextWorldRef World = (MasterContextWorldRef)ArrayGetElementAtIndex(
            Master->Worlds,
            WorldIndex
        );

        if (World->WorldID == WorldID) return World;
    }

    return NULL;
}
