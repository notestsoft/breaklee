#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GET_CHARACTER) {
    IPC_D2W_DATA_GET_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GET_CHARACTER);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

    MASTERDB_DATA_ACCOUNT Account = { 0 };
    Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, Packet->AccountID, &Account)) goto error;

    MASTERDB_DATA_CHARACTER Character = { 0 };
    Character.CharacterID = Packet->CharacterID;
	if (!MasterDBSelectCharacterByID(Context->Database, &Character)) goto error;

	if (Character.AccountID != Account.AccountID) goto error;

	Response->Success = true;
    Response->CharacterIndex = Packet->CharacterIndex;
    Response->CharacterID = Character.CharacterID;
    Response->CharacterCreationDate = Character.CreatedAt;
    CStringCopySafe(Response->CharacterName, MAX_CHARACTER_NAME_LENGTH + 1, Character.Name);

#define CHARACTER_DATA_PROTOCOL(__TYPE__, __NAME__, __SCOPE__) \
    { \
        DataTableRef Table = DatabaseGetDataTable(Context->Database, #__SCOPE__, #__NAME__); \
        if (!Table) goto error; \
        if (!DataTableSelect(Table, Packet->__SCOPE__##ID, (UInt8*)&Response->CharacterData.__NAME__, sizeof(__TYPE__))); \
    }
#include "RuntimeLib/CharacterDataDefinition.h"

    IPCSocketUnicast(Socket, Response);
    return;

error:
	Response->Success = false;
    IPCSocketUnicast(Socket, Response);
}
