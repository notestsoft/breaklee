#include "AuthDB.h"
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
    Client->DisconnectTimestamp = ServerGetTimestamp(Server) + Timeout;
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

    S2C_DATA_AUTHENTICATE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, AUTHENTICATE);
    Response->KeepAlive = 1;
    Response->LoginStatus = LOGIN_STATUS_ERROR;
    Response->AccountStatus = ACCOUNT_STATUS_NORMAL;

    AUTHDB_DATA_ACCOUNT Account = { 0 };

    if (AuthDBSelectAccountByUsername(Context->Database, Username, &Account)) {
        if (Account.DeletedAt > 0) {
            Response->AccountStatus = ACCOUNT_STATUS_ACCOUNT_IS_DELETED;
        }
        else if (Context->Config.Login.EmailVerificationEnabled && !Account.EmailVerified) {
            Response->AccountStatus = ACCOUNT_STATUS_ACCOUNT_NOT_VERIFIED;
        }
        else {
            Bool InvalidCredentials = !ValidatePasswordHash(
                Password,
                Account.Salt,
                MAX_PASSWORD_SALT_LENGTH,
                Account.Hash,
                MAX_PASSWORD_HASH_LENGTH
            );

            if (InvalidCredentials) {
                Response->AccountStatus = ACCOUNT_STATUS_INVALID_CREDENTIALS;
            }
            else {
                Response->AccountStatus = ACCOUNT_STATUS_NORMAL;
                Response->LoginStatus = LOGIN_STATUS_SUCCESS;

                AUTHDB_DATA_SESSION Session = { 0 };
                if (AuthDBSelectSession(Context->Database, Account.ID, &Session) && Session.Online) {
                    Response->AccountStatus = ACCOUNT_STATUS_ALREADY_LOGGED_IN;
                }
            }

            AUTHDB_DATA_BLACKLIST Blacklist = { 0 };
            if (AuthDBSelectBlacklistByAccount(Context->Database, Account.ID, &Blacklist)) {
                Response->AccountStatus = ACCOUNT_STATUS_ACCOUNT_BANNED;
            }

            if (AuthDBSelectBlacklistByAddress(Context->Database, Connection->AddressIP, &Blacklist)) {
                Response->AccountStatus = ACCOUNT_STATUS_IP_BANNED;
            }
        }
    }
    else {
        Response->AccountStatus = ACCOUNT_STATUS_INVALID_CREDENTIALS;

        if (Context->Config.Login.AutoCreateAccountOnLogin) {
            UInt8* Salt = NULL;
            Int32 SaltLength = 0;
            UInt8 Hash[EVP_MAX_MD_SIZE] = { 0 };
            Int32 HashLength = 0;
            Bool Success = CreatePasswordHash(Password, &Salt, &SaltLength, &Hash[0], &HashLength);
            if (Success) Success &= AuthDBInsertAccount(Context->Database, Username, Username, Salt, SaltLength, Hash, HashLength);
            if (Success) Success &= AuthDBSelectAccountByUsername(Context->Database, Username, &Account);
            if (Success) {
                Response->AccountStatus = ACCOUNT_STATUS_NORMAL;
                Response->LoginStatus = LOGIN_STATUS_SUCCESS;
            }
        }

        // TODO: Add error handling to Database to differentiate "not found" from real errors
        // Response->AccountStatus = ACCOUNT_STATUS_OUT_OF_SERVICE;
    }

    Client->AccountID = Account.ID;
    Client->AccountStatus = Response->AccountStatus;
    Client->LoginStatus = Response->LoginStatus;

    SocketSend(Socket, Connection, Response);

    if (Response->LoginStatus != LOGIN_STATUS_SUCCESS) goto error;

    Client->Flags |= CLIENT_FLAGS_AUTHENTICATED;

    StartAuthTimer(Server, Context, Socket, Connection, Client, Context->Config.Login.AutoDisconnectDelay);
    SendURLList(Server, Context, Socket, Connection, Client);

    Response = PacketBufferInit(Connection->PacketBuffer, S2C, AUTHENTICATE);
    Response->KeepAlive = 1;
    Response->LoginStatus = Client->LoginStatus;
    Response->AccountStatus = Client->AccountStatus;
    Response->SubMessageType = 17;

    S2C_DATA_AUTHENTICATE_EXTENSION* Extension = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_AUTHENTICATE_EXTENSION);

    GenerateRandomKey(Extension->AuthKey, sizeof(Extension->AuthKey));

    AuthDBUpsertSession(
        Context->Database,
        Client->AccountID,
        false,
        Extension->AuthKey,
        Connection->AddressIP
    );

    StatementRef Statement = AuthDBSelectCharacterByID(Context->Database, Client->AccountID);
    if (Statement) {
        Int32 Index = 0;
        AUTHDB_DATA_CHARACTER Character = { 0 };
        while (Index < MAX_SERVER_COUNT && AuthDBSelectCharacterFetchNext(Context->Database, Statement, &Character)) {
            Extension->Servers[Index].ServerID = Character.ServerID;
            Extension->Servers[Index].CharacterCount = Character.CharacterCount;
        }
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
