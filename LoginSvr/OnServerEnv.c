#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"

CLIENT_PROCEDURE_BINDING(SERVER_ENVIRONMENT) {
	if (!(Client->Flags & CLIENT_FLAGS_VERSION_CHECKED)) {
		SocketDisconnect(Socket, Connection);
		return;
	}

	// TODO: Add last login based captcha
	/*
	Int32 UsernameLength = strlen(Packet->Username);
	if (UsernameLength < 1 || UsernameLength > MAX_USERNAME_LENGTH) {
		return SocketDisconnect(Socket, Connection);
	}
	memset(Client->Username, 0, sizeof(Client->Username));
	memcpy(Client->Username, Packet->Username, UsernameLength);
	IPC_DATA_REQAUTHENTICATE* Request = PacketBufferInitExtended(IPC_DATA_REQAUTHENTICATE);
	Request->ConnectionID = Connection->ID;
	memcpy(Request->Username, Packet->Username, UsernameLength);
	SocketSendAll(Server->DataSocket, Request);
}

DATA_PROCEDURE_BINDING(OnDataAccountInfo, IPC_DATA_ACCOUNTINFO, IPC_DATA_ACKACCOUNTINFO) {
	*/
	S2C_DATA_SERVER_ENVIRONMENT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, SERVER_ENVIRONMENT);
	if (Context->Config.Login.CaptchaVerificationEnabled) {
		Int32 Seed = (Int32)PlatformGetTickCount();
		Client->Captcha = (CaptchaInfoRef)ArrayGetElementAtIndex(
			Context->CaptchaInfoList,
			Random(&Seed) % ArrayGetElementCount(Context->CaptchaInfoList)
		);

		Response->Active = 1;
		Response->Timeout = Context->Config.Login.AutoDisconnectDelay;
		Response->CaptchaSize = Client->Captcha->DataLength;
		Client->Flags |= CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
		Client->DisconnectTimestamp = GetTimestampMs() + Response->Timeout;
		memcpy(Response->Captcha, Client->Captcha->Data, Response->CaptchaSize);
	}
	else {
		Client->Flags |= CLIENT_FLAGS_CAPTCHA_VERIFIED;
	}

	// TODO: Add support for image authentication
	Client->Flags |= CLIENT_FLAGS_USERNAME_CHECKED;
	SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(REFRESH_CAPTCHA) {
	if (!(Client->Flags & CLIENT_FLAGS_VERSION_CHECKED)) {
		SocketDisconnect(Socket, Connection);
		return;
	}

	S2C_DATA_REFRESH_CAPTCHA* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REFRESH_CAPTCHA);
	if (Context->Config.Login.CaptchaVerificationEnabled) {
		Int32 Seed = (Int32)PlatformGetTickCount();
		Client->Captcha = (CaptchaInfoRef)ArrayGetElementAtIndex(
			Context->CaptchaInfoList,
			Random(&Seed) % ArrayGetElementCount(Context->CaptchaInfoList)
		);

		Response->Active = 1;
		Response->Timeout = Context->Config.Login.AutoDisconnectDelay;
		Response->CaptchaSize = Client->Captcha->DataLength;
		Client->Flags |= CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
		Client->DisconnectTimestamp = GetTimestampMs() + Response->Timeout;
		memcpy(Response->Captcha, Client->Captcha->Data, Response->CaptchaSize);
	}
	else {
		Client->Flags |= CLIENT_FLAGS_CAPTCHA_VERIFIED;
	}

	SocketSend(Socket, Connection, Response);
}
