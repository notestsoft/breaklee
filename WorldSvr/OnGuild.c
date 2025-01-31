#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GUILD_INITIALIZE) {
	// TODO: Implementation missing!
}

CLIENT_PROCEDURE_BINDING(GUILD_CREATE) {
    if (!Character) goto error;

    IPC_W2D_DATA_GUILD_CREATE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, GUILD_CREATE);
    Request->Header.SourceConnectionID = Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB;
    Request->CharacterIndex = Character->CharacterIndex;
    memcpy(Request->GuildName, Packet->GuildName, sizeof(Request->GuildName));
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, GUILD_CREATE) {
    if (!ClientConnection || !Character) return;

    S2C_DATA_GUILD_CREATE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, GUILD_CREATE);
    Response->Result = Packet->Result;
    Response->Unknown1 = Packet->Unknown1;
    memcpy(Response->GuildName, Packet->GuildName, sizeof(Response->GuildName));
    Response->GuildIndex = Packet->GuildIndex;
    Response->GuildCurrency = Packet->GuildCurrency;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}
