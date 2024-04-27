#include "ClientProtocol.h"
#include "ClientProcedures.h"

SERVER_PROCEDURE_BINDING(CONNECT_LOGIN) {
	KeychainSeed(&Connection->Keychain, Packet->XorKey, Packet->XorKeyIndex);

	C2S_DATA_CHECK_VERSION_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, CHECK_VERSION_LOGIN);
	Request->ClientVersion = 13155;
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(VERIFY_LINKS_LOGIN) {}

SERVER_PROCEDURE_BINDING(AUTH_ACCOUNT_LOGIN) {}

SERVER_PROCEDURE_BINDING(SYSTEM_MESSAGE_LOGIN) {}

SERVER_PROCEDURE_BINDING(SERVER_LIST_LOGIN) {}

SERVER_PROCEDURE_BINDING(CHECK_VERSION_LOGIN) {
	C2S_DATA_SERVER_ENVIRONMENT_LOGIN* Request = PacketBufferInit(Connection->PacketBuffer, C2S, SERVER_ENVIRONMENT_LOGIN);
	CStringCopySafe(&Request->Username, 129, Context->Username);
	SocketSend(Socket, Connection, Request);
}

SERVER_PROCEDURE_BINDING(UNKNOWN_124_LOGIN) {}

SERVER_PROCEDURE_BINDING(URL_LIST_LOGIN) {}

SERVER_PROCEDURE_BINDING(PUBLIC_KEY_LOGIN) {
	ClientContextRef Client = (ClientContextRef)Connection->Userdata;
    if (Client->RSA) RSA_free(Client->RSA);
    
    Client->RSA = RSA_new();
    BIGNUM* Exponent = BN_new();
    BIO* BIO = BIO_new(BIO_s_mem());

    if (!Client->RSA || !Exponent || !BIO) goto error;

    BN_set_word(Exponent, RSA_F4);

    if (RSA_generate_key_ex(Client->RSA, 2048, Exponent, NULL) != 1) goto error;

    i2d_RSAPublicKey_bio(BIO, Client->RSA, );

    UInt8* Key = NULL;
    Int64 Length = BIO_get_mem_data(BIO, &Key);
    BIO_set_mem_buf(BIO, Packet->Payload, Packet->PublicKeyLength);
    if (!Key) goto error;

    BN_free(Exponent);

    return true;

error:
    if (Client->RSA) RSA_free(Client->RSA);
    if (Exponent) BN_free(Exponent);
    if (BIO) BIO_free(BIO);

    Client->RSA = NULL;

    return false;
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
