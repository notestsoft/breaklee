#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// TODO: Sync vehicle inventory to database!

Void ServerSyncCharacter(
	ServerRef Server,
	ServerContextRef Context,
	ClientContextRef Client,
	RTCharacterRef Character
) {
	Character->SyncTimestamp = GetTimestampMs();

	IPC_W2D_DATA_DBSYNC* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, DBSYNC);
	Request->Header.SourceConnectionID = Client->Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->Account.AccountID;
	Request->CharacterID = Client->CharacterDatabaseID;
	Request->SyncMask = Character->SyncMask;

#define CHARACTER_DATA_PROTOCOL(__TYPE__, __NAME__, __SCOPE__) \
    if (Character->SyncMask.__NAME__) { \
        IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.__NAME__, sizeof(__TYPE__)); \
    }
#include "RuntimeLib/CharacterDataDefinition.h"

	Character->SyncMask.RawValue = 0;

	IPCSocketUnicast(Server->IPCSocket, Request);
}

Void ServerSyncDB(
	ServerRef Server,
	ServerContextRef Context,
	Bool Force
) {
	Timestamp Timestamp = GetTimestampMs();

	SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
	while (Iterator) {
		SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
		// TODO: Connection can become null!!!

		Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

		ClientContextRef Client = (ClientContextRef)Connection->Userdata;
		if (Client->CharacterIndex < 1) continue;

		RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, Client->CharacterIndex);
		assert(Character);

		Bool PerformSync = Force || (
			Character->SyncMask.RawValue && 
            (Timestamp - Character->SyncTimestamp) >= Context->Config.WorldSvr.DBSyncTimer
		);

		if (PerformSync) {
			ServerSyncCharacter(Server, Context, Client, Character);
		}
	}
}

IPC_PROCEDURE_BINDING(D2W, DBSYNC) {
	if (!Client || !Character) return;

	if (Packet->SyncMaskFailed.RawValue) {
		Character->SyncMask.RawValue |= Packet->SyncMaskFailed.RawValue;
	}
}
