#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CREATE_CHARACTER) {
	S2C_DATA_CREATE_CHARACTER* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CREATE_CHARACTER);

	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) {
		return SocketDisconnect(Socket, Connection);
	}

	// TODO: CharacterSlotFlags is not set correctly?!
	if (!(Client->Account.CharacterSlotFlags & (1 << Packet->SlotIndex)) ||
		Client->Characters[Packet->SlotIndex].ID > 0) {
		Response->CharacterStatus = CREATE_CHARACTER_STATUS_NOT_ALLOWED;
		return SocketSend(Socket, Connection, Response);
	}

	if (Packet->NameLength < MIN_CHARACTER_NAME_LENGTH ||
		Packet->NameLength > MAX_CHARACTER_NAME_LENGTH) {
		Response->CharacterStatus = CREATE_CHARACTER_STATUS_NAME_VALIDATION_FAILED;
		return SocketSend(Socket, Connection, Response);
	}

	UInt32 RawStyle = SwapUInt32(Packet->Style);
	union _RTCharacterStyle Style = *((union _RTCharacterStyle*)&RawStyle);
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
		return SocketSend(Socket, Connection, Response);
	}

	if (Packet->CreateSpecialCharacter && Context->Config.WorldSvr.DebugCharacter) {
		Style.BattleRank = 10;
	}

	IPC_W2D_DATA_CREATE_CHARACTER* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, CREATE_CHARACTER);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->Account.AccountID;
	Request->CharacterSlotIndex = Packet->SlotIndex;
	Request->CharacterNameLength = Packet->NameLength;
	memcpy(Request->CharacterName, Packet->Name, Packet->NameLength);

	struct _RuntimeDataCharacterTemplate* CharacterTemplate = &Context->RuntimeData->CharacterTemplate[BattleStyleIndex - 1];
	if (CharacterTemplate->BattleStyleIndex != BattleStyleIndex) {
		Response->CharacterStatus = CREATE_CHARACTER_STATUS_DBERROR;
		return SocketSend(Socket, Connection, Response);
	}

	Request->CharacterData.Info.Resource.HP = INT32_MAX;
	Request->CharacterData.Info.Resource.MP = INT32_MAX;
	Request->CharacterData.Info.Resource.SP = INT32_MAX;
	Request->CharacterData.Info.Resource.BP = INT32_MAX;
	Request->CharacterData.Info.Resource.DP = 0;
	Request->CharacterData.Info.Basic.Level = 1;
	Request->CharacterData.Info.Skill.Rank = 1;
	Request->CharacterData.Info.Skill.Level = 0;
	Request->CharacterData.Info.Style = Style;

	RTDataPassiveAbilitySlotLimitRef PassiveAbilitySlotLimit = RTRuntimeDataPassiveAbilitySlotLimitGet(Runtime->Context);
	if (PassiveAbilitySlotLimit) {
		Request->CharacterData.Info.Ability.MaxEssenceAbilitySlotCount = PassiveAbilitySlotLimit->MinCount;
	}

	RTDataBlendedAbilitySlotLimitRef BlendedAbilitySlotLimit = RTRuntimeDataBlendedAbilitySlotLimitGet(Runtime->Context);
	if (BlendedAbilitySlotLimit) {
		Request->CharacterData.Info.Ability.MaxBlendedAbilitySlotCount = BlendedAbilitySlotLimit->MinCount;
	}

	RTDataKarmaAbilitySlotLimitRef KarmaAbilitySlotLimit = RTRuntimeDataKarmaAbilitySlotLimitGet(Runtime->Context);
	if (KarmaAbilitySlotLimit) {
		Request->CharacterData.Info.Ability.MaxKarmaAbilitySlotCount = KarmaAbilitySlotLimit->MinCount;
	}

	RTDataCharacterInitRef CharacterInit = RTRuntimeDataCharacterInitGet(Context->Runtime->Context, BattleStyleIndex);
	Request->CharacterData.Info.Position.WorldID = CharacterInit->WorldID;
	Request->CharacterData.Info.Position.X = CharacterInit->X;
	Request->CharacterData.Info.Position.Y = CharacterInit->Y;

	if (CharacterInit->Suit > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.Slots[Request->CharacterData.EquipmentInfo.Count];
		ItemSlot->Item.ID = CharacterInit->Suit;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_SUIT;
		Request->CharacterData.EquipmentInfo.Count += 1;
	}

	if (CharacterInit->Glove > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.Slots[Request->CharacterData.EquipmentInfo.Count];
		ItemSlot->Item.ID = CharacterInit->Glove;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_GLOVES;
		Request->CharacterData.EquipmentInfo.Count += 1;
	}

	if (CharacterInit->Boot > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.Slots[Request->CharacterData.EquipmentInfo.Count];
		ItemSlot->Item.ID = CharacterInit->Boot;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_BOOTS;
		Request->CharacterData.EquipmentInfo.Count += 1;
	}

	if (CharacterInit->RightHand > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.Slots[Request->CharacterData.EquipmentInfo.Count];
		ItemSlot->Item.ID = CharacterInit->RightHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_RIGHT;
		Request->CharacterData.EquipmentInfo.Count += 1;
	}

	if (CharacterInit->LeftHand > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterData.EquipmentInfo.Slots[Request->CharacterData.EquipmentInfo.Count];
		ItemSlot->Item.ID = CharacterInit->LeftHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT;
		Request->CharacterData.EquipmentInfo.Count += 1;
	}

	RTDataCharacterInitStatRef CharacterInitStat = RTRuntimeDataCharacterInitStatGet(Context->Runtime->Context, BattleStyleIndex);
	Request->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_STR] = CharacterInitStat->Str;
	Request->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] = CharacterInitStat->Dex;
	Request->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_INT] = CharacterInitStat->Int;

	memcpy(&Request->CharacterData.InventoryInfo, &CharacterTemplate->Inventory, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Request->CharacterData.SkillSlotInfo, &CharacterTemplate->SkillSlots, sizeof(struct _RTCharacterSkillSlotInfo));
	memcpy(&Request->CharacterData.QuickSlotInfo, &CharacterTemplate->QuickSlots, sizeof(struct _RTCharacterQuickSlotInfo));

	if (Packet->CreateSpecialCharacter && Context->Config.WorldSvr.DebugCharacter) {
		Request->CharacterData.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] = 999999999;
		Request->CharacterData.Info.Currency[RUNTIME_CHARACTER_CURRENCY_GEM] = 999999;
		Request->CharacterData.Info.Basic.Level = 200;
		Request->CharacterData.Info.Overlord.Level = 1;
		Request->CharacterData.Info.Overlord.Point = 1;
		Request->CharacterData.Info.Basic.Exp = RTRuntimeGetExpByLevel(Runtime, 200);
		Request->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] = 200 * 5;
		Request->CharacterData.Info.Style.BattleRank = 20;
		Request->CharacterData.Info.Skill.Rank = 10;
		Request->CharacterData.Info.Skill.Level = 500;
		Request->CharacterData.Info.Profile.Nation = 2;

		RTDataHonorLevelFormulaRef HonorLevelFormula = RTRuntimeDataHonorLevelFormulaGet(Runtime->Context, 20);
		Request->CharacterData.Info.Honor.Rank = HonorLevelFormula->Rank;
		Request->CharacterData.Info.Honor.Point = HonorLevelFormula->MaxPoint;
		Request->CharacterData.Info.Honor.Exp = 0;
	}

	IPCSocketUnicast(Server->IPCSocket, Request);
}

IPC_PROCEDURE_BINDING(D2W, CREATE_CHARACTER) {
	if (!ClientConnection || !Client) return;

	if (Packet->Status == CREATE_CHARACTER_STATUS_SUCCESS) {
		Client->Characters[Packet->CharacterSlotIndex] = Packet->Character;
	}
	
	S2C_DATA_CREATE_CHARACTER* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, CREATE_CHARACTER);
	Response->CharacterIndex = Packet->Character.ID * MAX_CHARACTER_COUNT + Packet->CharacterSlotIndex;
	Response->CharacterStatus = Packet->Status;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}
