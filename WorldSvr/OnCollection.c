#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

RTDataCollectionKindInfoRef RTRuntimeDataCollectionKindInfoGet(RTDataCollectionKindRef Parent, Int32 Key) {
    for (Int32 Index = 0; Index < Parent->CollectionKindInfoCount; Index += 1) {
        RTDataCollectionKindInfoRef Child = &Parent->CollectionKindInfoList[Index];
        if (Child->ID == Key) return Child;
    }

    return NULL;
}

RTDataCollectionKindInfoDetailRef RTRuntimeDataCollectionKindInfoDetailGet(RTDataCollectionKindInfoRef Parent, Int32 Key) {
    for (Int32 Index = 0; Index < Parent->CollectionKindInfoDetailCount; Index += 1) {
        RTDataCollectionKindInfoDetailRef Child = &Parent->CollectionKindInfoDetailList[Index];
        if (Child->MissionID == Key) return Child;
    }

    return NULL;
}

RTDataCollectionMissionInfoRef RTRuntimeDataCollectionMissionInfoGet(RTDataCollectionMissionRef Parent, Int32 CollectionID, Int32 MissionID) {
    for (Int32 Index = 0; Index < Parent->CollectionMissionInfoCount; Index += 1) {
        RTDataCollectionMissionInfoRef Child = &Parent->CollectionMissionInfoList[Index];
        if (Child->ID == CollectionID && Child->MissionID == MissionID) return Child;
    }

    return NULL;
}

RTDataCollectionMissionDetailRef RTRuntimeDataCollectionMissionInfoDetailGet(RTDataCollectionMissionInfoRef Parent, Int32 Key) {
    for (Int32 Index = 0; Index < Parent->CollectionMissionDetailCount; Index += 1) {
        RTDataCollectionMissionDetailRef Child = &Parent->CollectionMissionDetailList[Index];
        if (Child->ID == Key) return Child;
    }

    return NULL;
}


