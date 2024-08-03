#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, DBSYNC) {
	IPC_D2W_DATA_DBSYNC* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, DBSYNC);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->CharacterID = Packet->CharacterID;
	Response->SyncMaskFailed.RawValue = 0;

	Int32 DataOffset = 0;

    if (Packet->IsTransaction) DatabaseBeginTransaction(Context->Database); 
    {
#define ACCOUNT_DATA_PROTOCOL(__TYPE__, __NAME__) \
        if (Packet->SyncMask.__NAME__) { \
            __TYPE__* Data = (__TYPE__*)&Packet->Data[DataOffset]; \
            DataOffset += sizeof(__TYPE__); \
            DataTableRef Table = DatabaseGetDataTable(Context->Database, TABLE_SCOPE_ACCOUNT, #__NAME__); \
            if (Table) { \
                if (!DataTableUpdate(Table, Packet->AccountID, (UInt8*)Data, sizeof(__TYPE__))) { \
                    Response->SyncMaskFailed.__NAME__ = true; \
                } \
            } else { \
                Response->SyncMaskFailed.__NAME__ = true; \
            } \
        }
        
#define CHARACTER_DATA_PROTOCOL(__TYPE__, __NAME__) \
        if (Packet->SyncMask.__NAME__) { \
            __TYPE__* Data = (__TYPE__*)&Packet->Data[DataOffset]; \
            DataOffset += sizeof(__TYPE__); \
            DataTableRef Table = DatabaseGetDataTable(Context->Database, TABLE_SCOPE_CHARACTER, #__NAME__); \
            if (Table) { \
                if (!DataTableUpdate(Table, Packet->CharacterID, (UInt8*)Data, sizeof(__TYPE__))) { \
                    Response->SyncMaskFailed.__NAME__ = true; \
                } \
            } else { \
                Response->SyncMaskFailed.__NAME__ = true; \
            } \
        }

#include "RuntimeLib/CharacterDataDefinition.h"
    }

    if (Packet->IsTransaction) {
        if (Response->SyncMaskFailed.RawValue) {
            Response->SyncMaskFailed = Packet->SyncMask;
            DatabaseRollbackTransaction(Context->Database);
        }
        else if (!DatabaseCommitTransaction(Context->Database)) {
            Response->SyncMaskFailed = Packet->SyncMask;
            DatabaseRollbackTransaction(Context->Database);
        }
    }

    IPCSocketUnicast(Socket, Response);
}
