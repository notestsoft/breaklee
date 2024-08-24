#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, VERIFY_SUBPASSWORD) {
	IPC_D2W_DATA_VERIFY_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, VERIFY_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	
	if (!DatabaseCallProcedure(
		Context->Database,
		"VerifySubpassword",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT32(Packet->Type),
		DB_INPUT_INT32(Packet->ExpirationInHours),
		DB_INPUT_STRING(Packet->Password, sizeof(Packet->Password)),
		DB_OUTPUT_INT32(Response->Success),
		DB_OUTPUT_INT8(Response->FailureCount),
		DB_OUTPUT_INT32(Response->IsLocked),
		DB_OUTPUT_INT32(Response->Type),
		DB_PARAM_END
	)) {
		Response->Success = false;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, CREATE_SUBPASSWORD) {
	IPC_D2W_DATA_CREATE_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, CREATE_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	
	if (!DatabaseCallProcedure(
		Context->Database,
		"InsertSubpassword",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT32(Packet->Type),
		DB_INPUT_STRING(Packet->Password, sizeof(Packet->Password)),
		DB_INPUT_INT32(Packet->Question),
		DB_INPUT_STRING(Packet->Answer, sizeof(Packet->Answer)),
		DB_INPUT_INT32(Packet->IsChange),
		DB_OUTPUT_INT32(Response->Success),
		DB_OUTPUT_INT32(Response->IsChange),
		DB_OUTPUT_INT32(Response->Type),
		DB_OUTPUT_INT32(Response->IsLocked),
		DB_PARAM_END
	)) {
		Response->Success = false;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, DELETE_SUBPASSWORD) {
	IPC_D2W_DATA_DELETE_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, DELETE_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	
	if (!DatabaseCallProcedure(
		Context->Database,
		"DeleteSubpassword",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT32(Packet->Type),
		DB_OUTPUT_INT32(Response->Success),
		DB_OUTPUT_INT32(Response->Type),
		DB_PARAM_END
	)) {
		Response->Success = false;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, VERIFY_DELETE_SUBPASSWORD) {
	IPC_D2W_DATA_VERIFY_DELETE_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, VERIFY_DELETE_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	
	if (!DatabaseCallProcedure(
		Context->Database,
		"VerifyDeleteSubpassword",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT32(Packet->Type),
		DB_INPUT_STRING(Packet->Password, sizeof(Packet->Password)),
		DB_OUTPUT_INT32(Response->Success),
		DB_OUTPUT_INT8(Response->FailureCount),
		DB_PARAM_END
	)) {
		Response->Success = false;
	}
	
	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, VERIFY_CREDENTIALS_SUBPASSWORD) {
	IPC_D2W_DATA_VERIFY_CREDENTIALS_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, VERIFY_CREDENTIALS_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
		
	if (!DatabaseCallProcedure(
		Context->Database,
		"VerifyCredentialsSubpassword",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_STRING(Packet->Password, sizeof(Packet->Password)),
		DB_OUTPUT_INT8(Response->Success),
		DB_PARAM_END
	)) {
		Response->Success = false;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, CHECK_SUBPASSWORD) {
	IPC_D2W_DATA_CHECK_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, CHECK_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	DatabaseCallProcedure(
		Context->Database,
		"CheckSubpassword",
		DB_INPUT_INT32(Packet->AccountID),
		DB_OUTPUT_INT32(Response->IsVerificationRequired),
		DB_PARAM_END
	);

	IPCSocketUnicast(Socket, Response);
}
