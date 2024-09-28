#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// TODO: If the sync process fails we have to make sure the data is not lost!

CLIENT_PROCEDURE_BINDING(AUCTION_HOUSE_REGISTER_ITEM) {
    if (!Character) goto error;

    Int32 PacketLength = sizeof(C2S_DATA_AUCTION_HOUSE_REGISTER_ITEM) + Packet->ItemCount * sizeof(UInt16);
    if (Packet->Length != PacketLength) goto error;
    if (Packet->ItemCount < 1) goto error;

    RTItemSlotRef FirstItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[0]);
    RTItemDataRef FirstItemData = (FirstItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, FirstItemSlot->Item.ID) : NULL;
    if (!FirstItemSlot) goto error;

    for (Int32 Index = 1; Index < Packet->ItemCount; Index += 1) {
        RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
        if (FirstItemSlot->Item.Serial != ItemSlot->Item.Serial ||
            FirstItemSlot->ItemOptions != ItemSlot->ItemOptions) goto error;
    }

    UInt64 ItemStackSizeMask = RTItemDataGetStackSizeMask(FirstItemData);
    Int64 ItemStackSize = FirstItemSlot->ItemOptions & ItemStackSizeMask;
    UInt64 ItemOptions = FirstItemSlot->ItemOptions & ~ItemStackSizeMask;

    RTDataMarketListItemRef MarketListItem = RTRuntimeDataMarketListItemGet(Runtime->Context, FirstItemSlot->Item.ID, ItemOptions);
    RTDataMarketCategoryMainGroupRef MarketCategoryMainGroup = (MarketListItem) ? RTRuntimeDataMarketCategoryMainGroupGetByCategory2(Runtime->Context, MarketListItem->Category2) : NULL;
    if (!MarketListItem || !MarketCategoryMainGroup) goto error;

    IPC_W2D_DATA_AUCTION_REGISTER_ITEM* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, AUCTION_REGISTER_ITEM);
    Request->Header.SourceConnectionID = Client->Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB;
    Request->AccountID = Client->AccountID;
    Request->CharacterID = Client->CharacterDatabaseID;
    Request->CharacterIndex = Client->CharacterIndex;
    Request->SlotIndex = Packet->SlotIndex;
    Request->ItemCount = Packet->ItemCount;
    Request->ItemPrice = Packet->ItemPrice;
    Request->CategoryIndex[0] = MarketCategoryMainGroup->Category1;
    Request->CategoryIndex[1] = MarketListItem->Category2;
    Request->CategoryIndex[2] = MarketListItem->Category3;
    Request->CategoryIndex[3] = MarketListItem->Category4;
    Request->CategoryIndex[4] = 0; // TODO: Add special category handling for category 4 and 5 based on item upgrade level and epic option
    Request->ExpirationTime = Context->Config.Environment.AuctionExpirationTime;

    for (Int32 Index = 0; Index < Packet->ItemCount; Index += 1) {
        struct _RTItemSlot ItemSlot = { 0 };
        RTInventoryRemoveSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index], &ItemSlot);
        IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &ItemSlot, sizeof(struct _RTItemSlot));
    }

    Request->InventoryInfo = Character->Data.InventoryInfo.Info;
    if (Request->InventoryInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Server->IPCSocket->PacketBuffer, 
            &Character->Data.InventoryInfo.Slots, 
            sizeof(struct _RTItemSlot) * Character->Data.InventoryInfo.Info.SlotCount
        );
    }

    Character->SyncMask.InventoryInfo = false;
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, AUCTION_REGISTER_ITEM) {
    if (!ClientConnection) return;

    S2C_DATA_AUCTION_HOUSE_REGISTER_ITEM* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, AUCTION_HOUSE_REGISTER_ITEM);
    Response->Result = Packet->Result;
    Response->ExpirationDate = Packet->ExpirationDate;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(AUCTION_HOUSE_UPDATE_ITEM) {
}

CLIENT_PROCEDURE_BINDING(AUCTION_HOUSE_UNREGISTER_ITEM) {

}