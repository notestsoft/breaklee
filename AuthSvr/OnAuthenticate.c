#include "AuthDB.h"
#include "Constants.h"
#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "Notification.h"
#include "Server.h"

Void StartAuthTimer(
    ServerRef Server,
    ServerContextRef Context,
    SocketRef Socket,
    SocketConnectionRef Connection,
    ClientContextRef Client,
    UInt32 Timeout
) {
    S2C_DATA_AUTH_TIMER* Response = PacketInit(S2C_DATA_AUTH_TIMER);
    Response->Command = S2C_AUTH_TIMER;
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
    S2C_DATA_URL_LIST* Response = PacketInit(S2C_DATA_URL_LIST);
    Response->Command = S2C_URL_LIST;
    
    S2C_DATA_URL *Url = PacketAppendStruct(S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Itemshop);
    PacketAppendCString(Context->Config.Links.Itemshop);
    
    Url = PacketAppendStruct(S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Unknown1);
    PacketAppendCString(Context->Config.Links.Unknown1);
    
    Url = PacketAppendStruct(S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Unknown2);
    PacketAppendCString(Context->Config.Links.Unknown2);
    
    Url = PacketAppendStruct(S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Guild);
    PacketAppendCString(Context->Config.Links.Guild);
    
    Url = PacketAppendStruct(S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.SNS);
    PacketAppendCString(Context->Config.Links.SNS);
    
    Url = PacketAppendStruct(S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Unknown3);
    PacketAppendCString(Context->Config.Links.Unknown3);
    
    Url = PacketAppendStruct(S2C_DATA_URL);
    Url->Length = (UInt32)strlen(Context->Config.Links.Unknown4);
    PacketAppendCString(Context->Config.Links.Unknown4);

    Response->PayloadLength[0] = (
        Response->Signature.Length - 
        sizeof(S2C_DATA_URL_LIST) +
        sizeof(Response->PayloadLength[1])
    );
    Response->PayloadLength[1] = (
        Response->Signature.Length -
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
    S2C_DATA_SYSTEM_MESSAGE* Response = PacketInit(S2C_DATA_SYSTEM_MESSAGE);
    Response->Command = S2C_SYSTEM_MESSAGE;
    Response->Message = SYSTEM_MESSAGE_LOGIN_SUCCESS;
    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(AUTHENTICATE) {
    if (!(Client->Flags & CLIENT_FLAGS_AUTHORIZED)) {
        return SocketDisconnect(Socket, Connection);
    }

    if (Packet->SubMessageType == 25) {
        S2C_DATA_AUTHENTICATE* Response = PacketInit(S2C_DATA_AUTHENTICATE);
        Response->Command = S2C_AUTHENTICATE;
        Response->KeepAlive = 1;
        Response->SubMessageType = 25;
        Response->LoginStatus = Client->LoginStatus;
        Response->AccountStatus = Client->AccountStatus;

        S2C_DATA_AUTHENTICATE_EXTENSION_UNKNOWN* ResponseData = PacketAppendStruct(S2C_DATA_AUTHENTICATE_EXTENSION_UNKNOWN);
        return SocketSend(Socket, Connection, Response);
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

    S2C_DATA_AUTHENTICATE* Response = PacketInit(S2C_DATA_AUTHENTICATE);
    Response->Command = S2C_AUTHENTICATE;
    Response->KeepAlive = 1;
    Response->LoginStatus = LOGIN_STATUS_ERROR;
    Response->AccountStatus = ACCOUNT_STATUS_NORMAL;

    AUTHDB_DATA_ACCOUNT Account = { 0 };

    if (AuthDBSelectAccountByUsername(Context->Database, Username, &Account)) {
        if (Account.DeletedAt > 0) {
            Response->AccountStatus = ACCOUNT_STATUS_ACCOUNT_IS_DELETED;
        }
        else if (Context->Config.Auth.EmailVerificationEnabled && !Account.EmailVerified) {
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

        if (Context->Config.Auth.AutoCreateAccountOnLogin) {
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

    StartAuthTimer(Server, Context, Socket, Connection, Client, Context->Config.Auth.AutoDisconnectDelay);
    SendURLList(Server, Context, Socket, Connection, Client);

    Response = PacketInit(S2C_DATA_AUTHENTICATE);
    Response->Command = S2C_AUTHENTICATE;
    Response->KeepAlive = 1;
    Response->LoginStatus = Client->LoginStatus;
    Response->AccountStatus = Client->AccountStatus;
    Response->SubMessageType = 17;

    S2C_DATA_AUTHENTICATE_EXTENSION* Extension = PacketAppendStruct(S2C_DATA_AUTHENTICATE_EXTENSION);

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

    StartAuthTimer(Server, Context, Socket, Connection, Client, Context->Config.Auth.AutoDisconnectDelay);
    SendMessageLoginSuccess(Server, Context, Socket, Connection, Client);
    BroadcastWorldListToConnection(Server, Context, Connection, Client);
    
    // Just clearing the payload buffer to avoid keeping sensitive data in memory!
    memset(Client->RSAPayloadBuffer, 0, sizeof(Client->RSAPayloadBuffer));
    return;

error:
    // Just clearing the payload buffer to avoid keeping sensitive data in memory!
    memset(Client->RSAPayloadBuffer, 0, sizeof(Client->RSAPayloadBuffer));

    SocketDisconnect(Socket, Connection);
}
