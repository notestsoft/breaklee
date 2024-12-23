#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(M2N, FORCE_DISCONNECT) {
	/* TODO: Enable when client will connect to PartySvr in the future!
	SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
	while (Iterator) {
		SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
		ClientContextRef Client = (ClientContextRef)Connection->Userdata;
		if (Connection->ID != Packet->SourceConnectionID && Client->AccountID == Packet->AccountID) {
			SocketDisconnect(Context->ClientSocket, Connection);
		}

		Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);
	}
	*/
}
