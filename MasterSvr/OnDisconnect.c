#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(L2M, FORCE_DISCONNECT) {
	IPC_M2N_DATA_FORCE_DISCONNECT* Notification = IPCPacketBufferInit(Connection->PacketBuffer, M2N, FORCE_DISCONNECT);
	Notification->Header.Source = Server->IPCSocket->NodeID;
	Notification->Header.Target.Index = 0;
	Notification->Header.Target.Group = Server->IPCSocket->NodeID.Group;
	Notification->Header.Target.Type = IPC_TYPE_ALL;
	Notification->SourceConnectionID = Packet->SourceConnectionID;
	Notification->AccountID = Packet->AccountID;
	IPCSocketBroadcast(Socket, Notification);
}
