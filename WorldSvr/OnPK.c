#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(PKREQUEST) {

    if (!Character) goto error;

    S2C_DATA_PKREQUEST* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PKREQUEST);
    Response->Command = S2C_PKREQUEST;
    Response->Result = 1;

    ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->CharacterId, NULL);
    if (!TargetClient) goto error;

    RTCharacterRef TargetCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->CharacterId);
    if (!TargetCharacter) goto error;
    if (!RTCharacterIsAlive(Runtime, TargetCharacter)) goto error;

    Response->Result = 0;

    S2C_DATA_NFY_PKREQUEST* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PKREQUEST);
    Notification->CharacterId = (UInt32)Character->CharacterIndex;

    SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);

    SocketSend(Socket, Connection, Response);

error:
    SocketSend(Socket, Connection, Response);
    return;
}


CLIENT_PROCEDURE_BINDING(PKENDING) {
    if (!Character) goto error;

    S2C_DATA_PKENDING* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PKENDING);
    Response->Command = S2C_PKENDING;
    Response->Result = 1;

    ClientContextRef TargetClient = ServerGetClientByIndex(Context, Packet->CharacterId, NULL);
    if (!TargetClient) goto error;

    RTCharacterRef TargetCharacter = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Packet->CharacterId);
    if (!TargetCharacter) goto error;
    if (!RTCharacterIsAlive(Runtime, TargetCharacter)) goto error;

    Response->Result = 0;

    S2C_DATA_NFY_PKENDING* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_PKENDING);
    Notification->CharacterId = (UInt32)Character->CharacterIndex;
    Notification->Result = Packet->Result;

    SocketSend(Context->ClientSocket, TargetClient->Connection, Notification);

    SocketSend(Socket, Connection, Response);

error:
    SocketSend(Socket, Connection, Response);
    return;
}
