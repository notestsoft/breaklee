#include "ClientProtocol.h"
#include "ClientProcedures.h"

SERVER_PROCEDURE_BINDING(CONNECT_LOGIN) {
	KeychainSeed(&Connection->Keychain, Packet->XorKey, Packet->XorKeyIndex);

	C2S_DATA_CHECK_VERSION_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, CHECK_VERSION_LOGIN);
	Request->ClientVersion = 13162;
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(VERIFY_LINKS_LOGIN) {}

SERVER_PROCEDURE_BINDING(AUTH_ACCOUNT_LOGIN) {
	ClientContextRef Client = (ClientContextRef)Connection->Userdata;
	if (!Client->RSA) return;

	C2S_DATA_AUTHENTICATE_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, AUTHENTICATE_LOGIN);
	Request->SubMessageType = 0;
	
	Char Payload[256] = { 0 };
	CString Username = (CString)&Payload[0];
	CString Password = (CString)&Payload[129];
	CStringCopySafe(Username, 128, "test");
	CStringCopySafe(Password, 128, "test");
	
	CString EncryptedData = malloc(RSA_size(Client->RSA));
	Int32 EncryptedDataLength = RSA_public_encrypt(256, Payload, EncryptedData, Client->RSA, RSA_PKCS1_OAEP_PADDING);
	PacketBufferAppendCopy(Connection->PacketBuffer, EncryptedData, EncryptedDataLength);
	free(EncryptedData);

	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(SYSTEM_MESSAGE_LOGIN) {}

SERVER_PROCEDURE_BINDING(SERVER_LIST_LOGIN) {
	ClientContextRef Client = (ClientContextRef)Connection->Userdata;

	if (Packet->ServerCount < 1) return;
	
	S2C_DATA_LOGIN_SERVER_LIST_INDEX* Server = (S2C_DATA_LOGIN_SERVER_LIST_INDEX*)((UInt8*)Packet) + sizeof(S2C_DATA_SERVER_LIST_LOGIN);
	if (Server->WorldCount < 1) return;

	S2C_DATA_LOGIN_SERVER_LIST_WORLD* World = (S2C_DATA_LOGIN_SERVER_LIST_WORLD*)((UInt8*)Server) + sizeof(S2C_DATA_LOGIN_SERVER_LIST_INDEX);
	CStringCopySafe(Client->WorldHost, MAX_PATH, World->WorldHost);
	Client->WorldPort = World->WorldPort;

	C2S_DATA_CONNECT_WORLD* Request = PacketBufferInit(Connection->PacketBuffer, C2S, CONNECT_WORLD);
	Request->ServerID = World->ServerID;
	Request->WorldServerID = World->WorldID;
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(CHECK_VERSION_LOGIN) {
	C2S_DATA_SERVER_ENVIRONMENT_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, SERVER_ENVIRONMENT_LOGIN);
	CStringCopySafe(Request->Username, 129, Context->Username);
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(UNKNOWN_124_LOGIN) {}

SERVER_PROCEDURE_BINDING(URL_LIST_LOGIN) {}

SERVER_PROCEDURE_BINDING(PUBLIC_KEY_LOGIN) {
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
	C2S_DATA_PUBLIC_KEY_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, PUBLIC_KEY_LOGIN);
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(DISCONNECT_TIMER_LOGIN) {}

SERVER_PROCEDURE_BINDING(AUTHENTICATE_LOGIN) {}

SERVER_PROCEDURE_BINDING(AUTH_TIMER_LOGIN) {}

SERVER_PROCEDURE_BINDING(UNKNOWN_3383_LOGIN) {}

SERVER_PROCEDURE_BINDING(UNKNOWN_5383_LOGIN) {}

SERVER_PROCEDURE_BINDING(VERIFY_CAPTCHA_LOGIN) {}
