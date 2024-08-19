#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// NOTE: Currently we use this shared memory because we don't do parallel processing...
static struct _RTCharacterInventoryInfo kInventoryInfoBackup;
static struct _RTCharacterStellarMasteryInfo kStellarMasteryInfoBackup;

CLIENT_PROCEDURE_BINDING(STELLAR_LINK_IMPRINT_SLOT) {
	memcpy(&kInventoryInfoBackup, &Character->Data.InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&kStellarMasteryInfoBackup, &Character->Data.StellarMasteryInfo, sizeof(struct _RTCharacterStellarMasteryInfo));

	S2C_DATA_STELLAR_LINK_IMPRINT_SLOT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, STELLAR_LINK_IMPRINT_SLOT);

	Info("STELLAR_LINK_IMPRINT_SLOT %d %d %d %d %d %d", Packet->GroupID, Packet->SlotLine, Packet->SlotIndex, Packet->MaterialCount, Packet->MaterialSlotCount1, Packet->MaterialSlotCount2);

	// TODO: send error codes instead of disconnecting

	RTDataStellarGroupRef StellarGroup = RTRuntimeDataStellarGroupGet(Runtime->Context, Packet->GroupID);
	if (StellarGroup == NULL) {
		Error("[STELLAR_LINK_IMPRINT_SLOT]: Incorrect GroupID %d", Packet->GroupID);
		goto error;
	}

	RTDataStellarLineRef StellarLine = RTRuntimeDataStellarLineGet(StellarGroup, Packet->SlotLine);
	if (StellarLine == NULL) {
		Error("[STELLAR_LINK_IMPRINT_SLOT]: Incorrect SlotLine %d", Packet->SlotLine);
		goto error;
	}

	RTDataStellarLineGradeRef StellarLineGrade = RTDataStellarMasteryGetLineGrade(
		Runtime,
		StellarLine,
		Packet->MaterialCount
	);
	if (StellarLineGrade == NULL) {
		Error("[STELLAR_LINK_IMPRINT_SLOT]: Incorrect MaterialCount %d", Packet->MaterialCount);
		goto error;
	}

	if (
		!RTInventoryCanConsumeStackableItems(
			Runtime,
			&Character->Data.InventoryInfo,
			StellarLineGrade->ItemID,
			StellarLineGrade->ItemCount,
			Packet->MaterialSlotCount1,
			Packet->MaterialSlotIndex
		)
	) {
		Error("[STELLAR_LINK_IMPRINT_SLOT]: Not enough materials");
		goto error;
	}

	// TODO: Add slot index validation

	// TODO: Add check of MaterialSlotCount2

	RTInventoryConsumeStackableItems(
		Runtime,
		&Character->Data.InventoryInfo,
		StellarLineGrade->ItemID,
		StellarLineGrade->ItemCount,
		Packet->MaterialSlotCount1,
		Packet->MaterialSlotIndex
	);

	// TODO: Add usage of MaterialSlotCount2

	Character->SyncMask.InventoryInfo = true;

	// TODO: Randomize slot values
	RTStellarMasterySlotRef MasterySlot = &(struct _RTStellarMasterySlot) {
		.GroupID = StellarGroup->GroupID,
		.SlotLine = StellarLine->LineID,
		.SlotIndex = Packet->SlotIndex,
		.StellarLinkGrade = RUNTIME_STELLAR_MASTERY_SLOT_LINK_GRADE_WRATH,
		.StellarForceEffect = RUNTIME_FORCE_EFFECT_ATTACK_UP,
		.StellarForceValue = 20,
		.StellarForceValueType = RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
	};


	Bool SetSlotResult = RTCharacterStellarMasterySetSlot(
		Character,
		MasterySlot
	);
	if (!SetSlotResult) {
		Error("[STELLAR_LINK_IMPRINT_SLOT]: Failed to set slot");
		goto error;
	}
	Character->SyncMask.StellarMasteryInfo = true;

	Response->GroupID = StellarGroup->GroupID;
	Response->SlotLine = Packet->SlotLine;
	Response->SlotIndex = Packet->SlotIndex;
	Response->StellarLinkGrade = MasterySlot->StellarLinkGrade;
	Response->StellarForceEffect = MasterySlot->StellarForceEffect;
	Response->StellarForceValue = 20;
	Response->StellarForceValueType = RUNTIME_FORCE_VALUE_TYPE_ADDITIVE;
	Response->ErrorCode = 0;

	SocketSend(Socket, Connection, Response);
	
	return;

error:
	memcpy(&Character->Data.InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Character->Data.StellarMasteryInfo, &kStellarMasteryInfoBackup, sizeof(struct _RTCharacterStellarMasteryInfo));
	SocketDisconnect(Socket, Connection);
}