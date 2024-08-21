#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(DELETE_CHARACTER) {
    if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->AccountID < 1) goto error;

    for (Int32 Index = 0; Index < MAX_CHARACTER_COUNT; Index++) {
        IPC_DATA_CHARACTER_INFO* Character = &Client->Characters[Index];
        if (Character->CharacterID < 1) continue;

        UInt32 CharacterIndex = Character->CharacterID * MAX_CHARACTER_COUNT + Index;
        if (CharacterIndex != Packet->CharacterIndex) continue;

        IPC_W2D_DATA_DELETE_CHARACTER* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, DELETE_CHARACTER);
        Request->Header.SourceConnectionID = Client->Connection->ID;
        Request->Header.Source = Server->IPCSocket->NodeID;
        Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
        Request->Header.Target.Type = IPC_TYPE_MASTERDB;
        Request->AccountID = Client->AccountID;
        Request->CharacterID = Character->CharacterID;
        IPCSocketUnicast(Server->IPCSocket, Request);
        return;
    }

    S2C_DATA_DELETE_CHARACTER* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DELETE_CHARACTER);
    Response->CharacterStatus = S2C_CHARACTER_STATUS_DATA_CORRUPTED;
    Response->ForceGem = 0; // TODO: Add force gem amount to account
    SocketSend(Socket, Connection, Response);
    return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, DELETE_CHARACTER) {
    if (!ClientConnection) return;

    for (Int32 Index = 0; Index < MAX_CHARACTER_COUNT; Index++) {
        IPC_DATA_CHARACTER_INFO* Character = &Client->Characters[Index];
        if (Character->CharacterID < 1) continue;

        UInt32 CharacterIndex = Character->CharacterID * MAX_CHARACTER_COUNT + Index;
        if (Character->CharacterID != Packet->CharacterID) continue;

        memset(Character, 0, sizeof(IPC_DATA_CHARACTER_INFO));
        break;
    }

    S2C_DATA_DELETE_CHARACTER* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, DELETE_CHARACTER);
    Response->CharacterStatus = (Packet->Success) ? S2C_CHARACTER_STATUS_SUCCESS : S2C_CHARACTER_STATUS_DATABASE_ERROR;
    Response->ForceGem = 0; // TODO: Add force gem amount to account
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}
