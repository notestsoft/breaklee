#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(REGISTER_MERCENARY_CARD) {
    if (!Character) goto error;

    S2C_DATA_REGISTER_MERCENARY_CARD* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REGISTER_MERCENARY_CARD);
    Response->Result = S2C_REGISTER_MERCENARY_CARD_RESULT_ERROR;

    if (RTCharacterRegisterMercenaryCard(Runtime, Character, Packet->InventorySlotIndex)) {
        Response->Result = S2C_REGISTER_MERCENARY_CARD_RESULT_SUCCESS;
    }

    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
