#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

CLIENT_PROCEDURE_BINDING(CONNECT) {
	S2C_DATA_CONNECT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CONNECT);
    Response->XorKey = (UInt32)rand();
    Response->AuthKey = (UInt32)rand();
    Response->ConnectionID = (UInt16)Connection->ID;
    Response->XorKeyIndex = (UInt16)rand() % KEYCHAIN_XORKEY_COUNT;
    Response->Unknown2 = 1;

    Client->AuthKey = Response->AuthKey;
    Client->Flags |= CLIENT_FLAGS_CONNECTED;
    
    SocketConnectionKeychainSeed(Socket, Connection, Response->XorKey, Response->XorKeyIndex);
	SocketSend(Socket, Connection, Response);
}

IPC_PROCEDURE_BINDING(W2C, CLIENT_CONNECT) {

}

IPC_PROCEDURE_BINDING(W2C, CLIENT_DISCONNECT) {

}