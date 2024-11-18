#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"

CLIENT_PROCEDURE_BINDING(VERIFY_CAPTCHA) {
	if (!(Client->Flags & CLIENT_FLAGS_VERSION_CHECKED)) {
		SocketDisconnect(Socket, Connection);
		return;
	}

	S2C_DATA_VERIFY_CAPTCHA* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, VERIFY_CAPTCHA);
	if (Context->Config.Login.CaptchaVerificationEnabled) {
		if (Client->Captcha) {
			Response->Success = CStringIsEqual(Client->Captcha->Name, Packet->Captcha);
		}
	}
	else {
		Response->Success = true;
	}

	if (Response->Success) {
		Client->Flags |= CLIENT_FLAGS_CAPTCHA_VERIFIED;
	}

	SocketSend(Socket, Connection, Response);
}
