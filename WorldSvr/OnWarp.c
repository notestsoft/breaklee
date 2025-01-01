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

    S2C_DATA_WARP* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, WARP);
    Response->Position.X = Character->Data.Info.PositionX;
    Response->Position.Y = Character->Data.Info.PositionY;
    Response->AccumulatedExp = Character->Data.Info.Exp;
    Response->AccumulatedOxp = Character->Data.OverlordMasteryInfo.Info.Exp;
    Response->ReceivedSkillExp = 0;
    Response->Currency = Character->Data.Info.Alz;
    Response->Entity = Character->ID;
    Response->WorldType = 0;
    Response->WorldID = Character->Data.Info.WorldIndex;
    Response->DungeonID = (UInt32)Character->Data.Info.DungeonIndex;
    Response->Result = Success ? 0 : 1;
    SocketSend(Socket, Connection, Response);
    
    RTWorldChunkNotify(Character->Movement.WorldChunk, Character->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP, true);
    return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_SERVER_POSITION) {
    if (!Character) goto error;

    S2C_DATA_GET_SERVER_POSITION* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GET_SERVER_POSITION);
    Response->PositionCurrentX = Character->Data.Info.PositionX;
    Response->PositionCurrentY = Character->Data.Info.PositionY;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
