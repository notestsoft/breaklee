#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"

CLIENT_PROCEDURE_BINDING(PUBLIC_KEY) {
    if (!(Client->Flags & CLIENT_FLAGS_USERNAME_CHECKED)) {
        SocketDisconnect(Socket, Connection);
        return;
    }

    if (Client->PublicKeyCipher) {
        CLPublicKeyCipherDestroy(Client->PublicKeyCipher);
        Client->PublicKeyCipher = NULL;
    }

    Client->PublicKeyCipher = CLPublicKeyCipherCreate(Context->Allocator);
    if (!Client->PublicKeyCipher) {
        SocketDisconnect(Socket, Connection);
        return;
    }

    Client->Flags |= CLIENT_FLAGS_PUBLICKEY_INITIALIZED;

    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
    S2C_DATA_PUBLIC_KEY* Response = PacketBufferInit(PacketBuffer, S2C, PUBLIC_KEY);
    Response->Unknown1 = 1;
    Response->PublicKeyLength = (UInt16)CLPublicKeyCipherGetKeyLength(Client->PublicKeyCipher);
    PacketBufferAppendCopy(PacketBuffer, CLPublicKeyCipherGetKey(Client->PublicKeyCipher), Response->PublicKeyLength);
    SocketSend(Socket, Connection, Response);
}
