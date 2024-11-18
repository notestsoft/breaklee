#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ADD_OVERLORD_MASTERY_SLOT) {
    if (!Character) goto error;

	if (Character->Data.OverlordMasteryInfo.Info.Level < 1) goto error;

	RTDataOverlordMasteryBaseRef Mastery = RTRuntimeDataOverlordMasteryBaseGet(Runtime->Context, Packet->MasteryIndex);
	if (!Mastery) goto error;
 
	if (Character->Data.OverlordMasteryInfo.Info.Level < Mastery->RequiredLevel) goto error;

	if (Mastery->PreMasteryIndex > 0) {
		RTDataOverlordMasteryBaseRef PreMastery = RTRuntimeDataOverlordMasteryBaseGet(Runtime->Context, Mastery->PreMasteryIndex);
		if (!PreMastery) goto error;

		RTOverlordMasterySlotRef Slot = RTCharacterGetOverlordMasterySlot(Runtime, Character, Mastery->PreMasteryIndex);
		if (!Slot) goto error;
		if (Slot->Level != PreMastery->MaxLevel) goto error;
	}
  
	RTOverlordMasterySlotRef Slot = RTCharacterGetOverlordMasterySlot(Runtime, Character, Packet->MasteryIndex);
	if (!Slot) {
		Slot = &Character->Data.OverlordMasteryInfo.Slots[Character->Data.OverlordMasteryInfo.Info.SlotCount];
		memset(Slot, 0, sizeof(struct _RTOverlordMasterySlot));
		Slot->MasteryIndex = Mastery->MasteryIndex;
		Character->Data.OverlordMasteryInfo.Info.SlotCount += 1;
	}
 
    RTDataOverlordMasteryValueRef MasteryValue = RTRuntimeDataOverlordMasteryValueGet(Runtime->Context, Packet->MasteryIndex);
    if (!MasteryValue) goto error;
    
    RTDataOverlordMasteryValueLevelRef MasteryValueLevel = RTRuntimeDataOverlordMasteryValueLevelGet(MasteryValue, Slot->Level + 1);
    if (!MasteryValueLevel) goto error;
    
	if (Character->Data.OverlordMasteryInfo.Info.Point < MasteryValueLevel->RequiredMasteryPointCount) goto error;

	Slot->Level += 1;
	Character->Data.OverlordMasteryInfo.Info.Point -= MasteryValueLevel->RequiredMasteryPointCount;
	Character->SyncMask.OverlordMasteryInfo = true;

	S2C_DATA_ADD_OVERLORD_MASTERY_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ADD_OVERLORD_MASTERY_SLOT);
	Response->MasteryIndex = Slot->MasteryIndex;
	Response->Level = Slot->Level;
    SocketSend(Socket, Connection, Response);
	return;

error:
    SocketDisconnect(Socket, Connection);
}
