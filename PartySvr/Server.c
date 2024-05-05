#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

ClientContextRef ServerGetClientByIndex(
    ServerContextRef Context,
    UInt32 CharacterIndex,
    CString CharacterName,
    Int32 CharacterNameLength
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client && Client->CharacterIndex == CharacterIndex && memcmp(Client->CharacterName, CharacterName, CharacterNameLength) == 0) {
            return Client;
        }
    }

    return NULL;
}

RTPartyRef ServerCreateParty(
    ServerContextRef Context,
    Index CharacterIndex,
    RTEntityID CharacterID,
    Int32 PartyType
) {
    return RTPartyManagerCreateParty(
        Context->PartyManager,
        CharacterIndex,
        CharacterID,
        PartyType
    );
}

Void ServerDestroyParty(
    ServerContextRef Context,
    RTPartyRef Party
) {
    return RTPartyManagerDestroyParty(Context->PartyManager, Party);
}

RTPartyRef ServerGetPartyByCharacter(
    ServerContextRef Context,
    Index CharacterIndex
) {
    return RTPartyManagerGetPartyByCharacter(Context->PartyManager, CharacterIndex);
}

RTPartyRef ServerGetParty(
    ServerContextRef Context,
    RTEntityID PartyID
) {
    return RTPartyManagerGetParty(Context->PartyManager, PartyID);
}
