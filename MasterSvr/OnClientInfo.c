#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(N2M, CLIENT_CONNECT) {
	Int ClientIndex = Packet->AccountID;
	ClientInfoRef ClientInfo = (ClientInfoRef)DictionaryLookup(Context->ClientInfoTable, &ClientIndex);
	if (!ClientInfo) {
		struct _ClientInfo Info = { 0 };
		Info.AccountID = Packet->AccountID;
		DictionaryInsert(Context->ClientInfoTable, &ClientIndex, &Info, sizeof(struct _ClientInfo));
		ClientInfo = (ClientInfoRef)DictionaryLookup(Context->ClientInfoTable, &ClientIndex);
		assert(ClientInfo);
	}

	assert(ConnectionContext->NodeID.Type < IPC_TYPE_COUNT);
	ClientInfo->NodeConnectionCount[ConnectionContext->NodeID.Type] += 1;

	IPC_M2N_DATA_CLIENT_CONNECT* Notification = IPCPacketBufferInit(Connection->PacketBuffer, M2N, CLIENT_CONNECT);
	Notification->Header.Source = Server->IPCSocket->NodeID;
	Notification->Header.Target.Index = 0;
	Notification->Header.Target.Group = Server->IPCSocket->NodeID.Group;
	Notification->Header.Target.Type = IPC_TYPE_ALL;
	Notification->AccountID = Packet->AccountID;
	Notification->IsOnline = true;
	IPCSocketBroadcast(Socket, Notification);
}

IPC_PROCEDURE_BINDING(N2M, CLIENT_DISCONNECT) {
	Bool IsOnline = false;
	Int ClientIndex = Packet->AccountID;
	ClientInfoRef ClientInfo = (ClientInfoRef)DictionaryLookup(Context->ClientInfoTable, &ClientIndex);
	if (ClientInfo) {
		assert(ConnectionContext->NodeID.Type < IPC_TYPE_COUNT);
		ClientInfo->NodeConnectionCount[ConnectionContext->NodeID.Type] -= 1;

		for (Int Index = 0; Index < IPC_TYPE_COUNT; Index += 1) {
			if (ClientInfo->NodeConnectionCount[Index] > 0) {
				IsOnline = true;
				break;
			}
		}

		if (!IsOnline) {
			DictionaryRemove(Context->ClientInfoTable, &ClientIndex);
		}
	}

	IPC_M2N_DATA_CLIENT_DISCONNECT* Notification = IPCPacketBufferInit(Connection->PacketBuffer, M2N, CLIENT_DISCONNECT);
	Notification->Header.Source = Server->IPCSocket->NodeID;
	Notification->Header.Target.Index = 0;
	Notification->Header.Target.Group = Server->IPCSocket->NodeID.Group;
	Notification->Header.Target.Type = IPC_TYPE_ALL;
	Notification->AccountID = Packet->AccountID;
	Notification->IsOnline = IsOnline;
	IPCSocketBroadcast(Socket, Notification);
}
