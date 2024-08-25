#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(BUY_ITEM) {
    if (!Character) goto error;

    // TODO: Add packet bounds checking for dynamically appended data!
    // TODO: Check NPC distance to character

    if (Packet->ItemCount < 1 || Packet->ItemCount > SERVER_CHARACTER_MAX_BUY_ITEM_COUNT) goto error;
    if (Packet->ItemPriceCount < 0 || Packet->ItemPriceCount > SERVER_CHARACTER_MAX_BUY_ITEM_PRICE_COUNT) goto error;

    Int32 WorldIndex = (!Packet->IsRemoteShop) ? Character->Data.Info.WorldIndex : 0;
    RTDataShopIndexRef ShopIndex = RTRuntimeDataShopIndexGet(Runtime->Context, WorldIndex, Packet->NpcIndex);
    RTDataShopPoolRef ShopPool = (ShopIndex) ? RTRuntimeDataShopPoolGet(Runtime->Context, ShopIndex->ShopIndex) : NULL;
    RTDataShopItemRef ShopItem = (ShopPool) ? RTRuntimeDataShopItemGet(ShopPool, Packet->TabIndex, Packet->SlotIndex) : NULL;
    if (!ShopItem) goto error;

    if (ShopItem->ItemID != Packet->ItemID) goto error;
    if (ShopItem->ItemOptions != Packet->ItemOptions) goto error;
    if (ShopItem->MinLevel && ShopItem->MinLevel > Character->Data.Info.Level) goto error;
    if (ShopItem->MaxLevel && ShopItem->MaxLevel < Character->Data.Info.Level) goto error;

    Int32 HonorRank = RTCharacterGetHonorRank(Runtime, Character);
    if (ShopItem->MinHonorRank > HonorRank) goto error;
    if (ShopItem->MaxHonorRank < HonorRank) goto error;
    // if (ShopItem->IsPremiumOnly && /* TODO: Check Character Premium Service */) goto error;
    // if (ShopItem->IsWinningOnly && /* TODO: Check Character Bringer Status */) goto error;

    Int64 PriceAlz = ShopItem->PriceAlz * Packet->ItemCount;
    Int64 PriceWexp = ShopItem->PriceWexp * Packet->ItemCount;
    Int64 PriceAP = ShopItem->PriceAP * Packet->ItemCount;
    Int64 PriceDP = ShopItem->PriceDP * Packet->ItemCount;
    Int64 PriceCash = ShopItem->PriceCash * Packet->ItemCount;
    Int64 PriceGem = ShopItem->PriceGem * Packet->ItemCount;

    if (PriceAlz > Character->Data.Info.Alz) goto error;
    if (PriceWexp > Character->Data.Info.Wexp) goto error;
    if (PriceAP > Character->Data.AbilityInfo.Info.AP) goto error;
    // if (PriceDP > /* TODO: Check Character DP Amount */) goto error;
    // if (PriceCash > /* TODO: Check Character Cash Amount */) goto error;
    if (PriceGem > Character->Data.AccountInfo.ForceGem) goto error;

    if (ShopItem->ShopPricePoolIndex) {
        Int32 ItemPriceIndex = 0;

        for (Int32 Index = 0; Index < Runtime->Context->ShopPricePoolCount; Index += 1) {
            RTDataShopPricePoolRef PricePool = &Runtime->Context->ShopPricePoolList[Index];
            if (PricePool->PoolIndex != ShopItem->ShopPricePoolIndex) continue;

            Int64 ConsumableItemCount = 0;
            while (ConsumableItemCount < PricePool->ItemCount && ItemPriceIndex < Packet->ItemPriceCount) {
                S2C_DATA_ITEM_PRICE_INDEX* ItemPrice = &Packet->ItemPriceList[ItemPriceIndex];

                RTItemSlotRef PriceItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, ItemPrice->SlotIndex);
                RTItemDataRef PriceItemData = (PriceItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, PriceItemSlot->Item.ID) : NULL;
                if (!PriceItemSlot || !PriceItemData) goto error;

                UInt64 ItemStackSizeMask = RTItemDataGetStackSizeMask(PriceItemData);
                UInt64 ItemOptionsOffset = RTItemDataGetItemOptionsOffset(PriceItemData);
                Int64 ItemStackSize = PriceItemSlot->ItemOptions & ItemStackSizeMask;
                UInt64 ItemOptions = (PriceItemSlot->ItemOptions & ~ItemStackSizeMask) >> ItemOptionsOffset;
                
                if (PriceItemData->MaxStackSize < 1) {
                    ItemStackSize = 1;
                }

                if (PricePool->ItemID != PriceItemSlot->Item.Serial) goto error;
                if (PricePool->ItemOptions != ItemOptions) goto error;

                ConsumableItemCount += MIN(ItemStackSize, ItemPrice->Count);
                ItemPriceIndex += 1;
            }

            if (ConsumableItemCount < PricePool->ItemCount) goto error;
        }

        ItemPriceIndex = 0;

        for (Int32 Index = 0; Index < Runtime->Context->ShopPricePoolCount; Index += 1) {
            RTDataShopPricePoolRef PricePool = &Runtime->Context->ShopPricePoolList[Index];
            if (PricePool->PoolIndex != ShopItem->ShopPricePoolIndex) continue;

            Int64 RemainingItemCount = PricePool->ItemCount;
            while (RemainingItemCount > 0 && ItemPriceIndex < Packet->ItemPriceCount) {
                S2C_DATA_ITEM_PRICE_INDEX* ItemPrice = &Packet->ItemPriceList[ItemPriceIndex];

                RTItemSlotRef PriceItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, ItemPrice->SlotIndex);
                RTItemDataRef PriceItemData = (PriceItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, PriceItemSlot->Item.ID) : NULL;
                if (!PriceItemSlot || !PriceItemData) goto error;

                UInt64 ItemStackSizeMask = RTItemDataGetStackSizeMask(PriceItemData);
                Int64 ItemStackSize = PriceItemSlot->ItemOptions & ItemStackSizeMask;
                UInt64 ItemOptions = PriceItemSlot->ItemOptions & ~ItemStackSizeMask;

                if (PriceItemData->MaxStackSize < 1) {
                    ItemStackSize = 1;
                }
                
                RemainingItemCount -= MIN(ItemStackSize, ItemPrice->Count);
                ItemStackSize -= MIN(ItemStackSize, ItemPrice->Count);
                PriceItemSlot->ItemOptions = ItemOptions | (ItemStackSize & ItemStackSizeMask);

                if (ItemStackSize <= 0) {
                    RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemPrice->SlotIndex);
                }

                ItemPriceIndex += 1;
            }

            assert(RemainingItemCount <= 0);
        }
    }

    Bool Success = true;
    struct _RTItemSlot ItemSlot = { 0 };
    for (Int32 Index = 0; Index < Packet->ItemCount; Index += 1) {
        ItemSlot.SlotIndex = Packet->InventoryIndex[Index];
        ItemSlot.Item.Serial = ShopItem->ItemID;
        ItemSlot.ItemOptions = ShopItem->ItemOptions;
        ItemSlot.ItemDuration.Serial = ShopItem->ItemDuration;
        Success &= RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &ItemSlot);
        RTCharacterUpdateQuestItemCounter(Runtime, Character, ItemSlot.Item, ItemSlot.ItemOptions);

        if (!Success) break;
    }

    if (!Success) goto error;

    Character->Data.Info.Alz-= PriceAlz;
    Character->Data.Info.Wexp -= PriceWexp;
    Character->Data.AbilityInfo.Info.AP -= PriceAP;
    // TODO: Consume DP
    // TODO: Consume Cash
    RTCharacterConsumeForceGem(Runtime, Character, PriceGem);
    Character->SyncMask.Info = true;
    Character->SyncMask.AbilityInfo = true;
    Character->SyncMask.InventoryInfo = true;

    S2C_DATA_BUY_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, BUY_ITEM);
    Response->ItemID = ItemSlot.Item.Serial;
    Response->ItemOptions = ItemSlot.ItemOptions;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SELL_ITEM) {
    if (!Character) goto error;

    // TODO: Add packet bounds checking for dynamically appended data!
    if (Packet->InventoryIndexCount < 1) goto error;
    // || Packet->InventoryIndexCount > SERVER_CHARACTER_MAX_SELL_ITEM_COUNT) goto error;

    Int32 WorldIndex = (!Packet->IsRemoteShop) ? Character->Data.Info.WorldIndex : 0;
    RTDataShopIndexRef ShopIndex = RTRuntimeDataShopIndexGet(Runtime->Context, WorldIndex, Packet->NpcIndex);
    if (!ShopIndex) {
        Bool Found = false;
        for (Index Index = 0; Index < Runtime->Context->EventCount; Index += 1) {
            RTDataEventRef Event = &Runtime->Context->EventList[Index];
            if (Event->NpcIndex == Packet->NpcIndex && Event->EventShopCount > 0) {                
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

        Int32 RecoverySlotIndex = Character->Data.RecoveryInfo.Info.SlotCount % RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT;
        Character->Data.RecoveryInfo.Prices[RecoverySlotIndex] = ItemData->SellPrice;
        Character->Data.RecoveryInfo.Slots[RecoverySlotIndex] = *ItemSlot;
        Character->Data.RecoveryInfo.Info.SlotCount = MIN(Character->Data.RecoveryInfo.Info.SlotCount + 1, RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT);

        // TODO: This is probably causing issues with other items still being involved inside the inventory...
        if (ItemData->ItemType == RUNTIME_ITEM_TYPE_QUEST_S) {
            UInt64 QuestItemOptions = RTQuestItemOptions(RTQuestItemGetOptions(ItemSlot->ItemOptions), 0);
            RTCharacterUpdateQuestItemCounter(Runtime, Character, ItemSlot->Item, QuestItemOptions);
        }

        RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlotIndex);

        // TODO: Calculate sell price based on upgrade level ...
        Character->Data.Info.Alz += ItemData->SellPrice;
        Character->SyncMask.Info = true;
        Character->SyncMask.InventoryInfo = true;
    }

    S2C_DATA_SELL_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SELL_ITEM);
    Response->Currency = Character->Data.Info.Alz;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_SHOP_LIST) {
    if (!Character) goto error;

    S2C_DATA_GET_SHOP_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_SHOP_LIST);
    Response->Count = Runtime->Context->ShopIndexCount;
    
    for (Int32 Index = 0; Index < Runtime->Context->ShopIndexCount; Index += 1) {
        RTDataShopIndexRef ShopIndex = &Runtime->Context->ShopIndexList[Index];

        S2C_DATA_GET_SHOP_LIST_INDEX* ResponseIndex = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_GET_SHOP_LIST_INDEX);
        ResponseIndex->WorldIndex = (UInt32)ShopIndex->WorldIndex;
        ResponseIndex->NpcIndex = ShopIndex->NpcIndex;
        ResponseIndex->ShopIndex = (UInt16)ShopIndex->ShopIndex;
        ResponseIndex->IsCouponShop = ShopIndex->IsCouponShop;
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_SHOP_DATA) {
    if (!Character) goto error;

    RTDataShopPoolRef ShopPool = RTRuntimeDataShopPoolGet(Runtime->Context, Packet->ShopIndex);
    if (!ShopPool) goto error;

    S2C_DATA_GET_SHOP_DATA* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_SHOP_DATA);
    Response->ShopIndex = ShopPool->ShopIndex;
    Response->Count = ShopPool->ShopItemCount;

    for (Int32 Index = 0; Index < ShopPool->ShopItemCount; Index += 1) {
        RTDataShopItemRef ShopItem = &ShopPool->ShopItemList[Index];
         
        S2C_DATA_GET_SHOP_DATA_INDEX* ResponseItem = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_GET_SHOP_DATA_INDEX);
        ResponseItem->TabIndex = ShopItem->TabIndex;
        ResponseItem->SlotIndex = ShopItem->SlotIndex;
        ResponseItem->ItemID = ShopItem->ItemID;
        ResponseItem->ItemOptions = ShopItem->ItemOptions;
        ResponseItem->ItemDuration = ShopItem->ItemDuration;
        ResponseItem->MinLevel = ShopItem->MinLevel;
        ResponseItem->MaxLevel = ShopItem->MaxLevel;
        ResponseItem->MinGuildLevel = ShopItem->MinGuildLevel;
        ResponseItem->MinHonorRank = ShopItem->MinHonorRank;
        ResponseItem->MaxHonorRank = ShopItem->MaxHonorRank;
        ResponseItem->IsPremiumOnly = ShopItem->IsPremiumOnly;
        ResponseItem->IsWinningOnly = ShopItem->IsWinningOnly;
        // TODO: How does the game know how many are already purchased?
        ResponseItem->DailyPurchaseLimit = ShopItem->DailyPurchaseLimit;
        ResponseItem->WeeklyPurchaseLimit = ShopItem->WeeklyPurchaseLimit;
        ResponseItem->PriceAlz = ShopItem->PriceAlz;
        ResponseItem->PriceWexp = ShopItem->PriceWexp;
        ResponseItem->PriceAP = ShopItem->PriceAP;
        ResponseItem->PriceDP = ShopItem->PriceDP;
        ResponseItem->PriceCash = ShopItem->PriceCash;
        ResponseItem->ShopPricePoolIndex = ShopItem->ShopPricePoolIndex;
        ResponseItem->PriceGem = ShopItem->PriceGem;
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_SHOP_ITEM_PRICE_POOL) {
    if (!Character) goto error;

    S2C_DATA_GET_SHOP_ITEM_PRICE_POOL* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_SHOP_ITEM_PRICE_POOL);
    Response->Count = Runtime->Context->ShopPricePoolCount;

    for (Int32 Index = 0; Index < Runtime->Context->ShopPricePoolCount; Index += 1) {
        RTDataShopPricePoolRef ShopPrice = &Runtime->Context->ShopPricePoolList[Index];

        S2C_DATA_GET_SHOP_ITEM_PRICE_POOL_INDEX* ResponsePrice = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_GET_SHOP_ITEM_PRICE_POOL_INDEX);
        ResponsePrice->PoolIndex = ShopPrice->PoolIndex;
        ResponsePrice->ItemID.Serial = ShopPrice->ItemID;
        ResponsePrice->ItemOptions = ShopPrice->ItemOptions;
        ResponsePrice->ItemCount = ShopPrice->ItemCount;
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_ITEM_RECOVERY_LIST) {
    if (!Character) goto error;

    S2C_DATA_GET_ITEM_RECOVERY_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_ITEM_RECOVERY_LIST);
    Response->Count = Character->Data.RecoveryInfo.Info.SlotCount;

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

    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(RECOVER_ITEM) {
    if (!Character) goto error;

    if (Packet->RecoverySlotIndex >= RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT) goto error;

    RTItemSlotRef RecoverySlot = &Character->Data.RecoveryInfo.Slots[Packet->RecoverySlotIndex];
    if (!RecoverySlot->Item.Serial) goto error;

    UInt64 RecoveryPrice = Character->Data.RecoveryInfo.Prices[Packet->RecoverySlotIndex];
    if (Character->Data.Info.Alz< RecoveryPrice) goto error;

    RecoverySlot->SlotIndex = Packet->InventorySlotIndex;

    if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, RecoverySlot)) goto error;

    memset(RecoverySlot, 0, sizeof(struct _RTItemSlot));
    Character->Data.RecoveryInfo.Info.SlotCount -= 1;

    RTCharacterUpdateQuestItemCounter(Runtime, Character, RecoverySlot->Item, RecoverySlot->ItemOptions);

    Character->Data.Info.Alz-= RecoveryPrice;
    Character->SyncMask.Info = true;
    Character->SyncMask.InventoryInfo = true;

    S2C_DATA_RECOVER_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, RECOVER_ITEM);
    Response->Result = 1; // TODO: Check Result types and remove disconnects
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
