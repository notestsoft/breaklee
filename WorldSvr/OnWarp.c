#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(WARP) {	
    if (!Character) goto error;

    S2C_DATA_WARP* Response = PacketInit(S2C_DATA_WARP);
    Response->Command = S2C_WARP;
    Response->Result = 1;

    if (RTRuntimeWarpCharacter(Runtime, Character->ID, Packet->NpcID, Packet->WarpPositionX, Packet->WarpPositionY, Packet->WarpWorldID, Packet->WarpIndex, Packet->SlotIndex)) {
        // TODO: Move this to runtime
        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
        Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
        Response->Result = 0;
    }

    Response->AccumulatedExp = Character->Info.Basic.Exp;
    Response->AccumulatedOxp = Character->Info.Overlord.Exp;
    Response->Currency = Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
    Response->Position.X = Character->Info.Position.X;
    Response->Position.Y = Character->Info.Position.Y;
    Response->WorldID = Character->Info.Position.WorldID;
    Response->DungeonID = Character->Info.Position.DungeonIndex;
    SocketSend(Socket, Connection, Response);

    if (Response->Result == 0) {
        RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
        RTWorldBroadcastSpawnCharacterNotification(
            Runtime,
            WorldContext,
            Character->ID
        );
    }

    return;

error:
	return SocketDisconnect(Socket, Connection);
}
