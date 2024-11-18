#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SERVER_ENVIRONMENT) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->AccountID < 1) goto error;

    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_GET_SERVER_ENVIRONMENT* Response = PacketBufferInit(PacketBuffer, S2C, GET_SERVER_ENVIRONMENT);
	Response->MaxLevel = Context->Config.Environment.MaxLevel;
	Response->DummyEnabled = Context->Config.Environment.DummyEnabled;
	Response->CashshopEnabled = Context->Config.Environment.CashshopEnabled;
	Response->NetCafePointEnabled = Context->Config.Environment.NetCafePointEnabled;
//	Response->MinChatLevel = Context->Config.Environment.MinChatLevel;
    Response->MaxRank = 10;
	Response->MinShoutLevel = Context->Config.Environment.MinShoutLevel;
	Response->MinShoutSkillRank = Context->Config.Environment.MinShoutSkillRank;
	Response->MaxInventoryCurrency = Context->Config.Environment.MaxInventoryCurrency;
	Response->MaxWarehouseCurrency = Context->Config.Environment.MaxWarehouseCurrency;
	Response->MaxAuctionCurrency = Context->Config.Environment.MaxAuctionCurrency;
    Response->NetCafePremiumEnabled = 1;
    Response->GuildBoardEnabled = 1;
    Response->NetCafePremiumType = 0;
    Response->AgentShopEnabled = 1;
    Response->Unknown1 = 0;
    Response->Unknown2 = 0;
    Response->Unknown2 = 0;
    Response->MegaphoneShoutCooldownTime = 0;
    Response->MinDummyLevel = 0;
    Response->MinAgentShopLevel = 10;
    Response->MinPersonalShopLevel = 10;
    Response->TPointEnabled = 1;
    Response->GuildExpansionEnabled = 1;
    Response->LimitMegaphoneShoutEnabled = 0;
    Response->MinTalkLevel = 0;
    Response->MinTradeTalkLevel = 0;
    Response->MaxDP = 100;
    Response->Unknown4 = 0;
    Response->Unknown5 = 7;
    Response->Unknown6 = 1;
	Response->MaxHonorPoint = Context->Config.Environment.MaxHonorPoint;
	Response->MinHonorPoint = Context->Config.Environment.MinHonorPoint;
    Response->Unknown8 = 0;
    Response->UnknownCount = 3;

    S2C_DATA_ENVIRONMENT_WORLD_RESTRICTION* ResponseWorldRestriction = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_ENVIRONMENT_WORLD_RESTRICTION);
    ResponseWorldRestriction->Index = 1;
    ResponseWorldRestriction->StartIndex = 16;
    ResponseWorldRestriction->Count = 15;

    ResponseWorldRestriction = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_ENVIRONMENT_WORLD_RESTRICTION);
    ResponseWorldRestriction->Index = 2;
    ResponseWorldRestriction->StartIndex = 109;
    ResponseWorldRestriction->Count = 110;

    ResponseWorldRestriction = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_ENVIRONMENT_WORLD_RESTRICTION);
    ResponseWorldRestriction->Index = 3;
    ResponseWorldRestriction->StartIndex = 158;
    ResponseWorldRestriction->Count = 159;

    S2C_DATA_ENVIRONMENT_TAIL* ResponseTail = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_ENVIRONMENT_TAIL);
    ResponseTail->Unknown20 = 5;
    ResponseTail->Unknown21 = 0;
    ResponseTail->Unknown22 = 0;
    ResponseTail->Unknown23 = 0;
    ResponseTail->Unknown24 = 1;
	SocketSend(Socket, Connection, Response);
    return;

error:
	SocketDisconnect(Socket, Connection);
}
