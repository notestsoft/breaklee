#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(FORCE_DISCONNECT) {
    S2C_DATA_FORCE_DISCONNECT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, FORCE_DISCONNECT);
    Response->Result = 0;

    if (Client->AccountID == Packet->AccountID && Context->Config.Login.GroupIndex == Packet->GroupIndex &&
		Client->AccountStatus == ACCOUNT_STATUS_ALREADY_LOGGED_IN) {
		Response->Result = 1;

		IPC_L2M_DATA_FORCE_DISCONNECT* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, L2M, FORCE_DISCONNECT);
		Request->Header.Source = Server->IPCSocket->NodeID;
		Request->Header.Target.Group = Server->IPCSocket->NodeID.Group;
		Request->Header.Target.Type = IPC_TYPE_MASTER;
		Request->SourceConnectionID = Connection->ID;
		Request->AccountID = Client->AccountID;
		IPCSocketUnicast(Server->IPCSocket, Request);
    }

    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(DISCONNECT) {
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(M2N, FORCE_DISCONNECT) {
	SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
	while (Iterator) {
		SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
		ClientContextRef Client = (ClientContextRef)Connection->Userdata;
		if (Connection->ID != Packet->SourceConnectionID && Client->AccountID == Packet->AccountID) {
			SocketDisconnect(Context->ClientSocket, Connection);
		}

		Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);
	}
}
