#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, DELETE_CHARACTER) {
	IPC_D2W_DATA_DELETE_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, DELETE_CHARACTER);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->CharacterID = Packet->CharacterID;
	Response->Success = MasterDBDeleteCharacter(Context->Database, Packet->AccountID, Packet->CharacterID);
	IPCSocketUnicast(Socket, Response);
}