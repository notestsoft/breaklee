#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, UPDATE_SUBPASSWORD) {
    Bool Success = false;
    Success &= DatabaseCallProcedure(
        Context->Database,
        "UpdateAccountCharacterPassword",
        SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
        SQL_PARAM_INPUT, SQL_VARCHAR, Packet->CharacterPassword, sizeof(Packet->CharacterPassword),
        SQL_PARAM_INPUT, SQL_BIGINT, &Packet->CharacterQuestion,
        SQL_PARAM_INPUT, SQL_VARCHAR, Packet->CharacterAnswer, sizeof(Packet->CharacterAnswer),
        SQL_PARAM_OUTPUT, SQL_TINYINT, &Success,
        SQL_END
    );
}

IPC_PROCEDURE_BINDING(W2D, VERIFY_SUBPASSWORD) {
	IPC_D2W_DATA_VERIFY_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, VERIFY_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Type = Packet->Type;
	Response->Success &= DatabaseCallProcedure(
		Context->Database,
		"VerifySubpassword",
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->Type,
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->ExpirationInHours,
		SQL_PARAM_INPUT, SQL_VARCHAR, Packet->Password, sizeof(Packet->Password),
		SQL_PARAM_INPUT, SQL_VARCHAR, Packet->SessionIP, sizeof(Packet->SessionIP),
		SQL_PARAM_OUTPUT, SQL_INTEGER, &Response->Success,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->FailureCount,
		SQL_END
	);

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, CREATE_SUBPASSWORD) {
	IPC_D2W_DATA_CREATE_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, CREATE_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Success &= DatabaseCallProcedure(
		Context->Database,
		"InsertSubpassword",
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->Type,
		SQL_PARAM_INPUT, SQL_VARCHAR, Packet->Password, sizeof(Packet->Password),
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->Question,
		SQL_PARAM_INPUT, SQL_VARCHAR, Packet->Answer, sizeof(Packet->Answer),
		SQL_PARAM_OUTPUT, SQL_INTEGER, &Response->Success,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->Type,
		SQL_END
	);

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, DELETE_SUBPASSWORD) {
	IPC_D2W_DATA_DELETE_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, DELETE_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Type = Packet->Type;
	Response->Success &= DatabaseCallProcedure(
		Context->Database,
		"DeleteSubpassword",
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->Type,
		SQL_PARAM_OUTPUT, SQL_INTEGER, &Response->Success,
		SQL_END
	);

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, VERIFY_DELETE_SUBPASSWORD) {
	IPC_D2W_DATA_VERIFY_DELETE_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, VERIFY_DELETE_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Type = Packet->Type;
	Response->Success &= DatabaseCallProcedure(
		Context->Database,
		"VerifyDeleteSubpassword",
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->Type,
		SQL_PARAM_INPUT, SQL_VARCHAR, Packet->Password, sizeof(Packet->Password),
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->Success,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->FailureCount,
		SQL_END
	);

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, VERIFY_CREDENTIALS_SUBPASSWORD) {
	IPC_D2W_DATA_VERIFY_CREDENTIALS_SUBPASSWORD* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, VERIFY_CREDENTIALS_SUBPASSWORD);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Success &= DatabaseCallProcedure(
		Context->Database,
		"VerifyCredentialsSubpassword",
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_INPUT, SQL_VARCHAR, Packet->Password, sizeof(Packet->Password),
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->Success,
		SQL_END
	);

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
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->IsVerificationRequired,
		SQL_END
	);

	IPCSocketUnicast(Socket, Response);
}
