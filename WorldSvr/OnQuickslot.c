#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SET_QUICKSLOT) {
    if (!Character) goto error;

    Bool Success = false;

    if (Packet->SkillSlotIndex < UINT16_MAX) {
        Success |= RTCharacterAddQuickSlot(Runtime, Character, Packet->SkillSlotIndex, Packet->QuickSlotIndex);
    }
    else {
        Success |= RTCharacterRemoveQuickSlot(Runtime, Character, Packet->QuickSlotIndex);
    }
    
    S2C_DATA_SET_QUICKSLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, SET_QUICKSLOT);
    Response->Success = Success ? 1 : 0;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SWAP_QUICKSLOT) {
    if (!Character) goto error;

    Bool Success = RTCharacterSwapQuickSlot(Runtime, Character, Packet->SourceSlotIndex, Packet->TargetSlotIndex);

    S2C_DATA_SET_QUICKSLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, SET_QUICKSLOT);
    Response->Success = Success ? 1 : 0;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
