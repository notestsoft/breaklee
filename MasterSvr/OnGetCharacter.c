#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2M, GET_CHARACTER) {
    IPC_M2W_DATA_GET_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, M2W, GET_CHARACTER);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

    Account->AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, Account)) goto error;

    Character->CharacterID = Packet->CharacterID;
	if (!MasterDBSelectCharacterByID(Context->Database, Character)) goto error;

	if (Character->AccountID != Account->AccountID) goto error;

	Response->Success = true;
    Response->CharacterIndex = Packet->CharacterIndex;
    Response->Character.ID = Character->CharacterID;
    Response->Character.Index = Character->Index;
    Response->Character.CreationDate = Character->CreatedAt;

    CStringCopySafe(Response->Character.Name, MAX_CHARACTER_NAME_LENGTH + 1, Character->Name);
    memcpy(&Response->Character.CharacterData, &Character->CharacterData, sizeof(struct _RTCharacterInfo));
    memcpy(&Response->Character.EquipmentData, &Character->EquipmentData, sizeof(struct _RTCharacterEquipmentInfo));
    memcpy(&Response->Character.InventoryData, &Character->InventoryData, sizeof(struct _RTCharacterInventoryInfo));
    memcpy(&Response->Character.SkillSlotData, &Character->SkillSlotData, sizeof(struct _RTCharacterSkillSlotInfo));
    memcpy(&Response->Character.QuickSlotData, &Character->QuickSlotData, sizeof(struct _RTCharacterQuickSlotInfo));
    memcpy(&Response->Character.QuestSlotData, &Character->QuestSlotData, sizeof(struct _RTCharacterQuestSlotInfo));
    memcpy(&Response->Character.QuestFlagData, &Character->QuestFlagData, sizeof(struct _RTCharacterQuestFlagInfo));
    memcpy(&Response->Character.DungeonQuestFlagData, &Character->DungeonQuestFlagData, sizeof(struct _RTCharacterQuestFlagInfo));
    memcpy(&Response->Character.AchievementData, &Character->AchievementData, sizeof(GAME_DATA_CHARACTER_ACHIEVEMENT));
    memcpy(&Response->Character.EssenceAbilityData, &Character->EssenceAbilityData, sizeof(struct _RTCharacterEssenceAbilityInfo));
    memcpy(&Response->Character.BlendedAbilityData, &Character->BlendedAbilityData, sizeof(struct _RTCharacterBlendedAbilityInfo));
    memcpy(&Response->Character.HonorMedalData, &Character->HonorMedalData, sizeof(struct _RTCharacterHonorMedalInfo));
    memcpy(&Response->Character.OverlordData, &Character->OverlordData, sizeof(struct _RTCharacterOverlordMasteryInfo));
    memcpy(&Response->Character.ForceWingData, &Character->ForceWingData, sizeof(struct _RTCharacterForceWingInfo));
    memcpy(&Response->Character.TransformData, &Character->TransformData, sizeof(GAME_DATA_CHARACTER_TRANSFORM));
    memcpy(&Response->Character.TranscendenceData, &Character->TranscendenceData, sizeof(GAME_DATA_CHARACTER_TRANSCENDENCE));
    memcpy(&Response->Character.MercenaryData, &Character->MercenaryData, sizeof(GAME_DATA_CHARACTER_MERCENARY));
    memcpy(&Response->Character.CraftData, &Character->CraftData, sizeof(GAME_DATA_CHARACTER_CRAFT));
    memcpy(&Response->Character.WarehouseData, &Context->TempAccount.WarehouseData, sizeof(struct _RTCharacterWarehouseInfo));
    memcpy(&Response->Character.CollectionData, &Context->TempAccount.CollectionData, sizeof(struct _RTCharacterCollectionInfo));
    memcpy(&Response->Character.NewbieSupportData, &Character->NewbieSupportData, sizeof(struct _RTCharacterNewbieSupportInfo));

    IPCSocketUnicast(Socket, Response);
    return;

error:
	Response->Success = false;
    IPCSocketUnicast(Socket, Response);
}