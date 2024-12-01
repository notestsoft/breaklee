#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

Void SendGiftBoxPricePoolList(
    ServerContextRef Context,
    ClientContextRef Client
) {
    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);
    S2C_DATA_GIFTBOX_PRICE_POOL_LIST* Notification = PacketBufferInit(PacketBuffer, S2C, GIFTBOX_PRICE_POOL_LIST);
    Notification->Count = Context->Runtime->Context->GiftBoxPricePoolCount;

    for (Int PoolIndex = 0; PoolIndex < Context->Runtime->Context->GiftBoxPricePoolCount; PoolIndex += 1) {
        RTDataGiftBoxPricePoolRef PricePool = &Context->Runtime->Context->GiftBoxPricePoolList[PoolIndex];

        S2C_DATA_GIFTBOX_PRICE_POOL* NotificationPool = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_GIFTBOX_PRICE_POOL);
        NotificationPool->SlotIndex = PricePool->Index;
        NotificationPool->PriceCount = PricePool->GiftBoxPricePoolValueCount;

        for (Int PriceIndex = 0; PriceIndex < PricePool->GiftBoxPricePoolValueCount; PriceIndex += 1) {
            PacketBufferAppendValue(PacketBuffer, Int32, PricePool->GiftBoxPricePoolValueList[PriceIndex].Price);
        }
    }

    SocketSend(Context->ClientSocket, Client->Connection, Notification);
}

CLIENT_PROCEDURE_BINDING(OPEN_GIFTBOX_ROLL) {
    if (!Character) goto error;

    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
    S2C_DATA_OPEN_GIFTBOX_ROLL* Response = PacketBufferInit(PacketBuffer, S2C, OPEN_GIFTBOX_ROLL);
    if (RTCharacterRollGiftBox(Runtime, Character, Packet->SlotIndex)) {
        Response->Count = 1;
        
        S2C_DATA_OPEN_GIFTBOX_ROLL_SLOT* ResponseSlot = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_OPEN_GIFTBOX_ROLL_SLOT);
        ResponseSlot->SlotIndex = Packet->SlotIndex;
        ResponseSlot->ItemID = Character->Data.GiftboxInfo.RewardSlots[Packet->SlotIndex].ItemID.Serial;
        ResponseSlot->ItemOptions = Character->Data.GiftboxInfo.RewardSlots[Packet->SlotIndex].ItemOptions.Serial;
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(OPEN_GIFTBOX_RECEIVE) {
    if (!Character) goto error;
    
    Int32 PacketLength = sizeof(C2S_DATA_OPEN_GIFTBOX_RECEIVE) + Packet->Count * sizeof(C2S_DATA_OPEN_GIFTBOX_RECEIVE_SLOT);
    if (Packet->Length != PacketLength) goto error;

    // TODO: Add support for multiple reward items
    if (Packet->Count != 1) goto error;

    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);

    S2C_DATA_OPEN_GIFTBOX_RECEIVE* Response = PacketBufferInit(PacketBuffer, S2C, OPEN_GIFTBOX_RECEIVE);
    if (RTCharacterReceiveGiftBox(Runtime, Character, Packet->SlotIndex, Packet->Data[0].InventorySlotIndex, Packet->Data[0].StackSize)) {
        Response->Count = 1;

        S2C_DATA_OPEN_GIFTBOX_RECEIVE_SLOT* ResponseSlot = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_OPEN_GIFTBOX_RECEIVE_SLOT);
        ResponseSlot->SlotIndex = Packet->SlotIndex;

        RTGiftBoxSlotRef GiftBoxSlot = RTCharacterGetGiftBox(Runtime, Character, Packet->SlotIndex);
        if (GiftBoxSlot) {
            ResponseSlot->ElapsedTimeInSeconds = GiftBoxSlot->ElapsedTime;
            ResponseSlot->TotalTimeInSeconds = GiftBoxSlot->CooldownTime;
        }

        RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->Data[0].InventorySlotIndex);
        if (ItemSlot) {
            S2C_DATA_OPEN_GIFTBOX_RECEIVE_ITEMSLOT* ResponseItemSlot = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_OPEN_GIFTBOX_RECEIVE_ITEMSLOT);
            ResponseSlot->ItemCount = 1;
            ResponseItemSlot->InventorySlotIndex = ItemSlot->SlotIndex;
            ResponseItemSlot->ItemID = ItemSlot->Item.Serial;
            ResponseItemSlot->ItemOptions = ItemSlot->ItemOptions;
            ResponseItemSlot->ItemDuration = ItemSlot->ItemDuration.Serial;
        }
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
