#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SERVER_ENVIRONMENT) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->Account.AccountID < 1) goto error;

	S2C_DATA_GET_SERVER_ENVIRONMENT* Response = PacketInit(S2C_DATA_GET_SERVER_ENVIRONMENT);
	Response->Command = S2C_GET_SERVER_ENVIRONMENT;
	Response->MaxLevel = Context->Config.Environment.MaxLevel;
	Response->DummyEnabled = Context->Config.Environment.DummyEnabled;
	Response->CashshopEnabled = Context->Config.Environment.CashshopEnabled;
	Response->NetCafePointEnabled = Context->Config.Environment.NetCafePointEnabled;
	Response->MinChatLevel = Context->Config.Environment.MinChatLevel;
	Response->MinShoutLevel = Context->Config.Environment.MinShoutLevel;
	Response->MinShoutSkillRank = Context->Config.Environment.MinShoutSkillRank;
	Response->MaxInventoryCurrency = Context->Config.Environment.MaxInventoryCurrency;
	Response->MaxWarehouseCurrency = Context->Config.Environment.MaxWarehouseCurrency;
	Response->MaxAuctionCurrency = Context->Config.Environment.MaxAuctionCurrency;
    Response->NetCafePremiumEnabled = 1;
    Response->GuildBoardEnabled = 0;
    Response->NetCafePremiumType = 0;
    Response->AgentShopEnabled = 1;
    Response->MegaphoneShoutCooldownTime = 0;
    Response->MinAgentShopLevel = 10;
    Response->MinPersonalShopLevel = 10;
    Response->TPointEnabled = 1;
    Response->GuildExpansionEnabled = 1;
    Response->LimitMegaphoneShoutEnabled = 1;
    Response->MinTalkLevel = 10;
    Response->MinTradeTalkLevel = 10;
    Response->MaxDP = 999;
    Response->Unknown4 = 1000000000;
    Response->Unknown5 = 7;
    Response->Unknown6 = 1;
    Response->Unknown7 = 1;
	Response->MaxHonorPoint = Context->Config.Environment.MaxHonorPoint;
	Response->MinHonorPoint = Context->Config.Environment.MinHonorPoint;
    Response->Unknown8 = 0;
    Response->Unknown9 = 3;
    Response->Unknown10 = 1;
    Response->Unknown11 = 16;
    Response->Unknown12 = 15;
    Response->Unknown13 = 2;
    Response->Unknown14 = 109;
    Response->Unknown15 = 110;
    Response->Unknown16 = 3;
    Response->Unknown17 = 158;
    Response->Unknown18 = 159;
    Response->Unknown20 = 5;
    Response->Unknown21 = 0;
    Response->Unknown22 = 0;
    Response->Unknown23 = 0;
    Response->Unknown24 = 1;

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
