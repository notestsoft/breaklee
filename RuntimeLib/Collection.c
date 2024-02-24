#include "Character.h"
#include "Collection.h"

RTCollectionSlotRef RTCharacterGetCollectionSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt8 TypeID,
	UInt8 CollectionID,
	UInt16 MissionID
) {
	for (Int32 Index = 0; Index < Character->CollectionInfo.Count; Index += 1) {
		RTCollectionSlotRef Slot = &Character->CollectionInfo.Slots[Index];
		if (Slot->TypeID != TypeID) continue;
		if (Slot->CollectionID != CollectionID) continue;
		if (Slot->MissionID != MissionID) continue;

		return Slot;
	}

	assert(Character->CollectionInfo.Count + 1 < RUNTIME_CHARACTER_MAX_COLLECTION_SLOT_COUNT);
	RTCollectionSlotRef Slot = &Character->CollectionInfo.Slots[Character->CollectionInfo.Count];
	Character->CollectionInfo.Count += 1;
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_COLLECTION;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
	memset(Slot, 0, sizeof(struct _RTCollectionSlot));
	Slot->TypeID = TypeID;
	Slot->CollectionID = CollectionID;
	Slot->MissionID = MissionID;
	return Slot;
}
