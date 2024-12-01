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

	S2C_DATA_CREATE_CHARACTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CREATE_CHARACTER);

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
	
	if (Style.AuraCode > 0 ||
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
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->CharacterSlotIndex = Packet->SlotIndex;
	Request->CharacterNameLength = Packet->NameLength;
	memcpy(Request->CharacterName, Packet->Name, Packet->NameLength);
	Request->CharacterStyle = Style;
	Request->StatSTR = CharacterInitStat->Str;
	Request->StatDEX = CharacterInitStat->Dex;
	Request->StatINT = CharacterInitStat->Int;
	Request->WorldIndex = CharacterInit->WorldID;
	Request->PositionX = CharacterInit->X;
	Request->PositionY = CharacterInit->Y;

	struct _RTCharacterEquipmentData EquipmentData = { 0 };
	if (CharacterInit->Suit != UINT32_MAX) {
		RTItemSlotRef ItemSlot = &EquipmentData.EquipmentSlots[EquipmentData.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->Suit;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_SUIT;
		EquipmentData.Info.EquipmentSlotCount += 1;
	}

	if (CharacterInit->Glove != UINT32_MAX) {
		RTItemSlotRef ItemSlot = &EquipmentData.EquipmentSlots[EquipmentData.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->Glove;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_GLOVES;
		EquipmentData.Info.EquipmentSlotCount += 1;
	}

	if (CharacterInit->Boot != UINT32_MAX) {
		RTItemSlotRef ItemSlot = &EquipmentData.EquipmentSlots[EquipmentData.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->Boot;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_BOOTS;
		EquipmentData.Info.EquipmentSlotCount += 1;
	}

	if (CharacterInit->RightHand != UINT32_MAX) {
		RTItemSlotRef ItemSlot = &EquipmentData.EquipmentSlots[EquipmentData.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->RightHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_RIGHT;
		EquipmentData.Info.EquipmentSlotCount += 1;
	}

	if (CharacterInit->LeftHand != UINT32_MAX) {
		RTItemSlotRef ItemSlot = &EquipmentData.EquipmentSlots[EquipmentData.Info.EquipmentSlotCount];
		ItemSlot->Item.ID = CharacterInit->LeftHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT;
		EquipmentData.Info.EquipmentSlotCount += 1;
	}

	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &EquipmentData.Info, sizeof(struct _RTEquipmentInfo));
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, EquipmentData.EquipmentSlots, sizeof(struct _RTItemSlot) * EquipmentData.Info.EquipmentSlotCount);
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, EquipmentData.InventorySlots, sizeof(struct _RTItemSlot) * EquipmentData.Info.InventorySlotCount);
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, EquipmentData.LinkSlots, sizeof(struct _RTEquipmentLinkSlot) * EquipmentData.Info.LinkSlotCount);
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, EquipmentData.LockSlots, sizeof(struct _RTEquipmentLockSlot) * EquipmentData.Info.LockSlotCount);

	struct _RTCharacterInventoryInfo InventoryData = { 0 };
	for (Int Index = 0; Index < CharacterTemplate->CharacterTemplateInventorySlotCount; Index += 1) {
		RTDataCharacterTemplateInventorySlotRef TemplateInventorySlot = &CharacterTemplate->CharacterTemplateInventorySlotList[Index];

		struct _RTItemSlot ItemSlot = {
			.Item.Serial = TemplateInventorySlot->ItemID,
			.ItemOptions = TemplateInventorySlot->ItemOption,
			.SlotIndex = TemplateInventorySlot->SlotIndex,
		};
		RTInventorySetSlot(Runtime, &InventoryData, &ItemSlot);
	}
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &InventoryData.Info, sizeof(struct _RTInventoryInfo));
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, InventoryData.Slots, sizeof(struct _RTItemSlot) * InventoryData.Info.SlotCount);

	struct _RTCharacterSkillSlotInfo SkillSlotData = { 0 };
	for (Int Index = 0; Index < CharacterTemplate->CharacterTemplateSkillSlotCount; Index += 1) {
		RTDataCharacterTemplateSkillSlotRef TemplateSkillSlot = &CharacterTemplate->CharacterTemplateSkillSlotList[Index];
		RTSkillSlotRef CharacterSkillSlot = &SkillSlotData.Slots[SkillSlotData.Info.SlotCount];
		CharacterSkillSlot->ID = TemplateSkillSlot->SkillIndex;
		CharacterSkillSlot->Index = TemplateSkillSlot->SlotIndex;
		CharacterSkillSlot->Level = TemplateSkillSlot->Level;
		SkillSlotData.Info.SlotCount += 1;
	}
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &SkillSlotData.Info, sizeof(struct _RTSkillSlotInfo));
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, SkillSlotData.Slots, sizeof(struct _RTSkillSlot) * SkillSlotData.Info.SlotCount);

	struct _RTCharacterQuickSlotInfo QuickSlotData = { 0 };
	for (Int Index = 0; Index < CharacterTemplate->CharacterTemplateQuickSlotCount; Index += 1) {
		RTDataCharacterTemplateQuickSlotRef TemplateQuickSlot = &CharacterTemplate->CharacterTemplateQuickSlotList[Index];
		RTQuickSlotRef CharacterQuickSlot = &QuickSlotData.Slots[QuickSlotData.Info.SlotCount];
		CharacterQuickSlot->SkillIndex = TemplateQuickSlot->SkillIndex;
		CharacterQuickSlot->SlotIndex = TemplateQuickSlot->SlotIndex;
		QuickSlotData.Info.SlotCount += 1;
	}
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &QuickSlotData.Info, sizeof(struct _RTQuickSlotInfo));
	IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, QuickSlotData.Slots, sizeof(struct _RTQuickSlot) * QuickSlotData.Info.SlotCount);

	if (Packet->CreateSpecialCharacter) {
		// TODO: Add configuration for special characters
	}

	IPCSocketUnicast(Server->IPCSocket, Request);
}

IPC_PROCEDURE_BINDING(D2W, CREATE_CHARACTER) {
	if (!ClientConnection || !Client) return;

	if (Packet->Status == CREATE_CHARACTER_STATUS_SUCCESS) {
		Client->Characters[Packet->CharacterSlotIndex] = Packet->Character;
	}
	
	S2C_DATA_CREATE_CHARACTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, CREATE_CHARACTER);
	Response->CharacterIndex = Packet->Character.CharacterIndex;
	Response->CharacterStatus = Packet->Status;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}
