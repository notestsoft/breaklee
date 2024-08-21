#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(VERIFY_LINKS) {
    if (!(Client->Flags & CLIENT_FLAGS_AUTHENTICATED)) goto error;

    if (Context->Config.Login.CaptchaVerificationEnabled) {
        if (!(Client->Flags & CLIENT_FLAGS_CAPTCHA_VERIFIED)) goto error;
    }

    if (Client->Flags & CLIENT_FLAGS_VERIFIED) goto error;

    Info("Client Magic Key: %d", Packet->ClientMagicKey);

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
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.SourceConnectionID = Connection->ID;
    Request->Header.Target.Group = Packet->ServerID;
    Request->Header.Target.Index = Packet->WorldID;
    Request->Header.Target.Type = IPC_TYPE_WORLD;
    Request->AccountID = Client->AccountID;
    Request->AuthKey = Packet->AuthKey;
    Request->EntityID = Packet->EntityID;
    Request->NodeIndex = Packet->WorldID;
    Request->GroupIndex = Packet->ServerID;
    memcpy(Request->SessionIP, Connection->AddressIP, MAX_ADDRESSIP_LENGTH);
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;
    
error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(W2L, VERIFY_LINKS) {
    S2C_DATA_VERIFY_LINKS* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, VERIFY_LINKS);
    Response->WorldID = Packet->Header.Source.Index;
    Response->ServerID = Packet->Header.Source.Group;
    Response->Status = Packet->Status;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
    SocketDisconnect(Context->ClientSocket, ClientConnection);
}

IPC_PROCEDURE_BINDING(W2L, WORLD_VERIFY_LINKS) {
    Client = ServerGetClientByAuthKey(Context, Packet->AuthKey, Packet->EntityID);
    if (!Client) goto error;

    ClientConnection = Client->Connection;
    assert(Client->Connection);

    if (Client->AccountStatus != ACCOUNT_STATUS_NORMAL || Client->LoginStatus != LOGIN_STATUS_SUCCESS) goto error;

    S2C_DATA_AUTH_TIMER* AuthTimerResponse = PacketBufferInit(ClientConnection->PacketBuffer, S2C, AUTH_TIMER);
    AuthTimerResponse->Timeout = Context->Config.Login.AutoDisconnectDelay;
    SocketSend(Context->ClientSocket, ClientConnection, AuthTimerResponse);

    Client->Flags |= CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
    Client->Flags |= CLIENT_FLAGS_CAPTCHA_VERIFIED;
    Client->Flags |= CLIENT_FLAGS_AUTHENTICATED;
    Client->DisconnectTimestamp = GetTimestampMs() + Context->Config.Login.AutoDisconnectDelay;

    IPC_L2M_DATA_GET_WORLD_LIST* Request = IPCPacketBufferInit(Socket->PacketBuffer, L2M, GET_WORLD_LIST);
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Server->IPCSocket->NodeID.Group;
    Request->Header.Target.Type = IPC_TYPE_MASTER;
    IPCSocketUnicast(Socket, Request);
    return;

error:
    {
        IPC_L2W_DATA_WORLD_VERIFY_LINKS* Response = IPCPacketBufferInit(Connection->PacketBuffer, L2W, WORLD_VERIFY_LINKS);
        Response->Header.Source = Server->IPCSocket->NodeID;
        Response->Header.Target = Packet->Header.Source;
        Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
        Response->Success = false;
        IPCSocketUnicast(Socket, Response);
    }
}