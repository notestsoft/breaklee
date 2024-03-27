#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SKILL_TO_ACTION) {
    if (!Character) goto error;
    if (!RTCharacterIsAlive(Runtime, Character)) goto error;

    // TODO: Adjust character position based on casted social skill

    S2C_DATA_SKILL_TO_ACTION* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, SKILL_TO_ACTION);
    Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
    Notification->TargetIndex = Packet->TargetIndex;
    Notification->ActionIndex = Packet->ActionIndex;
    Notification->X = Packet->X;
    Notification->Y = Packet->Y;

    return BroadcastToWorld(
        Context,
        RTRuntimeGetWorldByCharacter(Runtime, Character),
        kEntityIDNull,
        Notification->X,
        Notification->Y,
        Notification
    );

error:
    return SocketDisconnect(Socket, Connection);
}
