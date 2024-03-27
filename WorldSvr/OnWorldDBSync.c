#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

Void ServerSyncDB(
	ServerRef Server,
	ServerContextRef Context,
	Bool Force
) {
	Timestamp Timestamp = PlatformGetTickCount();

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
				(Character->SyncPriority.Low && (Timestamp - Character->SyncTimestamp) >= SERVER_DBSYNC_TIMEOUT_PRIORITY_LOW) ||
				(Character->SyncPriority.High && (Timestamp - Character->SyncTimestamp) >= SERVER_DBSYNC_TIMEOUT_PRIORITY_HIGH) ||
				(Character->SyncPriority.Immediate)
			)
		);

		if (PerformSync) {
			Character->SyncTimestamp = Timestamp;

			IPC_DATA_WORLD_REQDBSYNC* Request = PacketBufferInitExtended(Context->MasterSocket->PacketBuffer, IPC, WORLD_REQDBSYNC);
			Request->ConnectionID = Connection->ID;
			Request->AccountID = Client->Account.AccountID;
			Request->CharacterID = Client->CharacterDatabaseID;
			Request->SyncMask = Character->SyncMask;
			Request->SyncPriority = Character->SyncPriority;

			if (Character->SyncMask.Info) {
				RTMovementUpdateDeadReckoning(Context->Runtime, &Character->Movement);
				Character->Info.Position.X = Character->Movement.PositionCurrent.X;
				Character->Info.Position.Y = Character->Movement.PositionCurrent.Y;
                
                PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->Info, sizeof(struct _RTCharacterInfo));
			}

			if (Character->SyncMask.EquipmentInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
			}

			if (Character->SyncMask.InventoryInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
			}

			if (Character->SyncMask.SkillSlotInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->SkillSlotInfo, sizeof(struct _RTCharacterSkillSlotInfo));
			}

			if (Character->SyncMask.QuickSlotInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->QuickSlotInfo, sizeof(struct _RTCharacterQuickSlotInfo));
			}

			if (Character->SyncMask.QuestSlotInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->QuestSlotInfo, sizeof(struct _RTCharacterQuestSlotInfo));
			}

			if (Character->SyncMask.QuestFlagInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->QuestFlagInfo, sizeof(struct _RTCharacterQuestFlagInfo));
			}

            if (Character->SyncMask.DungeonQuestFlagInfo) {
                PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->DungeonQuestFlagInfo, sizeof(struct _RTCharacterDungeonQuestFlagInfo));
            }

			if (Character->SyncMask.EssenceAbilityInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->EssenceAbilityInfo, sizeof(struct _RTCharacterEssenceAbilityInfo));
			}

			if (Character->SyncMask.OverlordMasteryInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->OverlordMasteryInfo, sizeof(struct _RTCharacterOverlordMasteryInfo));
			}

			if (Character->SyncMask.CollectionInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->CollectionInfo, sizeof(struct _RTCharacterCollectionInfo));
			}

			if (Character->SyncMask.NewbieSupportInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->NewbieSupportInfo, sizeof(struct _RTCharacterNewbieSupportInfo));
			}

			if (Character->SyncMask.WarehouseInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));
			}

			if (Character->SyncMask.RequestCraftInfo) {
				PacketBufferAppendCopy(Context->MasterSocket->PacketBuffer, &Character->RequestCraftInfo, sizeof(struct _RTCharacterRequestCraftInfo));
			}

			SocketSendAll(Context->MasterSocket, Request);

			Character->SyncMask.RawValue = 0;
			Character->SyncPriority.RawValue = 0;
		}
	}
}

IPC_PROCEDURE_BINDING(OnWorldDBSync, IPC_WORLD_ACKDBSYNC, IPC_DATA_WORLD_ACKDBSYNC) {
	if (!Client || !Character) return;

	if (Packet->SyncMaskFailed.RawValue) {
		Character->SyncMask.RawValue |= Packet->SyncMaskFailed.RawValue;
		Character->SyncPriority.RawValue |= Packet->SyncPriority.RawValue;
	}
}
