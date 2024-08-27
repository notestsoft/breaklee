#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CREATE_CHARACTER) {
	Int32 PacketLength = sizeof(C2S_DATA_CREATE_CHARACTER) + sizeof(Char) * Packet->NameLength;
	if (Packet->Length != PacketLength) {
		SocketDisconnect(Socket, Connection);
		return;
	}

	S2C_DATA_CREATE_CHARACTER* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CREATE_CHARACTER);

	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) {
		SocketDisconnect(Socket, Connection);
		return;
	}

	if (Character) {
		Client->AccountInfo = Character->Data.AccountInfo;
	}

	if (Packet->NameLength < MIN_CHARACTER_NAME_LENGTH ||
		Packet->NameLength > MAX_CHARACTER_NAME_LENGTH) {
		Response->CharacterStatus = CREATE_CHARACTER_STATUS_NAME_VALIDATION_FAILED;
		SocketSend(Socket, Connection, Response);
		return;
	}

	union _RTCharacterStyle Style = { .RawValue = Packet->Style };
	UInt32 BattleStyleIndex = Style.BattleStyle | (Style.ExtendedBattleStyle << 3);

	if (Style.Padding1 > 0 ||
		Style.Padding2 > 0 ||
		Style.Padding3 > 0 ||
		Style.Padding4 > 0 ||
		Style.Unknown1 < 1 ||
		Style.AuraCode > 0 ||
		Style.ShowHelmed < 1 ||
		Style.HairStyle >= MAX_CHARACTER_NORMAL_HAIR_COUNT ||
		BattleStyleIndex < RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN ||
		BattleStyleIndex > RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX ||
		Style.Face >= MAX_CHARACTER_NORMAL_FACE_COUNT ||
		Style.HairColor >= MAX_CHARACTER_NORMAL_HAIR_COLOR_COUNT ||
		Style.BattleRank > 1) {
		Response->CharacterStatus = CREATE_CHARACTER_STATUS_NOT_ALLOWED;
		SocketSend(Socket, Connection, Response);
		return;
	}

	RTDataCharacterTemplateRef CharacterTemplate = RTRuntimeDataCharacterTemplateGet(Runtime->Context, BattleStyleIndex);
	RTDataCharacterInitRef CharacterInit = RTRuntimeDataCharacterInitGet(Context->Runtime->Context, BattleStyleIndex);
	RTDataCharacterInitStatRef CharacterInitStat = RTRuntimeDataCharacterInitStatGet(Context->Runtime->Context, BattleStyleIndex);
	if (!CharacterTemplate || !CharacterInit || !CharacterInitStat) {
		Response->CharacterStatus = CREATE_CHARACTER_STATUS_DBERROR;
		SocketSend(Socket, Connection, Response);
		return;
	}

	IPC_W2D_DATA_CREATE_CHARACTER* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, CREATE_CHARACTER);
	memset(&Request->CharacterData, 0, sizeof(struct _RTCharacterData));
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->CharacterSlotIndex = Packet->SlotIndex;
	Request->CharacterNameLength = Packet->NameLength;
	memcpy(Request->CharacterName, Packet->Name, Packet->NameLength);
	Request->CharacterData.Info.CurrentHP = INT32_MAX;
	Request->CharacterData.Info.CurrentMP = INT32_MAX;
	Request->CharacterData.Info.CurrentSP = INT32_MAX;
	Request->CharacterData.Info.CurrentBP = INT32_MAX;
	Request->CharacterData.Info.DP = 0;
	Request->CharacterData.Info.Level = 1;
	Request->CharacterData.Info.SkillRank = 1;
	Request->CharacterData.Info.SkillLevel = 0;
	Request->CharacterData.StyleInfo.Style = Style;
	Request->CharacterData.Info.WorldIndex = CharacterInit->WorldID;
	Request->CharacterData.Info.PositionX = CharacterInit->X;
	Request->CharacterData.Info.PositionY = CharacterInit->Y;

	if (CharacterInit->Suit > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.EquipmentSlots[Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->Suit;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_SUIT;
		Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount += 1;
	}

	if (CharacterInit->Glove > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.EquipmentSlots[Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->Glove;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_GLOVES;
		Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount += 1;
	}

	if (CharacterInit->Boot > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.EquipmentSlots[Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->Boot;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_BOOTS;
		Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount += 1;
	}

	if (CharacterInit->RightHand > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.EquipmentSlots[Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->RightHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_RIGHT;
		Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount += 1;
	}

	if (CharacterInit->LeftHand > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.EquipmentSlots[Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->LeftHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT;
		Request->CharacterData.EquipmentInfo.Info.EquipmentSlotCount += 1;
	}

	Request->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_STR] = CharacterInitStat->Str;
	Request->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] = CharacterInitStat->Dex;
	Request->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_INT] = CharacterInitStat->Int;

	for (Int32 Index = 0; Index < CharacterTemplate->CharacterTemplateSkillSlotCount; Index += 1) {
		RTDataCharacterTemplateSkillSlotRef TemplateSkillSlot = &CharacterTemplate->CharacterTemplateSkillSlotList[Index];
		RTSkillSlotRef CharacterSkillSlot = &Request->CharacterData.SkillSlotInfo.Slots[Request->CharacterData.SkillSlotInfo.Info.SlotCount];
		CharacterSkillSlot->ID = TemplateSkillSlot->SkillIndex;
		CharacterSkillSlot->Index = TemplateSkillSlot->SlotIndex;
		CharacterSkillSlot->Level = TemplateSkillSlot->Level;
		Request->CharacterData.SkillSlotInfo.Info.SlotCount += 1;
	}

	for (Int32 Index = 0; Index < CharacterTemplate->CharacterTemplateQuickSlotCount; Index += 1) {
		RTDataCharacterTemplateQuickSlotRef TemplateQuickSlot = &CharacterTemplate->CharacterTemplateQuickSlotList[Index];
		RTQuickSlotRef CharacterQuickSlot = &Request->CharacterData.QuickSlotInfo.Slots[Request->CharacterData.QuickSlotInfo.Info.SlotCount];
		CharacterQuickSlot->SkillIndex = TemplateQuickSlot->SkillIndex;
		CharacterQuickSlot->SlotIndex = TemplateQuickSlot->SlotIndex;
		Request->CharacterData.QuickSlotInfo.Info.SlotCount += 1;
	}

	for (Int32 Index = 0; Index < CharacterTemplate->CharacterTemplateInventorySlotCount; Index += 1) {
		RTDataCharacterTemplateInventorySlotRef TemplateInventorySlot = &CharacterTemplate->CharacterTemplateInventorySlotList[Index];
		
		struct _RTItemSlot ItemSlot = {
			.Item.Serial = TemplateInventorySlot->ItemID,
			.ItemOptions = TemplateInventorySlot->ItemOption,
			.SlotIndex = TemplateInventorySlot->SlotIndex,
		};
		RTInventorySetSlot(Runtime, &Request->CharacterData.InventoryInfo, &ItemSlot);		
	}

	Request->CharacterData.StyleInfo.MapsMask = 0xFFFFFFFF;
	Request->CharacterData.StyleInfo.WarpMask = 0xFFFFFFFF;

	Request->CharacterData.AnimaMasteryInfo.Info.PresetCount = 3;
	for (Int32 PresetIndex = 0; PresetIndex < RUNTIME_MAX_ANIMA_MASTERY_PRESET_COUNT; PresetIndex += 1) {
		for (Int32 CategoryIndex = 0; CategoryIndex < RUNTIME_MAX_ANIMA_MASTERY_CATEGORY_COUNT; CategoryIndex += 1) {
			Request->CharacterData.AnimaMasteryInfo.PresetData[PresetIndex].CategoryOrder[CategoryIndex].StorageIndex = 0;
			Request->CharacterData.AnimaMasteryInfo.PresetData[PresetIndex].CategoryOrder[CategoryIndex].CategoryIndex = CategoryIndex;
		}
	}

	if (Packet->CreateSpecialCharacter && Context->Config.WorldSvr.DebugCharacter) {
		Style.BattleRank = 10;
	}

	if (Packet->CreateSpecialCharacter && Context->Config.WorldSvr.DebugCharacter) {
		memset(Request->CharacterData.QuestInfo.Info.FinishedQuests, 0xFF, RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT);
		Request->CharacterData.StyleInfo.Style.BattleRank = 20;
		Request->CharacterData.Info.Level = Runtime->Context->LevelList[Runtime->Context->LevelCount - 1].Level;
		Request->CharacterData.Info.Exp = Runtime->Context->LevelList[Runtime->Context->LevelCount - 1].AccumulatedExp;
		Request->CharacterData.OverlordMasteryInfo.Info.Level = Runtime->Context->OverlordMasteryExpList[Runtime->Context->OverlordMasteryExpCount - 1].Level;
		Request->CharacterData.OverlordMasteryInfo.Info.Exp = Runtime->Context->OverlordMasteryExpList[Runtime->Context->OverlordMasteryExpCount - 1].AccumulatedExp;
		Request->CharacterData.Info.SkillRank = 10;
		Request->CharacterData.Info.SkillPoint = 1000;
		Request->CharacterData.Info.SkillLevel = 540;
		Request->CharacterData.ForceWingInfo.Info.Grade = 1;
		Request->CharacterData.ForceWingInfo.Info.Level = 1;
		Request->CharacterData.MythMasteryInfo.Info.Level = 94;
		Request->CharacterData.MythMasteryInfo.Info.Exp = 0;
		Request->CharacterData.MythMasteryInfo.Info.Rebirth = 1000;
		Request->CharacterData.MythMasteryInfo.Info.HolyPower = 27500;
		Request->CharacterData.MythMasteryInfo.Info.UnlockedPageCount = 4;
		Request->CharacterData.StyleInfo.MapsMask = 0xFFFFFFFF;
		Request->CharacterData.StyleInfo.WarpMask = 0xFFFFFFFF;
		Request->CharacterData.StyleInfo.Nation = 3;
		Request->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] = 2000;

		Request->CharacterData.AbilityInfo.Info.EssenceAbilityCount = MIN(Runtime->Context->PassiveAbilityCostCount, RUNTIME_CHARACTER_MAX_ESSENCE_ABILITY_SLOT_COUNT);
		Request->CharacterData.AbilityInfo.Info.ExtendedEssenceAbilityCount = MAX(0, Request->CharacterData.AbilityInfo.Info.EssenceAbilityCount - RUNTIME_CHARACTER_ESSENCE_ABILITY_SLOT_COUNT);
		for (Int32 Index = 0; Index < Request->CharacterData.AbilityInfo.Info.EssenceAbilityCount; Index += 1) {
			Request->CharacterData.AbilityInfo.EssenceAbilitySlots[Index].AbilityID = Runtime->Context->PassiveAbilityCostList[Index].Index;
			Request->CharacterData.AbilityInfo.EssenceAbilitySlots[Index].Level = Runtime->Context->PassiveAbilityCostList[Index].PassiveAbilityCostLevelList[Runtime->Context->PassiveAbilityCostList[Index].PassiveAbilityCostLevelCount - 1].Level;
		}

		Request->CharacterData.AbilityInfo.Info.BlendedAbilityCount = MIN(Runtime->Context->BlendedAbilityCostCount, RUNTIME_CHARACTER_MAX_BLENDED_ABILITY_SLOT_COUNT);
		Request->CharacterData.AbilityInfo.Info.ExtendedBlendedAbilityCount = MAX(0, Request->CharacterData.AbilityInfo.Info.BlendedAbilityCount - RUNTIME_CHARACTER_BLENDED_ABILITY_SLOT_COUNT);
		for (Int32 Index = 0; Index < Request->CharacterData.AbilityInfo.Info.BlendedAbilityCount; Index += 1) {
			Request->CharacterData.AbilityInfo.BlendedAbilitySlots[Index].AbilityID = Runtime->Context->BlendedAbilityCostList[Index].Index;
		}

		Request->CharacterData.AbilityInfo.Info.KarmaAbilityCount = MIN(Runtime->Context->KarmaAbilityCostCount, RUNTIME_CHARACTER_MAX_KARMA_ABILITY_SLOT_COUNT);
		Request->CharacterData.AbilityInfo.Info.ExtendedKarmaAbilityCount = MAX(0, Request->CharacterData.AbilityInfo.Info.KarmaAbilityCount - RUNTIME_CHARACTER_KARMA_ABILITY_SLOT_COUNT);
		for (Int32 Index = 0; Index < Request->CharacterData.AbilityInfo.Info.KarmaAbilityCount; Index += 1) {
			Request->CharacterData.AbilityInfo.KarmaAbilitySlots[Index].AbilityID = Runtime->Context->KarmaAbilityCostList[Index].Index;
			Request->CharacterData.AbilityInfo.KarmaAbilitySlots[Index].Level = Runtime->Context->KarmaAbilityCostList[Index].KarmaAbilityCostLevelList[Runtime->Context->KarmaAbilityCostList[Index].KarmaAbilityCostLevelCount - 1].Level;
		}

		Request->CharacterData.StyleInfo.Nation = 2;
		Request->CharacterData.Info.HonorPoint = Runtime->Context->HonorLevelFormulaList[Runtime->Context->HonorLevelFormulaCount - 1].MaxPoint - 1;
		Request->CharacterData.HonorMedalInfo.Info.Score = Runtime->Context->HonorMedalScoreCategoryList[0].HonorMedalScoreMedalList[Runtime->Context->HonorMedalScoreCategoryList[0].HonorMedalScoreMedalCount - 1].AccumulatedRequiredScore - 1;
		Request->CharacterData.HonorMedalInfo.Info.SlotCount = 0;
		for (Int32 Index = 0; Index < 4; Index += 1) {
			RTHonorMedalSlotRef Slot = &Request->CharacterData.HonorMedalInfo.Slots[Request->CharacterData.HonorMedalInfo.Info.SlotCount];
			Slot->CategoryIndex = 0;
			Slot->GroupIndex = 1;
			Slot->SlotIndex = Request->CharacterData.HonorMedalInfo.Info.SlotCount;
			Slot->ForceEffectIndex = 45;
			Slot->IsUnlocked = 1;
			Request->CharacterData.HonorMedalInfo.Info.SlotCount += 1;
		}

		Int32 SlotIndex = Request->CharacterData.SkillSlotInfo.Info.SlotCount;
		for (Index Index = 144; Index < 148; Index++) {
			RTSkillSlotRef GmSkill = &Request->CharacterData.SkillSlotInfo.Slots[Request->CharacterData.SkillSlotInfo.Info.SlotCount];
			GmSkill->ID = Index;
			GmSkill->Level = 1;
			GmSkill->Index = SlotIndex++;
			Request->CharacterData.SkillSlotInfo.Info.SlotCount += 1;
		}
		/*
		AchievementIndex = Request->CharacterData.AchievementInfo.AchievementExtendedRewardCount;
		Request->CharacterData.AchievementInfo.AchievementRewardSlots[AchievementIndex].TitleIndex = AchievementReward->AchievementID;
		Request->CharacterData.AchievementInfo.AchievementRewardSlots[AchievementIndex].Unknown1 = 0;
		Request->CharacterData.AchievementInfo.AchievementRewardCount += 1;

		Request->CharacterData.Info.Alz= 999999999;
		Request->CharacterData.Info.Currency[RUNTIME_CHARACTER_CURRENCY_GEM] = 999999;
		Request->CharacterData.Info.Level = 200;
		Request->CharacterData.Info.Overlord.Level = 1;
		Request->CharacterData.Info.Overlord.Point = 1;
		Request->CharacterData.Info.Exp = RTRuntimeGetExpByLevel(Runtime, 200);
		Request->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] = 200 * 5;
		Request->CharacterData.StyleInfo.Style.BattleRank = 20;
		Request->CharacterData.Info.Skill.Rank = 10;
		Request->CharacterData.Info.Skill.Level = 500;
		Request->CharacterData.StyleInfo.Nation = 2;

		RTDataHonorLevelFormulaRef HonorLevelFormula = RTRuntimeDataHonorLevelFormulaGet(Runtime->Context, 20);
		Request->CharacterData.Info.HonorRank = HonorLevelFormula->Rank;
		Request->CharacterData.Info.HonorPoint = HonorLevelFormula->MaxPoint;
		Request->CharacterData.Info.Wexp = 0;
		*/
	}

	IPCSocketUnicast(Server->IPCSocket, Request);
}

IPC_PROCEDURE_BINDING(D2W, CREATE_CHARACTER) {
	if (!ClientConnection || !Client) return;

	if (Packet->Status == CREATE_CHARACTER_STATUS_SUCCESS) {
		Client->Characters[Packet->CharacterSlotIndex] = Packet->Character;
	}
	
	S2C_DATA_CREATE_CHARACTER* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, CREATE_CHARACTER);
	Response->CharacterIndex = Packet->Character.CharacterID * MAX_CHARACTER_COUNT + Packet->CharacterSlotIndex;
	Response->CharacterStatus = Packet->Status;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}
