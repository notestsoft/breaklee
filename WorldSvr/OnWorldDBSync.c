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

			IPC_DATA_WORLD_REQDBSYNC* Request = PacketInitExtended(IPC_DATA_WORLD_REQDBSYNC);
			Request->Command = IPC_WORLD_REQDBSYNC;
			Request->ConnectionID = Connection->ID;
			Request->AccountID = Client->Account.AccountID;
			Request->CharacterID = Client->CharacterDatabaseID;
			Request->SyncMask = Character->SyncMask;
			Request->SyncPriority = Character->SyncPriority;

			if (Character->SyncMask.Info) {
				RTMovementUpdateDeadReckoning(Context->Runtime, &Character->Movement);
				Character->Info.Position.X = Character->Movement.PositionCurrent.X;
				Character->Info.Position.Y = Character->Movement.PositionCurrent.Y;

				PacketAppendMemoryCopy(&Character->Info, sizeof(struct _RTCharacterInfo));
			}

			if (Character->SyncMask.EquipmentInfo) {
				PacketAppendMemoryCopy(&Character->EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
			}

			if (Character->SyncMask.InventoryInfo) {
				PacketAppendMemoryCopy(&Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
			}

			if (Character->SyncMask.SkillSlotInfo) {
				PacketAppendMemoryCopy(&Character->SkillSlotInfo, sizeof(struct _RTCharacterSkillSlotInfo));
			}

			if (Character->SyncMask.QuickSlotInfo) {
				PacketAppendMemoryCopy(&Character->QuickSlotInfo, sizeof(struct _RTCharacterQuickSlotInfo));
			}

			if (Character->SyncMask.QuestSlotInfo) {
				PacketAppendMemoryCopy(&Character->QuestSlotInfo, sizeof(struct _RTCharacterQuestSlotInfo));
			}

			if (Character->SyncMask.QuestFlagInfo) {
				PacketAppendMemoryCopy(&Character->QuestFlagInfo, sizeof(struct _RTCharacterQuestFlagInfo));
			}

            if (Character->SyncMask.DungeonQuestFlagInfo) {
                PacketAppendMemoryCopy(&Character->DungeonQuestFlagInfo, sizeof(struct _RTCharacterDungeonQuestFlagInfo));
            }

			if (Character->SyncMask.EssenceAbilityInfo) {
				PacketAppendMemoryCopy(&Character->EssenceAbilityInfo, sizeof(struct _RTCharacterEssenceAbilityInfo));
			}

			if (Character->SyncMask.OverlordMasteryInfo) {
				PacketAppendMemoryCopy(&Character->OverlordMasteryInfo, sizeof(struct _RTCharacterOverlordMasteryInfo));
			}

			if (Character->SyncMask.CollectionInfo) {
				PacketAppendMemoryCopy(&Character->CollectionInfo, sizeof(struct _RTCharacterCollectionInfo));
			}

			if (Character->SyncMask.WarehouseInfo) {
				PacketAppendMemoryCopy(&Character->WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));
			}

			if (Character->SyncMask.RequestCraftInfo) {
				PacketAppendMemoryCopy(&Character->RequestCraftInfo, sizeof(struct _RTCharacterRequestCraftInfo));
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
