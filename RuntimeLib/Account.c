#include "Account.h"
#include "Character.h"

Void RTCharacterAddForceGem(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 Amount
) {
	Character->Data.AccountInfo.ForceGem += Amount;
	Character->SyncMask.AccountInfo = true;
}

Bool RTCharacterConsumeForceGem(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 Amount
) {
	if (Character->Data.AccountInfo.ForceGem < Amount) return false;

	Character->Data.AccountInfo.ForceGem -= Amount;
	Character->SyncMask.AccountInfo = true;
	return true;
}

Bool RTAccountIsCharacterSlotOpen(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int Index
) {
	return Character->Data.AccountInfo.CharacterSlotOpenMask & (1 << Index);
}

Bool RTAccountOpenCharacterSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	for (Int Index = 0; Index < RUNTIME_ACCOUNT_MAX_CHARACTER_COUNT; Index += 1) {
		if ((Character->Data.AccountInfo.CharacterSlotOpenMask & (1 << Index)) < 1) {
			Character->Data.AccountInfo.CharacterSlotOpenMask |= 1 << Index;
			Character->SyncMask.AccountInfo = true;
			return true;
		}
	}

	return false;
}
