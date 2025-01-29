#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(MYTH_ROLL_SLOT) {
	if (!Character || Character->Data.MythMasteryInfo.Info.Level < 1) goto error;

	Int32 Seed = (Int32)PlatformGetTickCount();                       
	Int32 RandomValue = RandomRange(&Seed, 1, 5);

	S2C_DATA_MYTH_ROLL_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MYTH_ROLL_SLOT);
	Response->MythPoints = Character->Data.MythMasteryInfo.Info.Points;
	Response->HolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
	Response->MasteryIndex = Packet->MasteryIndex;
	Response->SlotIndex = Packet->SlotIndex;
	Response->TierIndex = 53;
	Response->TierLevel = 5;
	Response->StatOption = 180;
	Response->StatValue = RandomValue;
	Response->ValueType = 1;
	Response->StigmaGrade = 1;
	Response->StigmaXP = 500;
	Response->ErrorCode = 0;

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MYTH_REROLL_SLOT) {
	if (!Character || Character->Data.MythMasteryInfo.Info.Level < 1) goto error;

	Info("Myth rollback slot");

	S2C_DATA_MYTH_REROLL_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MYTH_REROLL_SLOT);
	Response->MasteryIndex = Packet->MasteryIndex;
	Response->SlotIndex = Packet->SlotIndex;
	Response->TierIndex = 53;
	if (Packet->bAcceptTransform == true)
		Response->TierLevel = 5;
	else 
		Response->TierLevel = 3;
	Response->StatOption = 180;
	if (Packet->bAcceptTransform == true)
		Response->StatValue = 5;
	else
		Response->StatValue = 3;
	Response->ValueType = 1;
	Response->HolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
	Response->ErrorCode = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MYTH_OPEN_LOCK) {
	if (!Character || Character->Data.MythMasteryInfo.Info.Level < 1) goto error;

	RTDataMythLockPageRef MythLockPageRef = RTRuntimeDataMythLockPageGet(Runtime->Context, Packet->MasteryIndex);
	RTDataMythLockInfoRef MythLockInfoRef = RTRuntimeDataMythLockInfoGet(MythLockPageRef, Packet->LockGroupIndex);

	S2C_DATA_MYTH_OPEN_LOCK* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MYTH_OPEN_LOCK);

	if (RTCharacterMythMasteryGetCanOpenLockGroup(Runtime, Character, Packet->MasteryIndex, Packet->LockGroupIndex)) {
		Response->MasteryIndex = Packet->MasteryIndex;
		Response->LockGroupIndex = MythLockInfoRef->LockGroup;
		Response->ErrorCode = 0;

		Character->Data.MythMasteryInfo.Info.UnlockedPageCount = MythLockInfoRef->LockGroup;
		Character->Data.MythMasteryInfo.Info.HolyPower += MythLockInfoRef->BonusScore;
		Character->SyncMask.MythMasteryInfo = true;
	}
	else {
		Response->MasteryIndex = 0;
		Response->LockGroupIndex = 0;
		Response->ErrorCode = 1; //gives random error on client but fine for now
	}
	
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}