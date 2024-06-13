#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// TODO: Persist all official packets in postgres db with ostara while playing pinned to a client version

CLIENT_PROCEDURE_BINDING(BUY_ITEM) {
    if (!Character) goto error;

    // TODO: Add packet bounds checking for dynamically appended data!
    if (Packet->InventoryIndexCount < 1 || Packet->InventoryIndexCount > SERVER_CHARACTER_MAX_BUY_ITEM_COUNT) goto error;

    RTShopDataRef Shop = RTRuntimeGetShopByWorldNpcID(
        Runtime,
        Character->Data.Info.Position.WorldID,
        Packet->NpcID
    );
    if (!Shop) goto error;

    // TODO: Check NPC distance to character
    // TODO: Check and subtract item prices

    Int32 ItemIndex = -1;
    for (Int32 Index = 0; Index < Shop->ItemCount; Index++) {
        RTShopItemDataRef Item = &Shop->Items[Index];
        if (Item->SlotID == Packet->SlotIndex &&
            Item->ItemID == Packet->ItemID &&
            Item->ItemOption == Packet->ItemOptions) {
            ItemIndex = Index;
            break;
        }
    }
    if (ItemIndex < 0) goto error;

    RTShopItemDataRef Item = &Shop->Items[ItemIndex];
    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Item->ItemID);
    Bool Success = true;
    struct _RTItemSlot ItemSlot = { 0 };
    for (Int32 Index = 0; Index < Packet->InventoryIndexCount; Index++) {
        ItemSlot.SlotIndex = Packet->InventoryIndex[Index];
        ItemSlot.Item.ID = Item->ItemID;
        ItemSlot.ItemOptions = Item->ItemOption;

        Success &= RTInventorySetSlot(
            Runtime,
            &Character->Data.InventoryInfo,
            &ItemSlot
        );

        Character->SyncMask.InventoryInfo = true;

        if (!Success) 
            break;
    }

    if (!Success) goto error;

    RTCharacterUpdateQuestItemCounter(Runtime, Character, ItemSlot.Item, ItemSlot.ItemOptions);

    S2C_DATA_BUY_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, BUY_ITEM);
    Response->ItemID = ItemSlot.Item.Serial;
    Response->ItemOptions = ItemSlot.ItemOptions;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SELL_ITEM) {
    if (!Character) goto error;

    // TODO: Add packet bounds checking for dynamically appended data!
    if (Packet->InventoryIndexCount < 1) goto error;// || Packet->InventoryIndexCount > SERVER_CHARACTER_MAX_SELL_ITEM_COUNT) goto error;

    RTShopDataRef Shop = RTRuntimeGetShopByWorldNpcID(
        Runtime,
        Character->Data.Info.Position.WorldID,
        Packet->NpcID
    );
    if (!Shop) {
        Bool Found = false;
        for (Index Index = 0; Index < Runtime->Context->EventCount; Index += 1) {
            RTDataEventRef Event = &Runtime->Context->EventList[Index];
            if (Event->NpcIndex == Packet->NpcID && Event->EventShopCount > 0) {                
                Found = true;
                break;
            }
        }

        if (!Found) goto error;
    }

    // TODO: Check NPC distance to character

    for (Int32 Index = 0; Index < Packet->InventoryIndexCount; Index++) {
        Int32 ItemSlotIndex = Packet->InventoryIndex[Index];
        RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, ItemSlotIndex);
        if (!ItemSlot) goto error;

        RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
        if (!ItemData) goto error;

        Int32 RecoverySlotIndex = Character->Data.RecoveryInfo.Count % RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT;
        Character->Data.RecoveryInfo.Prices[RecoverySlotIndex] = ItemData->SellPrice;
        Character->Data.RecoveryInfo.Slots[RecoverySlotIndex] = *ItemSlot;
        Character->Data.RecoveryInfo.Count = MIN(Character->Data.RecoveryInfo.Count + 1, RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT);

        // TODO: This is probably causing issues with other items still being involved inside the inventory...
        if (ItemData->ItemType == RUNTIME_ITEM_TYPE_QUEST_S) {
            UInt64 QuestItemOptions = RTQuestItemOptions(RTQuestItemGetOptions(ItemSlot->ItemOptions), 0);
            RTCharacterUpdateQuestItemCounter(Runtime, Character, ItemSlot->Item, QuestItemOptions);
        }

        RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlotIndex);

        // TODO: Calculate sell price based on upgrade level ...
        Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] += ItemData->SellPrice;
        Character->SyncMask.Info = true;
        Character->SyncMask.InventoryInfo = true;
    }

    S2C_DATA_SELL_ITEM *Response = PacketBufferInit(Connection->PacketBuffer, S2C, SELL_ITEM);
    Response->Currency = Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_SHOP_LIST) {
    if (!Character) goto error;

    S2C_DATA_GET_SHOP_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_SHOP_LIST);
    Response->Count = Runtime->ShopDataCount;
    
    for (Int32 Index = 0; Index < Runtime->ShopDataCount; Index++) {
        RTShopDataRef ShopData = &Runtime->ShopData[Index];

        S2C_DATA_GET_SHOP_LIST_INDEX* ShopIndex = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_GET_SHOP_LIST_INDEX);
        ShopIndex->WorldID = (UInt32)ShopData->WorldID;
        ShopIndex->ShopID = ShopData->NpcID;
        ShopIndex->ShopIndex = (UInt16)ShopData->Index;
        ShopIndex->IsCouponShop = 0;
    }

    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_SHOP_DATA) {
    if (!Character) goto error;

    RTShopDataRef ShopData = RTRuntimeGetShopByIndex(Runtime, Packet->ShopIndex);
    if (!ShopData) goto error;

    S2C_DATA_GET_SHOP_DATA* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_SHOP_DATA);
    Response->ShopIndex = Packet->ShopIndex;
    Response->Count = ShopData->ItemCount;

    for (Int32 Index = 0; Index < ShopData->ItemCount; Index++) {
        RTShopItemDataRef ItemData = &ShopData->Items[Index];
         
        S2C_DATA_GET_SHOP_DATA_INDEX* ItemIndex = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_GET_SHOP_DATA_INDEX);
        ItemIndex->ShopSlotIndex = ItemData->SlotID;
        ItemIndex->ItemID = ItemData->ItemID;
        ItemIndex->ItemOptions = ItemData->ItemOption;
        ItemIndex->MinHonorRank = (Int8)ItemData->MinHonorRank;
        ItemIndex->MaxHonorRank = 30; // TODO: Add configuration for max honor rank
        ItemIndex->AlzPrice = ItemData->Price;
        ItemIndex->DpPrice = (UInt16)ItemData->DpPrice;
        ItemIndex->WexpPrice = ItemData->WexpPrice;
    }

    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_ITEM_RECOVERY_LIST) {
    if (!Character) goto error;

    S2C_DATA_GET_ITEM_RECOVERY_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_ITEM_RECOVERY_LIST);
    Response->Count = Character->Data.RecoveryInfo.Count;

    for (Int32 Index = 0; Index < RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT; Index += 1) {
        RTItemSlotRef ItemSlot = &Character->Data.RecoveryInfo.Slots[Index];
        if (!ItemSlot->Item.Serial) continue;

        UInt64 RecoveryPrice = Character->Data.RecoveryInfo.Prices[Index];

        S2C_ITEM_RECOVERY_LIST_SLOT* ResponseData = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_ITEM_RECOVERY_LIST_SLOT);
        ResponseData->ItemID = ItemSlot->Item.ID;
        ResponseData->ItemSerial = ItemSlot->Item.Serial;
        ResponseData->ItemOption = ItemSlot->ItemOptions;
        ResponseData->RecoveryPrice = RecoveryPrice;
        ResponseData->RecoverySlotIndex = Index;
        ResponseData->ExpirationTimestamp = 0;
    }

    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(RECOVER_ITEM) {
    if (!Character) goto error;

    if (Packet->RecoverySlotIndex >= RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT) goto error;

    RTItemSlotRef RecoverySlot = &Character->Data.RecoveryInfo.Slots[Packet->RecoverySlotIndex];
    if (!RecoverySlot->Item.Serial) goto error;

    UInt64 RecoveryPrice = Character->Data.RecoveryInfo.Prices[Packet->RecoverySlotIndex];
    if (Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < RecoveryPrice) goto error;

    RecoverySlot->SlotIndex = Packet->InventorySlotIndex;

    if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, RecoverySlot)) goto error;

    memset(RecoverySlot, 0, sizeof(struct _RTItemSlot));
    Character->Data.RecoveryInfo.Count -= 1;

    RTCharacterUpdateQuestItemCounter(Runtime, Character, RecoverySlot->Item, RecoverySlot->ItemOptions);

    Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= RecoveryPrice;
    Character->SyncMask.Info = true;
    Character->SyncMask.InventoryInfo = true;

    S2C_DATA_RECOVER_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, RECOVER_ITEM);
    Response->Result = 1; // TODO: Check Result types and remove disconnects
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}
