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

    for (Int Index = 1; Index < Packet->ItemCount; Index += 1) {
        RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
        if (FirstItemSlot->Item.Serial != ItemSlot->Item.Serial ||
            FirstItemSlot->ItemOptions != ItemSlot->ItemOptions) goto error;
    }

    UInt64 ItemStackSizeMask = RTItemDataGetStackSizeMask(FirstItemData);
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
    Request->CharacterIndex = (UInt32)Client->CharacterIndex;
    Request->SlotIndex = Packet->SlotIndex;
    Request->ItemCount = Packet->ItemCount;
    Request->ItemPrice = Packet->ItemPrice;
    Request->CategoryIndex[0] = MarketCategoryMainGroup->Category1;
    Request->CategoryIndex[1] = MarketListItem->Category2;
    Request->CategoryIndex[2] = MarketListItem->Category3;
    Request->CategoryIndex[3] = MarketListItem->Category4;
    Request->CategoryIndex[4] = 0; // TODO: Add special category handling for category 4 and 5 based on item upgrade level and epic option
    Request->ExpirationTime = Context->Config.Environment.AuctionExpirationTime;

    for (Int Index = 0; Index < Packet->ItemCount; Index += 1) {
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
    {
        S2C_DATA_AUCTION_HOUSE_REGISTER_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, AUCTION_HOUSE_REGISTER_ITEM);
        Response->Result = 179;
        Response->ExpirationDate = 0;
        SocketSend(Socket, Connection, Response);
    }
}