CLIENT_PROCEDURE_BINDING(REGISTER_COLLECTION_ITEM) {
	if (!Character) goto error;

    RTDataCollectionQuestRef CollectionQuest = RTRuntimeDataCollectionQuestGet(Runtime->Context);
    assert(CollectionQuest);

    if (!RTCharacterQuestFlagIsSet(Character, CollectionQuest->QuestID)) goto error;

    RTCollectionSlotRef CharacterCollectionSlot = RTCharacterGetCollectionSlot(
        Runtime,
        Character,
        Packet->TypeID,
        Packet->CollectionID,
        Packet->MissionID
    );
    assert(CharacterCollectionSlot);

    S2C_DATA_REGISTER_COLLECTION_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, REGISTER_COLLECTION_ITEM);
    Response->TypeID = Packet->TypeID;
    Response->Unknown1 = Packet->Unknown1;
    Response->CollectionID = Packet->CollectionID;
    Response->MissionID = Packet->MissionID;
    Response->MissionSlotIndex = Packet->MissionSlotIndex;

    RTDataCollectionMissionRef CollectionMission = RTRuntimeDataCollectionMissionGet(Runtime->Context, Packet->TypeID);
    if (!CollectionMission) goto error;

    RTDataCollectionMissionInfoRef CollectionMissionInfo = RTRuntimeDataCollectionMissionInfoGet(
        CollectionMission,
        Packet->CollectionID, 
        Packet->MissionID
    );
    if (!CollectionMissionInfo) goto error;

    RTDataCollectionMissionDetailRef CollectionMissionDetail = RTRuntimeDataCollectionMissionInfoDetailGet(
        CollectionMissionInfo,
        Packet->MissionSlotIndex
    );
    if (!CollectionMissionDetail) goto error;

    if (CollectionMissionDetail->ItemType == RUNTIME_DATA_COLLECTION_ITEM_TYPE_INDEX_ITEM) {
        RTDataCollectionMissionItemRef CollectionMissionItem = RTRuntimeDataCollectionMissionItemGet(
            Runtime->Context,
            CollectionMissionDetail->ItemID
        );

        for (Int32 Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
            if (!ItemSlot) goto error;

            RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
            assert(ItemData);

            Bool IsStackable = ItemData->MaxStackSize > 0;
            Bool Found = false;
            for (Int32 DetailIndex = 0; DetailIndex < CollectionMissionItem->CollectionMissionItemDetailCount; DetailIndex += 1) {
                RTDataCollectionMissionItemDetailRef CollectionMissionItemDetail = &CollectionMissionItem->CollectionMissionItemDetailList[0];

                if (CollectionMissionItemDetail->ItemIndex != ItemSlot->Item.ID) continue;
                if (!IsStackable && CollectionMissionItemDetail->ItemOption != ItemSlot->ItemOptions) continue;

                // TODO: ItemKind is currently not mapped in the data!
                // if (CollectionMissionItemDetail->ItemKind != ItemData->ItemType) continue;

                Found = true;
                break;
            }

            if (!Found) goto error;

            Int32 RegisteredAmount = 1;
            if (IsStackable) {
                Int32 RequiredAmount = MAX(0, 
                    CollectionMissionDetail->ItemCount - CharacterCollectionSlot->MissionItemCounts[Packet->MissionSlotIndex]
                );
                Int32 RemainingAmount = MAX(0,
                    (Int32)ItemSlot->ItemOptions - RequiredAmount
                );

                RegisteredAmount = MIN((Int32)ItemSlot->ItemOptions, RequiredAmount);
                ItemSlot->ItemOptions = RemainingAmount;
            }

            CharacterCollectionSlot->MissionItemCounts[Packet->MissionSlotIndex] = MIN(
                CharacterCollectionSlot->MissionItemCounts[Packet->MissionSlotIndex] + RegisteredAmount,
                CollectionMissionDetail->ItemCount
            );

            if (!IsStackable || ItemSlot->ItemOptions < 1) {
                if (!RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index])) goto error;
            }
        }

        Character->SyncMask.CollectionInfo = true;
        Character->SyncMask.InventoryInfo = true;
        Response->Success = 1;
    }
    else if (CollectionMissionDetail->ItemType == RUNTIME_DATA_COLLECTION_ITEM_TYPE_INDEX_UPGRADE) {
        RTDataCollectionMissionUpgradeRef CollectionMissionUpgrade = RTRuntimeDataCollectionMissionUpgradeGet(
            Runtime->Context,
            CollectionMissionDetail->ItemID
        );

        for (Int32 Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
            if (!ItemSlot) goto error;

            RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
            assert(ItemData);

            Bool Found = false;
            for (Int32 DetailIndex = 0; DetailIndex < CollectionMissionUpgrade->CollectionMissionUpgradeDetailCount; DetailIndex += 1) {
                RTDataCollectionMissionUpgradeDetailRef CollectionMissionUpgradeDetail = &CollectionMissionUpgrade->CollectionMissionUpgradeDetailList[DetailIndex];

                RTItem MissionItem = { 0 };
                MissionItem.ID = CollectionMissionUpgradeDetail->ItemIndex;
                MissionItem.UpgradeLevel = CollectionMissionUpgradeDetail->ItemGrade;

                if (MissionItem.ID != ItemSlot->Item.ID) continue;

                Found = true;
                break;
            }

            if (!Found) goto error;

            Int32 RegisteredAmount = 1;
            CharacterCollectionSlot->MissionItemCounts[Packet->MissionSlotIndex] = MIN(
                CharacterCollectionSlot->MissionItemCounts[Packet->MissionSlotIndex] + RegisteredAmount,
                CollectionMissionDetail->ItemCount
            );

            if (!RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index])) goto error;
        }

        Character->SyncMask.CollectionInfo = true;
        Character->SyncMask.InventoryInfo = true;
        Response->Success = 1;
    }
    else if (CollectionMissionDetail->ItemType == RUNTIME_DATA_COLLECTION_ITEM_TYPE_INDEX_QUEST) {
        RTDataCollectionMissionQuestRef CollectionMissionQuest = RTRuntimeDataCollectionMissionQuestGet(
            Runtime->Context,
            CollectionMissionDetail->ItemID
        );

        for (Int32 Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
            if (!ItemSlot) goto error;

            RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
            assert(ItemData);

            Bool IsStackable = ItemData->MaxStackSize > 0;
            
            assert(CollectionMissionQuest->CollectionMissionQuestDetailCount == 1);

            RTDataCollectionMissionQuestDetailRef CollectionMissionItemDetail = &CollectionMissionQuest->CollectionMissionQuestDetailList[0];
            if (CollectionMissionItemDetail->ItemIndex != ItemSlot->Item.ID) goto error;
            if (!IsStackable) goto error;

            Int32 RegisteredAmount = 1;
            if (IsStackable) {
                Int32 RequiredAmount = MAX(0,
                    CollectionMissionDetail->ItemCount - CharacterCollectionSlot->MissionItemCounts[Packet->MissionSlotIndex]
                );
                Int32 RemainingAmount = MAX(0,
                    (Int32)ItemSlot->ItemOptions - RequiredAmount
                );

                RegisteredAmount = MIN((Int32)ItemSlot->ItemOptions, RequiredAmount);
                ItemSlot->ItemOptions = RemainingAmount;
            }

            CharacterCollectionSlot->MissionItemCounts[Packet->MissionSlotIndex] = MIN(
                CharacterCollectionSlot->MissionItemCounts[Packet->MissionSlotIndex] + RegisteredAmount,
                CollectionMissionDetail->ItemCount
            );

            if (!IsStackable || ItemSlot->ItemOptions < 1) {
                if (!RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index])) goto error;
            }
        }

        Character->SyncMask.CollectionInfo = true;
        Character->SyncMask.InventoryInfo = true;
        Response->Success = 1;
    }
    else {
        goto error;
    }

    for (Int32 Index = 0; Index < RUNTIME_CHARACTER_MAX_COLLECTION_ITEM_COUNT; Index += 1) {
        Response->MissionItemCounts[Index] = CharacterCollectionSlot->MissionItemCounts[Index];
    }

    return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(RECEIVE_COLLECTION_REWARD) {
    S2C_DATA_RECEIVE_COLLECTION_REWARD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, RECEIVE_COLLECTION_REWARD);

    // TODO: Check packet bounds based on InventorySlotCount...

    if (!Character) goto error;

    RTCollectionSlotRef CollectionSlot = RTCharacterGetCollectionSlot(Runtime, Character, Packet->TypeID, Packet->CollectionID, Packet->MissionID);
    if (!CollectionSlot) goto error;

    if (CollectionSlot->ReceivedReward) goto error;

    RTDataCollectionKindRef CollectionKind = RTRuntimeDataCollectionKindGet(Runtime->Context, Packet->TypeID);
    if (!CollectionKind) goto error;

    RTDataCollectionKindInfoRef CollectionKindInfo = RTRuntimeDataCollectionKindInfoGet(CollectionKind, Packet->CollectionID);
    if (!CollectionKindInfo) goto error;

    RTDataCollectionKindInfoDetailRef CollectionKindInfoDetail = RTRuntimeDataCollectionKindInfoDetailGet(CollectionKindInfo, Packet->MissionID);
    if (!CollectionKindInfoDetail) goto error;

    if (CollectionKindInfoDetail->RewardType == RUNTIME_DATA_COLLECTION_REWARD_TYPE_CURRENCY) {
        RTDataCollectionRewardCurrencyRef Reward = RTRuntimeDataCollectionRewardCurrencyGet(Runtime->Context, CollectionKindInfoDetail->RewardID);

        CollectionSlot->ReceivedReward = true;
        Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_GEM] += Reward->Quantity;
        Character->SyncMask.Info = true;
        Character->SyncMask.CollectionInfo = true;

        Response->RewardType = CollectionKindInfoDetail->RewardType;
        S2C_DATA_RECEIVE_COLLECTION_REWARD_CURRENCY* ResponseData = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_RECEIVE_COLLECTION_REWARD_CURRENCY);
        ResponseData->Currency = (UInt32)Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_GEM];
    }
    else if (CollectionKindInfoDetail->RewardType == RUNTIME_DATA_COLLECTION_REWARD_TYPE_ITEM) {
        if (Packet->InventorySlotCount < 1) goto error;

        RTDataCollectionRewardItemRef Reward = RTRuntimeDataCollectionRewardItemGet(Runtime->Context, CollectionKindInfoDetail->RewardID);

        struct _RTItemSlot ItemSlot = { 0 };
        ItemSlot.SlotIndex = Packet->InventorySlotIndex[0];
        ItemSlot.Item.Serial = Reward->ItemKind;
        ItemSlot.ItemOptions = Reward->ItemOption;
        ItemSlot.ItemDuration.Serial = Reward->ItemDuration;
        if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &ItemSlot)) goto error;

        CollectionSlot->ReceivedReward = true;
        Character->SyncMask.CollectionInfo = true;
        Character->SyncMask.InventoryInfo = true;

        Response->RewardType = CollectionKindInfoDetail->RewardType;
        Response->InventorySlotCount = 1;
        S2C_DATA_RECEIVE_COLLECTION_REWARD_ITEM* Slot = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_RECEIVE_COLLECTION_REWARD_ITEM);
        Slot->ItemID = ItemSlot.Item;
        Slot->ItemOptions = ItemSlot.ItemOptions;
        Slot->InventorySlotIndex = ItemSlot.SlotIndex;
        Slot->ItemDuration = ItemSlot.ItemDuration.Serial;
    }
    else {
        goto error;
    }

    Response->Success = 1;
    return SocketSend(Socket, Connection, Response);

error:
    Response->Success = 0;
    return SocketSend(Socket, Connection, Response);
}
