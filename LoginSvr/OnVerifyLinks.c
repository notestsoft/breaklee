#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

CLIENT_PROCEDURE_BINDING(VERIFY_LINKS) {
    if (!(Client->Flags & CLIENT_FLAGS_AUTHENTICATED)) goto error;

    if (Client->Flags & CLIENT_FLAGS_VERIFIED) goto error;

    LogMessageFormat(LOG_LEVEL_INFO, "Client Magic Key: %d", Packet->ClientMagicKey);

    if (Context->Config.Login.ClientMagicKey != Packet->ClientMagicKey) goto error;

    /*
    MasterContextWorldRef World = ServerGetWorld(Server, Context, Packet->ServerID, Packet->WorldID);
    if (!World) goto error;

    Bool IsWorldFull = (World->PlayerCount >= World->MaxPlayerCount);
    if (IsWorldFull) goto error;
    */

    assert(Client->AccountID > 0);

    Client->Flags |= CLIENT_FLAGS_VERIFIED;

    // TODO: Add account premium service info

    IPC_L2W_DATA_VERIFY_LINKS* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, L2W, VERIFY_LINKS);
    Request->Header.SourceConnectionID = Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Packet->ServerID;
    Request->Header.Target.Index = Packet->WorldID;
    Request->Header.Target.Type = IPC_TYPE_WORLD;
    Request->AccountID = Client->AccountID;
    Request->AuthKey = Packet->AuthKey;
    Request->EntityID = Packet->EntityID;
    memcpy(Request->SessionIP, Connection->AddressIP, MAX_ADDRESSIP_LENGTH);
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;
    
error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(W2L, VERIFY_LINKS) {
	S2C_DATA_VERIFY_LINKS* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, VERIFY_LINKS);
	Response->WorldID = Packet->WorldID;
	Response->ServerID = Packet->ServerID;
    Response->Status = Packet->Status;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
	SocketDisconnect(Context->ClientSocket, ClientConnection);
}
