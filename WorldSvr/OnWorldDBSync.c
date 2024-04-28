#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// TODO: Sync vehicle inventory to database!

Void ServerSyncCharacter(
	ServerRef Server,
	ServerContextRef Context,
	ClientContextRef Client,
	RTCharacterRef Character
) {
	Character->SyncTimestamp = GetTimestampMs();

	IPC_W2M_DATA_DBSYNC* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2M, DBSYNC);
	Request->Header.SourceConnectionID = Client->Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTER;
	Request->AccountID = Client->Account.AccountID;
	Request->CharacterID = Client->CharacterDatabaseID;
	Request->SyncMask = Character->SyncMask;
	Request->SyncPriority = Character->SyncPriority;

	if (Character->SyncMask.Info) {
		RTMovementUpdateDeadReckoning(Context->Runtime, &Character->Movement);
		Character->Info.Position.X = Character->Movement.PositionCurrent.X;
		Character->Info.Position.Y = Character->Movement.PositionCurrent.Y;

		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Info, sizeof(struct _RTCharacterInfo));
	}

	if (Character->SyncMask.EquipmentInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
	}

	if (Character->SyncMask.InventoryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
	}

	if (Character->SyncMask.SkillSlotInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->SkillSlotInfo, sizeof(struct _RTCharacterSkillSlotInfo));
	}

	if (Character->SyncMask.QuickSlotInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->QuickSlotInfo, sizeof(struct _RTCharacterQuickSlotInfo));
	}

	if (Character->SyncMask.QuestSlotInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->QuestSlotInfo, sizeof(struct _RTCharacterQuestSlotInfo));
	}

	if (Character->SyncMask.QuestFlagInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->QuestFlagInfo, sizeof(struct _RTCharacterQuestFlagInfo));
	}

	if (Character->SyncMask.DungeonQuestFlagInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->DungeonQuestFlagInfo, sizeof(struct _RTCharacterDungeonQuestFlagInfo));
	}

	if (Character->SyncMask.EssenceAbilityInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->EssenceAbilityInfo, sizeof(struct _RTCharacterEssenceAbilityInfo));
	}

	if (Character->SyncMask.OverlordMasteryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->OverlordMasteryInfo, sizeof(struct _RTCharacterOverlordMasteryInfo));
	}

	if (Character->SyncMask.ForceWingInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->ForceWingInfo, sizeof(struct _RTCharacterForceWingInfo));
	}

	if (Character->SyncMask.CollectionInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->CollectionInfo, sizeof(struct _RTCharacterCollectionInfo));
	}

	if (Character->SyncMask.NewbieSupportInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->NewbieSupportInfo, sizeof(struct _RTCharacterNewbieSupportInfo));
	}

	if (Character->SyncMask.WarehouseInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));
	}

	if (Character->SyncMask.RequestCraftInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->RequestCraftInfo, sizeof(struct _RTCharacterRequestCraftInfo));
	}

	IPCSocketUnicast(Server->IPCSocket, Request);

	Character->SyncMask.RawValue = 0;
	Character->SyncPriority.RawValue = 0;
}

Void ServerSyncDB(
	ServerRef Server,
	ServerContextRef Context,
	Bool Force
) {
	Timestamp Timestamp = GetTimestampMs();

	SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
	while (Iterator) {
		SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
		// TODO: Connection can become null!!!

		Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

		ClientContextRef Client = (ClientContextRef)Connection->Userdata;
		if (Client->CharacterIndex < 1) continue;

		RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, Client->CharacterIndex);
		assert(Character);

		Bool PerformSync = Force || (
			Character->SyncMask.RawValue && (
				(Character->SyncPriority.Low && (Timestamp - Character->SyncTimestamp) >= Context->Config.WorldSvr.DBSyncTimerPriorityLow) ||
				(Character->SyncPriority.High && (Timestamp - Character->SyncTimestamp) >= Context->Config.WorldSvr.DBSyncTimerPriorityHigh) ||
				(Character->SyncPriority.Immediate)
			)
		);

		if (PerformSync) {
			ServerSyncCharacter(Server, Context, Client, Character);
		}
	}
}

IPC_PROCEDURE_BINDING(M2W, DBSYNC) {
	if (!Client || !Character) return;

	if (Packet->SyncMaskFailed.RawValue) {
		Character->SyncMask.RawValue |= Packet->SyncMaskFailed.RawValue;
		Character->SyncPriority.RawValue |= Packet->SyncPriority.RawValue;
	}
}