IPC_PROCEDURE_BINDING(D2W, AUCTION_REGISTER_ITEM) {
    if (!ClientConnection) return;

    S2C_DATA_AUCTION_HOUSE_REGISTER_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, AUCTION_HOUSE_REGISTER_ITEM);
    Response->Result = Packet->Result;
    Response->ExpirationDate = Packet->ExpirationDate;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(AUCTION_HOUSE_UPDATE_ITEM) {
    if (!Character) goto error;

    Int32 PacketLength = sizeof(C2S_DATA_AUCTION_HOUSE_UPDATE_ITEM) + sizeof(UInt16) * Packet->InventorySlotCount;
    if (Packet->Length != PacketLength) goto error;

    IPC_W2D_DATA_AUCTION_UPDATE_ITEM* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, AUCTION_UPDATE_ITEM);
    Request->Header.SourceConnectionID = Client->Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB;
    Request->AccountID = Client->AccountID;
    Request->CharacterIndex = (UInt32)Client->CharacterIndex;
    Request->SlotIndex = Packet->SlotIndex;
    Request->ItemCount = Packet->ItemCount;
    Request->ItemPriceType = Packet->ItemPriceType;
    Request->ItemPrice = Packet->ItemPrice; 
    Request->ExpirationTime = Context->Config.Environment.AuctionExpirationTime;

    if (Packet->ItemCountDecreased || !Packet->InventorySlotCount) {
        Request->IsDecrease = true;

        IPC_W2D_DATA_AUCTION_UPDATE_ITEM_DECREASE* RequestData = IPCPacketBufferAppendStruct(Server->IPCSocket->PacketBuffer, IPC_W2D_DATA_AUCTION_UPDATE_ITEM_DECREASE);
        RequestData->InventorySlotCount = Packet->InventorySlotCount;

        if (Packet->InventorySlotCount > 0) {
            IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Packet->InventorySlotIndex[0], sizeof(UInt16) * Packet->InventorySlotCount);
        }
    }
    else {
        Request->IsDecrease = false;

        IPC_W2D_DATA_AUCTION_UPDATE_ITEM_INCREASE* RequestData = IPCPacketBufferAppendStruct(Server->IPCSocket->PacketBuffer, IPC_W2D_DATA_AUCTION_UPDATE_ITEM_INCREASE);

        RTItemSlotRef FirstItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[0]);
        RTItemDataRef FirstItemData = (FirstItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, FirstItemSlot->Item.ID) : NULL;
        if (!FirstItemSlot || !FirstItemData) goto error;

        for (Int Index = 1; Index < Packet->InventorySlotCount; Index += 1) {
            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
            if (FirstItemSlot->Item.Serial != ItemSlot->Item.Serial ||
                FirstItemSlot->ItemOptions != ItemSlot->ItemOptions) goto error;
        }

        RequestData->InventorySlotCount = Packet->InventorySlotCount;
        for (Int Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
            struct _RTItemSlot ItemSlot = { 0 };
            RTInventoryRemoveSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index], &ItemSlot);
            IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &ItemSlot, sizeof(struct _RTItemSlot));
        }

        RequestData->InventoryInfo = Character->Data.InventoryInfo.Info;
        if (RequestData->InventoryInfo.SlotCount > 0) {
            IPCPacketBufferAppendCopy(
                Server->IPCSocket->PacketBuffer,
                &Character->Data.InventoryInfo.Slots,
                sizeof(struct _RTItemSlot) * Character->Data.InventoryInfo.Info.SlotCount
            );
        }
    }

    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, AUCTION_UPDATE_ITEM) {
    if (!ClientConnection || !Character) return;

    if (Packet->Result == 0 && Packet->IsDecrease) {
        IPC_D2W_DATA_AUCTION_UPDATE_ITEM_DECREASE* PacketPayload = (IPC_D2W_DATA_AUCTION_UPDATE_ITEM_DECREASE*)&Packet->Data[0];
        for (Int Index = 0; Index < PacketPayload->InventorySlotCount; Index += 1) {
            PacketPayload->ItemSlot.SlotIndex = PacketPayload->InventorySlotIndex[Index];
            if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &PacketPayload->ItemSlot)) {
                PacketPayload->ItemSlot.SlotIndex = RTInventoryGetNextFreeSlotIndex(Runtime, &Character->Data.TemporaryInventoryInfo);
                RTInventoryInsertSlot(Runtime, &Character->Data.TemporaryInventoryInfo, &PacketPayload->ItemSlot);
            }
        }

        Character->SyncMask.InventoryInfo = true;
    }

    S2C_DATA_AUCTION_HOUSE_UPDATE_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, AUCTION_HOUSE_UPDATE_ITEM);
    Response->Result = Packet->Result;
    Response->ExpirationDate = Packet->ExpirationDate;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(AUCTION_HOUSE_UNREGISTER_ITEM) {
    if (!Character) goto error;

    Int32 PacketLength = sizeof(C2S_DATA_AUCTION_HOUSE_UNREGISTER_ITEM) + sizeof(UInt16) * Packet->InventorySlotCount;
    if (Packet->Length != PacketLength) goto error;

    IPC_W2D_DATA_AUCTION_UNREGISTER_ITEM* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, AUCTION_UNREGISTER_ITEM);
    Request->Header.SourceConnectionID = Client->Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB;
    Request->AccountID = Client->AccountID;
    Request->SlotIndex = Packet->SlotIndex;
    Request->InventorySlotCount = Packet->InventorySlotCount;

    if (Packet->InventorySlotCount > 0) {
        IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Packet->InventorySlotIndex[0], sizeof(UInt16) * Packet->InventorySlotCount);
    }

    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, AUCTION_UNREGISTER_ITEM) {
    if (!ClientConnection || !Character) return;

    if (Packet->Result == 0) {
        for (Int Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
            Packet->ItemSlot.SlotIndex = Packet->InventorySlotIndex[Index];
            if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &Packet->ItemSlot)) {
                Packet->ItemSlot.SlotIndex = RTInventoryGetNextFreeSlotIndex(Runtime, &Character->Data.TemporaryInventoryInfo);
                RTInventoryInsertSlot(Runtime, &Character->Data.TemporaryInventoryInfo, &Packet->ItemSlot);
            }
        }

        Character->SyncMask.InventoryInfo = true;
    }

    S2C_DATA_AUCTION_HOUSE_UNREGISTER_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, AUCTION_HOUSE_UNREGISTER_ITEM);
    Response->Result = Packet->Result;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(AUCTION_HOUSE_PROCEED_ITEM) {
    if (!Character) goto error;

    if (Packet->UseCoupon) {
        RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
        if (!ItemSlot) goto error;

        RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
        if (!ItemData) goto error;

        assert(true);
    }

    IPC_W2D_DATA_AUCTION_PROCEED_ITEM* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, AUCTION_PROCEED_ITEM);
    Request->Header.SourceConnectionID = Client->Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB;
    Request->AccountID = Client->AccountID;
    Request->SlotIndex = Packet->SlotIndex;
    Request->IsCommissionFree = Packet->UseCoupon;
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, AUCTION_PROCEED_ITEM) {
    if (!ClientConnection || !Character) return;

    Character->Data.Info.Alz += Packet->ReceivedAlz;
    Character->SyncMask.Info = true;

    S2C_DATA_AUCTION_HOUSE_PROCEED_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, AUCTION_HOUSE_PROCEED_ITEM);
    Response->Result = Packet->Result;
    Response->SoldItemCount = Packet->SoldItemCount;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(AUCTION_HOUSE_BUY_ITEM) {
    if (!Character) goto error;

    Int32 PacketLength = sizeof(C2S_DATA_AUCTION_HOUSE_BUY_ITEM) + Packet->InventorySlotCount * sizeof(UInt16);
    if (Packet->Length != PacketLength) goto error;

    Int64 TotalPrice = Packet->InventorySlotCount * Packet->ItemPrice;
    if (Packet->ItemPriceType == 0 && TotalPrice > Character->Data.Info.Alz) goto error;
    if (Packet->ItemPriceType == 1 && TotalPrice > Character->Data.AccountInfo.ForceGem) goto error;
    if (Packet->ItemPriceType > 1) goto error;

    IPC_W2D_DATA_AUCTION_BUY_ITEM* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, AUCTION_BUY_ITEM); 
    Request->Header.SourceConnectionID = Client->Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB;
    Request->AccountID = Client->AccountID;
    Request->CharacterIndex = (UInt32)Character->CharacterIndex;
    Request->ItemAccountID = Packet->AccountID;
    Request->Unknown1 = Packet->Unknown1;
    Request->ItemID = Packet->ItemID;
    Request->ItemOptions = Packet->ItemOptions;
    Request->ItemOptionExtended = Packet->ItemOptionExtended;
    Request->ItemPriceType = Packet->ItemPriceType;
    Request->ItemPrice = Packet->ItemPrice;
    CStringCopySafe(Request->ItemName, MAX_AUCTION_ITEM_NAME_LENGTH, Packet->ItemName);
    Request->InventorySlotCount = Packet->InventorySlotCount;
    IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Packet->InventorySlotIndex[0], Packet->InventorySlotCount * sizeof(UInt16));
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
    {
        S2C_DATA_AUCTION_HOUSE_BUY_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, AUCTION_HOUSE_BUY_ITEM);
        Response->Result = 1;
        SocketSend(Socket, Connection, Response);
    }
}

IPC_PROCEDURE_BINDING(D2W, AUCTION_BUY_ITEM) {
    if (!ClientConnection || !Character) return;

    Int64 TotalPrice = Packet->InventorySlotCount * Packet->ItemPrice;
    if (Packet->ItemPriceType == 0) {
        Character->Data.Info.Alz -= TotalPrice;
    }
    else if (Packet->ItemPriceType == 1) {
        Character->Data.AccountInfo.ForceGem -= TotalPrice;
    }
    
    for (Int Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
        struct _RTItemSlot ItemSlot = {
            .Item.Serial = Packet->ItemID,
            .ItemOptions = Packet->ItemOptions,
            .ItemDuration = Packet->ItemOptionExtended,
            .SlotIndex = Packet->InventorySlotIndex[Index]
        };

        RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &ItemSlot);
    }

    Character->SyncMask.Info = true;
    Character->SyncMask.InventoryInfo = true;

    S2C_DATA_AUCTION_HOUSE_BUY_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, AUCTION_HOUSE_BUY_ITEM);
    Response->Result = Packet->Result;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}
