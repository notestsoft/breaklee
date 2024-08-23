#include "Constants.h"
#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProcedures.h"

Void StartAuthTimer(
    ServerRef Server,
    ServerContextRef Context,
    SocketRef Socket,
    SocketConnectionRef Connection,
    ClientContextRef Client,
    UInt32 Timeout
) {
    S2C_DATA_AUTH_TIMER* Response = PacketBufferInit(Connection->PacketBuffer, S2C, AUTH_TIMER);
    Response->Timeout = Timeout;
    SocketSend(Socket, Connection, Response);

    Client->Flags |= CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
    Client->DisconnectTimestamp = GetTimestampMs() + Timeout;
}

Void SendURLList(
    ServerRef Server,
    ServerContextRef Context,
    SocketRef Socket,
    SocketConnectionRef Connection,
    ClientContextRef Client
) {
    S2C_DATA_URL_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, URL_LIST);
    
    S2C_DATA_URL *Url = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Itemshop);
    PacketBufferAppendCString(Connection->PacketBuffer, Context->Config.Links.Itemshop);
    
    Url = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Unknown1);
    PacketBufferAppendCString(Connection->PacketBuffer, Context->Config.Links.Unknown1);
    
    Url = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Unknown2);
    PacketBufferAppendCString(Connection->PacketBuffer, Context->Config.Links.Unknown2);
    
    Url = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Guild);
    PacketBufferAppendCString(Connection->PacketBuffer, Context->Config.Links.Guild);
    
    Url = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.SNS);
    PacketBufferAppendCString(Connection->PacketBuffer, Context->Config.Links.SNS);
    
    Url = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Unknown3);
    PacketBufferAppendCString(Connection->PacketBuffer, Context->Config.Links.Unknown3);
    
    Url = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Unknown4);
    PacketBufferAppendCString(Connection->PacketBuffer, Context->Config.Links.Unknown4);

    Response->PayloadLength[0] = (
        Response->Length -
        sizeof(S2C_DATA_URL_LIST) +
        sizeof(Response->PayloadLength[1])
    );
    Response->PayloadLength[1] = (
        Response->Length -
        sizeof(S2C_DATA_URL_LIST)
    );

    SocketSend(Socket, Connection, Response);
}

Void SendMessageLoginSuccess(
    ServerRef Server,
    ServerContextRef Context,
    SocketRef Socket,
    SocketConnectionRef Connection,
    ClientContextRef Client
) {
    S2C_DATA_SYSTEM_MESSAGE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SYSTEM_MESSAGE);
    Response->Message = SYSTEM_MESSAGE_LOGIN_SUCCESS;
    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(AUTHENTICATE) {
    if (!(Client->Flags & CLIENT_FLAGS_AUTHORIZED)) {
        SocketDisconnect(Socket, Connection);
        return;
    }

    if (Packet->SubMessageType == 25) {
        S2C_DATA_AUTHENTICATE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, AUTHENTICATE);
        Response->KeepAlive = 1;
        Response->SubMessageType = 25;
        Response->LoginStatus = Client->LoginStatus;
        Response->AccountStatus = Client->AccountStatus;

        S2C_DATA_AUTHENTICATE_EXTENSION_UNKNOWN* ResponseData = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_AUTHENTICATE_EXTENSION_UNKNOWN);
        SocketSend(Socket, Connection, Response);
        return;
    }

    assert(Client->RSA);
    Int32 Length = RSA_size(Client->RSA);
    Int32 DecryptedPayloadLength = RSA_private_decrypt(
        Length,
        Packet->Payload,
        Client->RSAPayloadBuffer,
        Client->RSA,
        RSA_PKCS1_OAEP_PADDING
    );

    if (CLIENT_RSA_PAYLOAD_LENGTH != DecryptedPayloadLength) goto error;

    CString Username = (CString)&Client->RSAPayloadBuffer[0];
    Int32 UsernameLength = (Int32)strlen(Username);
    if (UsernameLength > MAX_USERNAME_LENGTH) goto error;

    CString Password = (CString)&Client->RSAPayloadBuffer[129];
    Int32 PasswordLength = (Int32)strlen(Password);
    if (PasswordLength > MAX_PASSWORD_LENGTH) goto error;

    Char PasswordHash[SALTED_HASH_LENGTH] = { 0 };
    Bool InsertedAccount = false;

