#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(STELLAR_LINK_IMPRINT_SLOT) {
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

	// TODO: Add check for MaterialSlotCount2

	Response->GroupID = Packet->GroupID;
	Response->SlotLine = Packet->SlotLine;
	Response->SlotIndex = Packet->SlotIndex;
	Response->StellarLinkGrade = RUNTIME_STELLAR_MASTERY_SLOT_LINK_GRADE_WRATH;
	Response->StellarForceEffect = RUNTIME_FORCE_EFFECT_ATTACK_UP;
	Response->StellarForceValue = 20;
	Response->StellarForceValueType = RUNTIME_FORCE_VALUE_TYPE_ADDITIVE;
	Response->ErrorCode = 0;

	SocketSend(Socket, Connection, Response);
	
	return;

error:
	SocketDisconnect(Socket, Connection);
}