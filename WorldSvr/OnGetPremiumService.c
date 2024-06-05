#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_PREMIUM_SERVICE) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->Account.AccountID < 1) goto error;

	IPC_W2M_DATA_GET_PREMIUM_SERVICE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2M, GET_PREMIUM_SERVICE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTER;
	Request->AccountID = Client->Account.AccountID;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

Void SendPremiumServiceList(
    ServerContextRef Context,
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    S2C_DATA_PREMIUM_BENEFIT_INFO_LIST* Response = PacketBufferInitExtended(Connection->PacketBuffer, S2C, PREMIUM_BENEFIT_INFO_LIST);
    Response->Count = Context->Runtime->Context->PremiumServiceCount;

    for (Index Index = 0; Index < Context->Runtime->Context->PremiumServiceCount; Index += 1) {
        RTDataPremiumServiceRef PremiumService = &Context->Runtime->Context->PremiumServiceList[Index];

        S2C_DATA_PREMIUM_BENEFIT_INFO* ResponseInfo = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_PREMIUM_BENEFIT_INFO);
        ResponseInfo->DurationServiceID = PremiumService->DurationServiceID;
        ResponseInfo->Type = PremiumService->Type;
        ResponseInfo->XP = PremiumService->XP;
        ResponseInfo->SkillXP = PremiumService->SkillXP;
        ResponseInfo->DropRate = PremiumService->DropRate;
        ResponseInfo->AlzBombRate = PremiumService->AlzBombRate;
        ResponseInfo->WarXP = PremiumService->WarXP;
        ResponseInfo->PetXP = PremiumService->PetXP;
        ResponseInfo->AXP = PremiumService->AXP;
        ResponseInfo->TPointUp = PremiumService->TPointUp;
        ResponseInfo->AlzDropRate = PremiumService->AlzDropRate;
        ResponseInfo->BoxDropRate = PremiumService->BoxDropRate;
        ResponseInfo->ForceWingXP = PremiumService->ForceWingXP;
        ResponseInfo->Inventory1 = PremiumService->Inventory1;
        ResponseInfo->Inventory2 = PremiumService->Inventory2;
        ResponseInfo->Inventory3 = PremiumService->Inventory3;
        ResponseInfo->Inventory4 = PremiumService->Inventory4;
        ResponseInfo->Inventory5 = PremiumService->Inventory5;
        ResponseInfo->Inventory6 = PremiumService->Inventory6;
        ResponseInfo->ExtendPetSlot = PremiumService->ExtendPetSlot;
        ResponseInfo->Warehouse = PremiumService->Warehouse;
        ResponseInfo->SkillXPTDummy = PremiumService->SkillXPTDummy;
        ResponseInfo->GPSWarpMask = PremiumService->GPSWarpMask;
        ResponseInfo->UnlimitedWarp = PremiumService->UnlimitedWarp;
        ResponseInfo->AuctionHouseItemBonusSlots = PremiumService->AuctionHouseItemBonusSlots;
        ResponseInfo->AuctionHouseItemBonusQuantity = PremiumService->AuctionHouseItemBonusQuantity;
        ResponseInfo->AuctionHouseItemBonusPeriod = PremiumService->AuctionHouseItemBonusPeriod;
        ResponseInfo->AuctionHouseFeeExemption = PremiumService->AuctionHouseFeeExemption;
        ResponseInfo->AbleToBuyPremiumItemFromShop = PremiumService->AbleToBuyPremiumItemFromShop;
        ResponseInfo->RemoteShop = PremiumService->RemoteShop;
        ResponseInfo->AbleToEnterPremiumDungeon = PremiumService->AbleToEnterPremiumDungeon;
        ResponseInfo->PremiumDungeonReward = PremiumService->PremiumDungeonReward;
        ResponseInfo->RemoteWarehouse = PremiumService->RemoteWarehouse;
        ResponseInfo->CraftMasteryUp = PremiumService->CraftMasteryUp;
        ResponseInfo->RequestAmityUp = PremiumService->RequestAmityUp;
        ResponseInfo->RequestAmitySlots = PremiumService->RequestAmitySlots;
        ResponseInfo->DungeonPlayTimeIncreased = PremiumService->DungeonPlayTimeIncreased;
    }

    SocketSend(Context->ClientSocket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(PREMIUM_BENEFIT_INFO_LIST) {
    SendPremiumServiceList(Context, Socket, Connection);
}

IPC_PROCEDURE_BINDING(M2W, GET_PREMIUM_SERVICE) {
	if (!ClientConnection || !Client) return;

    S2C_DATA_GET_PREMIUM_SERVICE* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, GET_PREMIUM_SERVICE);
    Response->ServiceType = Packet->ServiceType;
    Response->ExpirationDate = Packet->ExpiredAt;
    SocketSend(Context->ClientSocket, ClientConnection, Response);

	S2C_DATA_NFY_PREMIUM_SERVICE* Notification = PacketBufferInit(ClientConnection->PacketBuffer, S2C, NFY_PREMIUM_SERVICE);
	Notification->ServiceType = Packet->ServiceType;
	Notification->ExpirationDate = Packet->ExpiredAt;
	SocketSend(Context->ClientSocket, ClientConnection, Notification);

	S2C_DATA_NFY_UNKNOWN_2518* UnknownNotification = PacketBufferInit(ClientConnection->PacketBuffer, S2C, NFY_UNKNOWN_2518);
	/*
	UnknownNotification->Unknown1[0] = 4;
	UnknownNotification->Unknown1[1] = 1;
	UnknownNotification->Unknown1[2] = 2;
	UnknownNotification->Unknown1[3] = 0;
	UnknownNotification->Unknown1[4] = 20;
	UnknownNotification->Unknown1[5] = 1;
	*/
	SocketSend(Context->ClientSocket, ClientConnection, UnknownNotification);
}
