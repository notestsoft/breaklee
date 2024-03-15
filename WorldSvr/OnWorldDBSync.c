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
			Character->SyncMask &&
			(
				((Character->SyncPriority & RUNTIME_CHARACTER_SYNC_PRIORITY_LOW) && (Timestamp - Character->SyncTimestamp) >= SERVER_DBSYNC_TIMEOUT_PRIORITY_LOW) ||
				((Character->SyncPriority & RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH) && (Timestamp - Character->SyncTimestamp) >= SERVER_DBSYNC_TIMEOUT_PRIORITY_HIGH) ||
				(Character->SyncPriority & RUNTIME_CHARACTER_SYNC_PRIORITY_INSTANT)
				)
			);

		if (PerformSync) {
			Character->SyncTimestamp = Timestamp;

			IPC_DATA_WORLD_REQDBSYNC* Request = PacketInit(IPC_DATA_WORLD_REQDBSYNC);
			Request->Command = IPC_WORLD_REQDBSYNC;
			Request->ConnectionID = Connection->ID;
			Request->AccountID = Client->Account.AccountID;
			Request->CharacterID = Client->CharacterDatabaseID;
			Request->DBSyncMask = Character->SyncMask;
			Request->DBSyncPriority = Character->SyncPriority;

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_INFO) {
				RTMovementUpdateDeadReckoning(Context->Runtime, &Character->Movement);
				Character->Info.Position.X = Character->Movement.PositionCurrent.X;
				Character->Info.Position.Y = Character->Movement.PositionCurrent.Y;

				PacketAppendMemoryCopy(&Character->Info, sizeof(struct _RTCharacterInfo));
			}

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_EQUIPMENT) {
				PacketAppendMemoryCopy(&Character->EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
			}

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_INVENTORY) {
				PacketAppendMemoryCopy(&Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
			}

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_SKILLSLOT) {
				PacketAppendMemoryCopy(&Character->SkillSlotInfo, sizeof(struct _RTCharacterSkillSlotInfo));
			}

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_QUICKSLOT) {
				PacketAppendMemoryCopy(&Character->QuickSlotInfo, sizeof(struct _RTCharacterQuickSlotInfo));
			}

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_QUESTSLOT) {
				PacketAppendMemoryCopy(&Character->QuestSlotInfo, sizeof(struct _RTCharacterQuestSlotInfo));
			}

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_QUESTFLAG) {
				PacketAppendMemoryCopy(&Character->QuestFlagInfo, sizeof(struct _RTCharacterQuestFlagInfo));
			}

            if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_DUNGEONQUESTFLAG) {
                PacketAppendMemoryCopy(&Character->DungeonQuestFlagInfo, sizeof(struct _RTCharacterDungeonQuestFlagInfo));
            }

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_ESSENCE_ABILITY) {
				PacketAppendMemoryCopy(&Character->EssenceAbilityInfo, sizeof(struct _RTCharacterEssenceAbilityInfo));
			}

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_OVERLORD) {
				PacketAppendMemoryCopy(&Character->OverlordMasteryInfo, sizeof(struct _RTCharacterOverlordMasteryInfo));
			}

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_COLLECTION) {
				PacketAppendMemoryCopy(&Character->CollectionInfo, sizeof(struct _RTCharacterCollectionInfo));
			}

			if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_WAREHOUSE) {
				PacketAppendMemoryCopy(&Character->WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));
			}

			SocketSendAll(Context->MasterSocket, Request);

			Character->SyncMask = 0;
			Character->SyncPriority = 0;
		}
	}
}

IPC_PROCEDURE_BINDING(OnWorldDBSync, IPC_WORLD_ACKDBSYNC, IPC_DATA_WORLD_ACKDBSYNC) {
	if (!Client || !Character) return;

	if (Packet->DBSyncMaskFailure) {
		Character->SyncMask |= Packet->DBSyncMaskFailure;
		Character->SyncPriority |= Packet->DBSyncPriority;
	}
}
