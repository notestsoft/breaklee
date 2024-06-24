#include "ClientProtocol.h"
#include "ClientProcedures.h"

SERVER_PROCEDURE_BINDING(CONNECT_LOGIN) {
	Trace("CONNECT_LOGIN");
	KeychainSeed(&Connection->Keychain, Packet->XorKey, Packet->XorKeyIndex);

	C2S_DATA_CHECK_VERSION_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, CHECK_VERSION_LOGIN);
	Request->ClientVersion = 13162;
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(VERIFY_LINKS_LOGIN) {
	Trace("VERIFY_LINKS_LOGIN");
}

SERVER_PROCEDURE_BINDING(AUTH_ACCOUNT_LOGIN) {
	Trace("AUTH_ACCOUNT_LOGIN");
	ClientContextRef Client = (ClientContextRef)Connection->Userdata;
	if (!Client->RSA) return;

	C2S_DATA_AUTHENTICATE_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, AUTHENTICATE_LOGIN);
	Request->SubMessageType = 25;
	
	Char Payload[CLIENT_RSA_PAYLOAD_LENGTH] = { 0 };
	CString Username = (CString)&Payload[0];
	CString Password = (CString)&Payload[129];
	CStringCopySafe(Username, 128, "test");
	CStringCopySafe(Password, CLIENT_RSA_PAYLOAD_LENGTH - 129, "test");
	
	CString EncryptedData = malloc(RSA_size(Client->RSA));
	Int32 EncryptedDataLength = RSA_public_encrypt(CLIENT_RSA_PAYLOAD_LENGTH, Payload, EncryptedData, Client->RSA, RSA_PKCS1_OAEP_PADDING);
	if (EncryptedDataLength > 0) {
		PacketBufferAppendCopy(Connection->PacketBuffer, EncryptedData, EncryptedDataLength);
		SocketSend(Socket, Connection, Request);
	}

	free(EncryptedData);
}

SERVER_PROCEDURE_BINDING(SYSTEM_MESSAGE_LOGIN) {
	Trace("SYSTEM_MESSAGE_LOGIN");
}

SERVER_PROCEDURE_BINDING(SERVER_LIST_LOGIN) {
	Trace("SERVER_LIST_LOGIN");
	ClientContextRef Client = (ClientContextRef)Connection->Userdata;

	if (Packet->ServerCount < 1) return;
	
	S2C_DATA_LOGIN_SERVER_LIST_INDEX* ServerList = (S2C_DATA_LOGIN_SERVER_LIST_INDEX*)(((UInt8*)Packet) + sizeof(S2C_DATA_SERVER_LIST_LOGIN));
	if (ServerList->WorldCount < 1) return;

	S2C_DATA_LOGIN_SERVER_LIST_WORLD* World = (S2C_DATA_LOGIN_SERVER_LIST_WORLD*)(((UInt8*)Server) + sizeof(S2C_DATA_LOGIN_SERVER_LIST_INDEX));
	CStringCopySafe(Client->WorldHost, MAX_PATH, World->WorldHost);
	Client->WorldPort = World->WorldPort;

	C2S_DATA_CONNECT_WORLD* Request = PacketBufferInit(Connection->PacketBuffer, C2S, CONNECT_WORLD);
	Request->ServerID = World->ServerID;
	Request->WorldServerID = World->WorldID;
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(CHECK_VERSION_LOGIN) {
	Trace("CHECK_VERSION_LOGIN");
	C2S_DATA_SERVER_ENVIRONMENT_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, SERVER_ENVIRONMENT_LOGIN);
	CStringCopySafe(Request->Username, 129, Context->Username);
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(UNKNOWN_124_LOGIN) {
	Trace("UNKNOWN_124_LOGIN");
}

SERVER_PROCEDURE_BINDING(URL_LIST_LOGIN) {
	Trace("URL_LIST_LOGIN");
}

SERVER_PROCEDURE_BINDING(PUBLIC_KEY_LOGIN) {
	Trace("PUBLIC_KEY_LOGIN");
	ClientContextRef Client = (ClientContextRef)Connection->Userdata;
    if (Client->RSA) RSA_free(Client->RSA);
    
    BIO* BIO = BIO_new_mem_buf(Packet->Payload, Packet->PublicKeyLength);
    if (!BIO) goto error;

    Client->RSA = d2i_RSAPublicKey_bio(BIO, NULL);
    if (!Client->RSA) goto error;

	BIO_free(BIO);

	C2S_DATA_AUTH_ACCOUNT_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, AUTH_ACCOUNT_LOGIN);
	SocketSend(Socket, Connection, Request);
    return;

error:
    if (Client->RSA) RSA_free(Client->RSA);
    if (BIO) BIO_free(BIO);

    Client->RSA = NULL;
}

SERVER_PROCEDURE_BINDING(SERVER_ENVIRONMENT_LOGIN) {
	Trace("SERVER_ENVIRONMENT_LOGIN");
	C2S_DATA_PUBLIC_KEY_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, PUBLIC_KEY_LOGIN);
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(DISCONNECT_TIMER_LOGIN) {}

SERVER_PROCEDURE_BINDING(AUTHENTICATE_LOGIN) {
	Trace("AUTHENTICATE_LOGIN");
}

SERVER_PROCEDURE_BINDING(AUTH_TIMER_LOGIN) {
	Trace("AUTH_TIMER_LOGIN");
}

SERVER_PROCEDURE_BINDING(UNKNOWN_3383_LOGIN) {
	Trace("UNKNOWN_3383_LOGIN");
}

SERVER_PROCEDURE_BINDING(UNKNOWN_5383_LOGIN) {
	Trace("UNKNOWN_5383_LOGIN");
}

SERVER_PROCEDURE_BINDING(VERIFY_CAPTCHA_LOGIN) {
	Trace("VERIFY_CAPTCHA_LOGIN");
}

SERVER_PROCEDURE_BINDING(REFRESH_CAPTCHA_LOGIN) {
	Trace("REFRESH_CAPTCHA_LOGIN");
}
