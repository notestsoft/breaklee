#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(OPEN_GIFTBOX_ROLL) {
    if (!Character) goto error;

	PacketLogBytes(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Packet
    );
    
    S2C_DATA_OPEN_GIFTBOX_ROLL* Response = PacketBufferInit(Connection->PacketBuffer, S2C, OPEN_GIFTBOX_ROLL);
    Response->Count = 0;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(OPEN_GIFTBOX_RECEIVE) {
    if (!Character) goto error;

    PacketLogBytes(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Packet
    );

    S2C_DATA_OPEN_GIFTBOX_RECEIVE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, OPEN_GIFTBOX_RECEIVE);
    Response->Count = Packet->Count;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}
