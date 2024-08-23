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

	if (Packet->SlotIndex >= 4 + Packet->SlotLine * 2) { // Each next line has 2 more slots, starting from 4
		Error("[STELLAR_LINK_IMPRINT_SLOT]: Incorrect SlotIndex %d", Packet->SlotIndex);
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
	RTInventoryConsumeStackableItems(
		Runtime,
		&Character->Data.InventoryInfo,
		StellarLineGrade->ItemID,
		StellarLineGrade->ItemCount,
		Packet->MaterialSlotCount1,
		Packet->MaterialSlotIndex
	);

	Character->SyncMask.InventoryInfo = true;

	RTStellarMasterySlotRef MasterySlot = RTCharacterStellarMasteryAssertSlot(
		Character,
		StellarGroup->GroupID,
		StellarLine->LineID,
		Packet->SlotIndex
	);

	RTStellarMasteryRollLinkGrade(
		Runtime,
		StellarLineGrade,
		MasterySlot
	);	
	RTStellarMasteryRollForceEffect(
		Runtime,
		StellarLineGrade,
		MasterySlot
	);

	Character->SyncMask.StellarMasteryInfo = true;


	Response->GroupID = StellarGroup->GroupID;
	Response->SlotLine = StellarLine->LineID;
	Response->SlotIndex = Packet->SlotIndex;
	Response->StellarLinkGrade = MasterySlot->LinkGrade;
	Response->StellarForceEffect = MasterySlot->ForceEffect;
	Response->StellarForceValue = MasterySlot->ForceValue;
	Response->StellarForceValueType = MasterySlot->ForceValueType;
	Response->ErrorCode = 0;

	SocketSend(Socket, Connection, Response);
	
	return;

error:
	memcpy(&Character->Data.InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Character->Data.StellarMasteryInfo, &kStellarMasteryInfoBackup, sizeof(struct _RTCharacterStellarMasteryInfo));
	SocketDisconnect(Socket, Connection);
}


CLIENT_PROCEDURE_BINDING(STELLAR_LINK_TRANSFORM_LINK) {
	memcpy(&kInventoryInfoBackup, &Character->Data.InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&kStellarMasteryInfoBackup, &Character->Data.StellarMasteryInfo, sizeof(struct _RTCharacterStellarMasteryInfo));

	S2C_DATA_STELLAR_LINK_IMPRINT_SLOT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, STELLAR_LINK_IMPRINT_SLOT);

	// TODO: send error codes instead of disconnecting

	RTDataStellarTransformationGroupRef StellarTransformationGroup = RTRuntimeDataStellarTransformationGroupGet(Runtime->Context, Packet->GroupID);
	if(StellarTransformationGroup == NULL) {
		Error("[STELLAR_LINK_TRANSFORM_LINK]: Incorrect GroupID %d", Packet->GroupID);
		goto error;
	}

	RTDataStellarLineTransformationRef StellarLineTransformation = RTRuntimeDataStellarLineTransformationGet(StellarTransformationGroup, Packet->SlotLine);
	if(StellarLineTransformation == NULL) {
		Error("[STELLAR_LINK_TRANSFORM_LINK]: Incorrect SlotLine %d", Packet->SlotLine);
		goto error;
	}

	RTDataStellarLineTransformationGradeRef StellarLineTransformationGrade = RTRuntimeDataStellarLineTransformationGradeGet(StellarLineTransformation, Packet->StellarLinkGrade);
	if(StellarLineTransformationGrade == NULL) {
		Error("[STELLAR_LINK_TRANSFORM_LINK]: Incorrect StellarLinkGrade %d", Packet->StellarLinkGrade);
		goto error;
	}

	RTStellarMasterySlotRef MasterySlot = RTCharacterStellarMasteryGetSlot(
		Character,
		StellarTransformationGroup->GroupID,
		StellarLineTransformation->LineID,
		Packet->SlotIndex
	);
	if (MasterySlot == NULL) {
		Error("[STELLAR_LINK_TRANSFORM_LINK]: Incorrect SlotIndex %d", Packet->SlotIndex);
		goto error;
	}


	if (
		!RTInventoryCanConsumeStackableItems(
			Runtime,
			&Character->Data.InventoryInfo,
			StellarLineTransformationGrade->ItemID,
			StellarLineTransformationGrade->ItemCount,
			Packet->MaterialSlotCount1,
			Packet->MaterialSlotIndex
		)
	) {
		Error("[STELLAR_LINK_TRANSFORM_LINK]: Not enough materials");
		goto error;
	}
	RTInventoryConsumeStackableItems(
		Runtime,
		&Character->Data.InventoryInfo,
		StellarLineTransformationGrade->ItemID,
		StellarLineTransformationGrade->ItemCount,
		Packet->MaterialSlotCount1,
		Packet->MaterialSlotIndex
	);

	Character->SyncMask.InventoryInfo = true;


	RTStellarMasterySetLink(
		StellarLineTransformationGrade->Grade,
		MasterySlot
	);

	Character->SyncMask.StellarMasteryInfo = true;


	Response->GroupID = MasterySlot->GroupID;
	Response->SlotLine = MasterySlot->SlotLine;
	Response->SlotIndex = MasterySlot->SlotIndex;
	Response->StellarLinkGrade = MasterySlot->LinkGrade;
	Response->StellarForceEffect = MasterySlot->ForceEffect;
	Response->StellarForceValue = MasterySlot->ForceValue;
	Response->StellarForceValueType = MasterySlot->ForceValueType;
	Response->ErrorCode = 0;

	SocketSend(Socket, Connection, Response);

	return;

error:
	memcpy(&Character->Data.InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Character->Data.StellarMasteryInfo, &kStellarMasteryInfoBackup, sizeof(struct _RTCharacterStellarMasteryInfo));
	SocketDisconnect(Socket, Connection);
}