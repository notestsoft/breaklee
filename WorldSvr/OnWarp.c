#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(WARP) {	
    if (!Character) goto error;

    Bool Success = RTRuntimeWarpCharacter(
        Runtime,
        Character->ID,
        Packet->NpcID,
        Packet->WarpPositionX,
        Packet->WarpPositionY,
        Packet->WarpWorldID,
        Packet->WarpIndex,
        Packet->SlotIndex
    );

    S2C_DATA_WARP* Response = PacketBufferInit(Connection->PacketBuffer, S2C, WARP);
    Response->Result = Success ? 0 : 1;
    Response->AccumulatedExp = Character->Info.Basic.Exp;
    Response->AccumulatedOxp = Character->Info.Overlord.Exp;
    Response->Currency = Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
    Response->Position.X = Character->Info.Position.X;
    Response->Position.Y = Character->Info.Position.Y;
    Response->WorldID = Character->Info.Position.WorldID;
    Response->DungeonID = (UInt32)Character->Info.Position.DungeonIndex;
    SocketSend(Socket, Connection, Response);
    
    RTWorldChunkNotify(Character->Movement.WorldChunk, Character->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP, true);
    return;

error:
	return SocketDisconnect(Socket, Connection);
}
