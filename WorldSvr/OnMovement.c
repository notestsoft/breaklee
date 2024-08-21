#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

CLIENT_PROCEDURE_BINDING(MOVEMENT_BEGIN) {
	if (!Character) goto error;
	
    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementBegin(
		Runtime,
		Character,
		Packet->WorldID,
		Packet->PositionBegin.X,
		Packet->PositionBegin.Y,
		Packet->PositionCurrent.X,
		Packet->PositionCurrent.Y,
		Packet->PositionEnd.X,
		Packet->PositionEnd.Y
	);
	// TODO: Movement system is still not in sync with client!
	//if (!Success) goto error;

	S2C_DATA_MOVEMENT_BEGIN* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, MOVEMENT_BEGIN);
	Notification->CharacterIndex = (UInt32)Client->CharacterIndex;
	Notification->TickCount = Character->Movement.TickCount;
	Notification->PositionBegin.X = Packet->PositionBegin.X;
	Notification->PositionBegin.Y = Packet->PositionBegin.Y;
	Notification->PositionEnd.X = Packet->PositionEnd.X;
	Notification->PositionEnd.Y = Packet->PositionEnd.Y;

	BroadcastToWorld(
		Context,
		World,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MOVEMENT_END) {
	if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementEnd(
		Runtime,
		Character,
		Packet->Position.X,
		Packet->Position.Y
	);
	// TODO: Movement system is still not in sync with client!
	//if (!Success) goto error;

	S2C_DATA_MOVEMENT_END* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, MOVEMENT_END);
	Notification->CharacterIndex = (UInt32)Client->CharacterIndex;
	Notification->Position.X = Packet->Position.X;
	Notification->Position.Y = Packet->Position.Y;

	BroadcastToWorld(
		Context,
		World,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MOVEMENT_CHANGE) {
	if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementChange(
		Runtime,
		Character,
		Packet->WorldID,
		Packet->PositionBegin.X,
		Packet->PositionBegin.Y,
		Packet->PositionCurrent.X,
		Packet->PositionCurrent.Y,
		Packet->PositionEnd.X,
		Packet->PositionEnd.Y
	);
	// TODO: Movement system is still not in sync with client!
	//if (!Success) goto error;

	S2C_DATA_MOVEMENT_CHANGE* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, MOVEMENT_CHANGE);
	Notification->CharacterIndex = (UInt32)Client->CharacterIndex;
	Notification->TickCount = Character->Movement.TickCount;
	Notification->PositionBegin.X = Packet->PositionBegin.X;
	Notification->PositionBegin.Y = Packet->PositionBegin.Y;
	Notification->PositionEnd.X = Packet->PositionEnd.X;
	Notification->PositionEnd.Y = Packet->PositionEnd.Y;

	BroadcastToWorld(
		Context,
		World,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MOVEMENT_WAYPOINTS) {
	if (!Character) goto error;
	Bool Success = RTCharacterMovementChangeWaypoints(
		Runtime,
		Character,
		Packet->PositionCurrent.X,
		Packet->PositionCurrent.Y,
		Packet->PositionNext.X,
		Packet->PositionNext.Y
	);
	// TODO: Movement system is still not in sync with client!
	//if (!Success) goto error;

	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MOVEMENT_TILE_POSITION) {
	if (!Character) goto error;

	Bool Success = RTCharacterMovementChangeChunk(
		Runtime,
		Character,
		Packet->PositionNext.X,
		Packet->PositionNext.Y
	);
	// TODO: Movement system is still not in sync with client!
	//if (!Success) goto error;

	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CHANGE_DIRECTION) {
	if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	S2C_DATA_CHANGE_DIRECTION* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, CHANGE_DIRECTION);
	Notification->CharacterIndex = (UInt32)Client->CharacterIndex;
	Notification->Direction = Packet->Direction;

	BroadcastToWorld(
		Context,
		World,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(KEY_MOVEMENT_BEGIN) {
	if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementBegin(
		Runtime,
		Character,
		Character->Data.Info.WorldIndex,
		(Int32)Packet->PositionStartX,
		(Int32)Packet->PositionStartY,
		(Int32)Packet->PositionCurrentX,
		(Int32)Packet->PositionCurrentY,
		(Int32)Packet->PositionEndX,
		(Int32)Packet->PositionEndY
	);
	// TODO: Movement system is still not in sync with client!
	//if (!Success) goto error;

	S2C_DATA_KEY_MOVEMENT_BEGIN* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, KEY_MOVEMENT_BEGIN);
	Notification->CharacterIndex = (UInt32)Client->CharacterIndex;
	Notification->TickCount = Character->Movement.TickCount;
	Notification->PositionStartX = Packet->PositionStartX;
	Notification->PositionStartY = Packet->PositionStartY;
	Notification->PositionEndX = Packet->PositionEndX;
	Notification->PositionEndY = Packet->PositionEndY;
	Notification->Direction = Packet->Direction;
	BroadcastToWorld(
		Context,
		World,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(KEY_MOVEMENT_END) {
	if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementEnd(
		Runtime,
		Character,
		(Int32)Packet->PositionCurrentX,
		(Int32)Packet->PositionCurrentY
	);
	// TODO: Movement system is still not in sync with client!
	//if (!Success) goto error;

	S2C_DATA_KEY_MOVEMENT_END* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, KEY_MOVEMENT_END);
	Notification->CharacterIndex = (UInt32)Client->CharacterIndex;
	Notification->PositionCurrentX = Packet->PositionCurrentX;
	Notification->PositionCurrentY = Packet->PositionCurrentY;
	BroadcastToWorld(
		Context,
		World,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(KEY_MOVEMENT_CHANGE) {
	if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementChange(
		Runtime,
		Character,
		Character->Data.Info.WorldIndex,
		(Int32)Packet->PositionStartX,
		(Int32)Packet->PositionStartY,
		(Int32)Packet->PositionCurrentX,
		(Int32)Packet->PositionCurrentY,
		(Int32)Packet->PositionEndX,
		(Int32)Packet->PositionEndY
	);
	// TODO: Movement system is still not in sync with client!
	//if (!Success) goto error;

	S2C_DATA_KEY_MOVEMENT_CHANGE* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, KEY_MOVEMENT_CHANGE);
	Notification->CharacterIndex = (UInt32)Client->CharacterIndex;
	Notification->TickCount = Character->Movement.TickCount;
	Notification->PositionStartX = Packet->PositionStartX;
	Notification->PositionStartY = Packet->PositionStartY;
	Notification->PositionEndX = Packet->PositionEndX;
	Notification->PositionEndY = Packet->PositionEndY;
	Notification->Direction = Packet->Direction;
	BroadcastToWorld(
		Context,
		World,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