authenticate:
    GenerateRandomKey(Client->SessionKey, sizeof(Client->SessionKey));
    Bool Success = DatabaseCallProcedure(
        Context->Database,
        "Authenticate",
        DB_INPUT_STRING(Username, strlen(Username)),
        DB_INPUT_STRING(Connection->AddressIP, strlen(Connection->AddressIP)),
        DB_INPUT_STRING(Client->SessionKey, strlen(Client->SessionKey)),
        DB_INPUT_BOOL(Context->Config.Login.EmailVerificationEnabled),
        DB_OUTPUT_INT32(Client->LoginStatus),
        DB_OUTPUT_INT32(Client->AccountStatus),
        DB_OUTPUT_INT32(Client->AccountID),
        DB_OUTPUT_DATA(PasswordHash, SALTED_HASH_LENGTH),
        DB_PARAM_END
    );
    if (!Success) {
        Client->LoginStatus = LOGIN_STATUS_ERROR;
        Client->AccountStatus = ACCOUNT_STATUS_OUT_OF_SERVICE;
    }

    if (Client->LoginStatus == LOGIN_STATUS_SUCCESS && Client->AccountStatus == ACCOUNT_STATUS_NORMAL) {
        if (!ValidatePasswordHash(Password, PasswordHash)) {
            Client->LoginStatus = LOGIN_STATUS_ERROR;
            Client->AccountStatus = ACCOUNT_STATUS_INVALID_CREDENTIALS;
        }        
    }

    if (!InsertedAccount && Context->Config.Login.AutoCreateAccountOnLogin &&
        Client->AccountStatus == ACCOUNT_STATUS_INVALID_CREDENTIALS) {
        if (CreatePasswordHash(Password, PasswordHash)) {
            DatabaseCallProcedure(
                Context->Database,
                "InsertAccount",
                DB_INPUT_STRING(Username, strlen(Username)),
                DB_INPUT_STRING(Username, strlen(Username)),
                DB_INPUT_DATA(PasswordHash, SALTED_HASH_LENGTH),
                DB_PARAM_END
            );
        }

        InsertedAccount = true;
        goto authenticate;
    }

    S2C_DATA_AUTHENTICATE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, AUTHENTICATE);
    Response->KeepAlive = (Client->AccountStatus == ACCOUNT_STATUS_NORMAL) ? 1 : 0;
    Response->LoginStatus = Client->LoginStatus;
    Response->AccountStatus = Client->AccountStatus;
    SocketSend(Socket, Connection, Response);

    if (Client->LoginStatus != LOGIN_STATUS_SUCCESS) goto error;

    Client->Flags |= CLIENT_FLAGS_AUTHENTICATED;

    StartAuthTimer(Server, Context, Socket, Connection, Client, Context->Config.Login.AutoDisconnectDelay);
    SendURLList(Server, Context, Socket, Connection, Client);

    Response = PacketBufferInit(Connection->PacketBuffer, S2C, AUTHENTICATE);
    Response->KeepAlive = (Client->AccountStatus == ACCOUNT_STATUS_NORMAL) ? 1 : 0;
    Response->LoginStatus = Client->LoginStatus;
    Response->AccountStatus = Client->AccountStatus;
    Response->SubMessageType = 17;

    S2C_DATA_AUTHENTICATE_EXTENSION* Extension = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_AUTHENTICATE_EXTENSION);
    memcpy(Extension->AuthKey, Client->SessionKey, MAX_SESSIONKEY_LENGTH);

    Int32 ServerCount = 0;
    DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
        Context->Database,
        "GetServerCharacterList",
        DB_INPUT_INT32(Client->AccountID),
        DB_OUTPUT_INT32(ServerCount),
        DB_PARAM_END
    );

    Int32 ServerIndex = 0;
    while (DatabaseHandleReadNext(
        Handle,
        DB_TYPE_UINT8, &Extension->Servers[ServerIndex].ServerID, sizeof(UInt8),
        DB_TYPE_UINT8, &Extension->Servers[ServerIndex].CharacterCount, sizeof(UInt8),
        DB_PARAM_END
    )) {
        assert(ServerIndex < MAX_SERVER_COUNT);
        ServerIndex += 1;
    }

    SocketSend(Socket, Connection, Response);

    StartAuthTimer(Server, Context, Socket, Connection, Client, Context->Config.Login.AutoDisconnectDelay);
    SendMessageLoginSuccess(Server, Context, Socket, Connection, Client);

    // Just clearing the payload buffer to avoid keeping sensitive data in memory!
    memset(Client->RSAPayloadBuffer, 0, sizeof(Client->RSAPayloadBuffer));

    IPC_L2M_DATA_GET_WORLD_LIST* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, L2M, GET_WORLD_LIST);
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Server->IPCSocket->NodeID.Group;
    Request->Header.Target.Type = IPC_TYPE_MASTER;
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
    // Just clearing the payload buffer to avoid keeping sensitive data in memory!
    memset(Client->RSAPayloadBuffer, 0, sizeof(Client->RSAPayloadBuffer));

    SocketDisconnect(Socket, Connection);
}
