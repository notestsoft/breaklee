#include "AuthDB.h"
#include "IPCProtocol.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(OnMasterVerifyPassword, IPC_AUTH_REQVERIFYPASSWORD, IPC_DATA_AUTH_REQVERIFYPASSWORD) {
    IPC_DATA_AUTH_ACKVERIFYPASSWORD* Response = PacketBufferInitExtended(Context->MasterSocket->PacketBuffer, IPC, AUTH_ACKVERIFYPASSWORD);
    Response->ConnectionID = Packet->ConnectionID;
    Response->Success = false;

    CString Password = (CString)Packet->Credentials;
    Int32 PasswordLength = (Int32)strlen(Password);
    AUTHDB_DATA_ACCOUNT Account = { 0 };
    if (PasswordLength <= MAX_PASSWORD_LENGTH && AuthDBSelectAccountByID(Context->Database, Packet->AccountID, &Account)) {
        if (Account.DeletedAt <= 0) {
            Bool ValidCredentials = ValidatePasswordHash(
                Password,
                Account.Salt,
                MAX_PASSWORD_SALT_LENGTH,
                Account.Hash,
                MAX_PASSWORD_HASH_LENGTH
            );

            AUTHDB_DATA_BLACKLIST Blacklist = { 0 };
            if (ValidCredentials && !AuthDBSelectBlacklistByAccount(Context->Database, Account.ID, &Blacklist)) {
                Response->Success = true;
            }
        }
    }

	SocketSendAll(Context->MasterSocket, Response);
}
