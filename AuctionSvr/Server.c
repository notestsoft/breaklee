#include "ClientSocket.h"
#include "ClientProtocol.h"
#include "Server.h" 
#include "IPCCommands.h"
#include "IPCProtocol.h"

Void SendEnvironmentNotification(
    ServerContextRef Context,
    SocketConnectionRef Connection
) {
    S2C_DATA_SERVER_ENVIRONMENT* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, SERVER_ENVIRONMENT);
    Notification->MaxSearchResultCount = Context->Config.Environment.MaxSearchResultCount;
    Notification->BuyListCooldown = Context->Config.Environment.BuyListCooldown;
    Notification->SellListCooldown = Context->Config.Environment.SellListCooldown;
    Notification->CancelSlotCooldown = Context->Config.Environment.CancelSlotCooldown;
    Notification->NoCancelCommissionTimeout = Context->Config.Environment.NoCancelCommissionTimeout;
    Notification->NoServerResponseTimeout = Context->Config.Environment.NoServerResponseTimeout;
    Notification->UseForceGem = Context->Config.Environment.UseForceGem;
    Notification->CommissionRateForSell = Context->Config.Environment.CommissionRateForSell;
    Notification->CommissionRateForSellForceGem = Context->Config.Environment.CommissionRateForSellForceGem;
    Notification->MaxSearchResultCount = Context->Config.Environment.MaxSearchResultCount;
    CStringCopySafe(Notification->Name, 21, Context->Config.Environment.Name);
    Notification->AlertAveragePrice = Context->Config.Environment.AlertAveragePrice;
    Notification->EntrySlotCount = Context->Config.Environment.EntrySlotCount;
    Notification->EntrySlotDurationInMinutes = 10140;
    Notification->EntrySlotMaxQuantity = 100;
    Notification->DefaultSlotCount = Context->Config.Environment.DefaultSlotCount;
    Notification->PremiumSlotCount = Context->Config.Environment.PremiumSlotCount;
    Notification->Unknown1 = 94371840;
    Notification->Unknown2[0] = 3;
    Notification->Unknown2[1] = 3;
    Notification->Unknown2[2] = 15;
    Notification->Unknown2[3] = 1000;
    Notification->Unknown2[4] = 100;
    Notification->Unknown2[5] = 1000;
    Notification->Unknown2[6] = 100;
    Notification->Unknown2[7] = 1000;
    Notification->Unknown2[8] = 500;
    Notification->Unknown2[9] = 1000;
    Notification->Unknown2[10] = 500;
    Notification->Unknown2[11] = 5;
    Notification->Unknown2[12] = 5;
    Notification->Unknown2[13] = 0;
    Notification->Unknown2[14] = 0;
    Notification->Unknown2[15] = 0;
    SocketSend(Context->ClientSocket, Connection, Notification);
}

ClientContextRef ServerGetClientByIndex(
    ServerContextRef Context,
    UInt32 CharacterIndex,
    Int32 WorldServerIndex
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client && Client->CharacterIndex == CharacterIndex && Client->WorldServerIndex == WorldServerIndex) {
            return Client;
        }
    }

    return NULL;
}
