#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_CHARACTERS) {
    if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->AccountID < 1) goto error;

    // TODO: Check if is entering premium channel and check service expiration!

    IPC_W2D_DATA_GET_CHARACTER_LIST* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, GET_CHARACTER_LIST);
    Request->Header.SourceConnectionID = Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB;
    Request->AccountID = Client->AccountID;
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, GET_CHARACTER_LIST) {
    if (!ClientConnection || !Client || !(Client->Flags & CLIENT_FLAGS_VERIFIED)) return;

    memcpy(Client->Characters, Packet->Characters, sizeof(Packet->Characters));
    Client->Flags |= CLIENT_FLAGS_CHARACTER_INDEX_LOADED;

    S2C_DATA_GET_CHARACTERS* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, GET_CHARACTERS);
    Response->IsSubpasswordSet = Client->IsSubpasswordSet;
    Response->CanJoinBeginnerGuild = 0;
    Response->AccountInfo = Client->AccountInfo;
    memcpy(Response->Characters, Packet->Characters, sizeof(Packet->Characters));
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}
