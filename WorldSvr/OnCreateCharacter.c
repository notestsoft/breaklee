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

	IPC_W2M_DATA_CREATE_CHARACTER* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2M, CREATE_CHARACTER);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTER;
	Request->AccountID = Client->Account.AccountID;
	Request->SlotIndex = Packet->SlotIndex;
	Request->NameLength = Packet->NameLength;
	memcpy(Request->Name, Packet->Name, Packet->NameLength);

	struct _RuntimeDataCharacterTemplate* CharacterTemplate = &Context->RuntimeData->CharacterTemplate[BattleStyleIndex - 1];
	if (CharacterTemplate->BattleStyleIndex != BattleStyleIndex) {
		Response->CharacterStatus = CREATE_CHARACTER_STATUS_DBERROR;
		return SocketSend(Socket, Connection, Response);
	}

	Request->CharacterData.Resource.HP = INT32_MAX;
	Request->CharacterData.Resource.MP = INT32_MAX;
	Request->CharacterData.Resource.SP = INT32_MAX;
	Request->CharacterData.Resource.BP = INT32_MAX;
	Request->CharacterData.Resource.DP = 0;
	Request->CharacterData.Basic.Level = 1;
	Request->CharacterData.Skill.Rank = 1;
	Request->CharacterData.Skill.Level = 0;
	Request->CharacterData.Style = Style;

	RTDataPassiveAbilitySlotLimitRef PassiveAbilitySlotLimit = RTRuntimeDataPassiveAbilitySlotLimitGet(Runtime->Context);
	if (PassiveAbilitySlotLimit) {
		Request->CharacterData.Ability.MaxEssenceAbilitySlotCount = PassiveAbilitySlotLimit->MinCount;
	}

	RTDataCharacterInitRef CharacterInit = RTRuntimeDataCharacterInitGet(Context->Runtime->Context, BattleStyleIndex);
	Request->CharacterData.Position.WorldID = CharacterInit->WorldID;
	Request->CharacterData.Position.X = CharacterInit->X;
	Request->CharacterData.Position.Y = CharacterInit->Y;

	if (CharacterInit->Suit > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->Suit;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_SUIT;

		Request->CharacterEquipment.Count += 1;
	}

	if (CharacterInit->Glove > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->Glove;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_GLOVES;

		Request->CharacterEquipment.Count += 1;
	}

	if (CharacterInit->Boot > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->Boot;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_BOOTS;

		Request->CharacterEquipment.Count += 1;
	}

	if (CharacterInit->RightHand > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->RightHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_RIGHT;

		Request->CharacterEquipment.Count += 1;
	}

	if (CharacterInit->LeftHand > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->LeftHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT;

		Request->CharacterEquipment.Count += 1;
	}

	RTDataCharacterInitStatRef CharacterInitStat = RTRuntimeDataCharacterInitStatGet(Context->Runtime->Context, BattleStyleIndex);
	Request->CharacterData.Stat[RUNTIME_CHARACTER_STAT_STR] = CharacterInitStat->Str;
	Request->CharacterData.Stat[RUNTIME_CHARACTER_STAT_DEX] = CharacterInitStat->Dex;
	Request->CharacterData.Stat[RUNTIME_CHARACTER_STAT_INT] = CharacterInitStat->Int;

	memcpy(&Request->CharacterInventory, &CharacterTemplate->Inventory, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Request->CharacterSkillSlots, &CharacterTemplate->SkillSlots, sizeof(struct _RTCharacterSkillSlotInfo));
	memcpy(&Request->CharacterQuickSlots, &CharacterTemplate->QuickSlots, sizeof(struct _RTCharacterQuickSlotInfo));

	if (Packet->CreateSpecialCharacter && Context->Config.WorldSvr.DebugCharacter) {
		Request->CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] = 999999999;
		Request->CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_GEM] = 999999;
		Request->CharacterData.Basic.Level = 200;
		Request->CharacterData.Overlord.Level = 1;
		Request->CharacterData.Overlord.Point = 1;
		Request->CharacterData.Basic.Exp = RTRuntimeGetExpByLevel(Runtime, 200);
		Request->CharacterData.Stat[RUNTIME_CHARACTER_STAT_PNT] = 200 * 5;
		Request->CharacterData.Style.BattleRank = 20;
		Request->CharacterData.Skill.Rank = 10;
		Request->CharacterData.Skill.Level = 500;
		Request->CharacterData.Profile.Nation = 2;

		RTDataHonorLevelFormulaRef HonorLevelFormula = RTRuntimeDataHonorLevelFormulaGet(Runtime->Context, 20);
		Request->CharacterData.Honor.Rank = HonorLevelFormula->Rank;
		Request->CharacterData.Honor.Point = HonorLevelFormula->MaxPoint;
		Request->CharacterData.Honor.Exp = 0;
	}

	IPCSocketUnicast(Server->IPCSocket, Request);
}

IPC_PROCEDURE_BINDING(M2W, CREATE_CHARACTER) {
	if (!ClientConnection || !Client) goto error;

	if (Packet->Status == CREATE_CHARACTER_STATUS_SUCCESS) {
		Client->Characters[Packet->SlotIndex] = Packet->Character;
	}
	
	S2C_DATA_CREATE_CHARACTER* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, CREATE_CHARACTER);
	Response->CharacterIndex = Packet->Character.ID * MAX_CHARACTER_COUNT + Packet->SlotIndex;
	Response->CharacterStatus = Packet->Status;
	return SocketSend(Context->ClientSocket, ClientConnection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
