#include "AuthDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2L, VERIFY_PASSWORD) {
    IPC_L2W_DATA_VERIFY_PASSWORD* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, L2W, VERIFY_PASSWORD);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
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

    IPCSocketUnicast(Server->IPCSocket, Response);
}
