#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SETTINGS) { 
	if (!Character) goto error;

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);

	S2C_DATA_GET_SETTINGS* Response = PacketBufferInit(PacketBuffer, S2C, GET_SETTINGS);
	Response->HotKeysDataLength = Character->Data.SettingsInfo.HotKeysDataLength;
	Response->OptionsDataLength = Character->Data.SettingsInfo.OptionsDataLength;
	Response->MacrosDataLength = Character->Data.SettingsInfo.MacrosDataLength;
	PacketBufferAppendCopy(PacketBuffer, Character->Data.SettingsInfo.HotKeysData, Character->Data.SettingsInfo.HotKeysDataLength);
	PacketBufferAppendCopy(PacketBuffer, Character->Data.SettingsInfo.OptionsData, Character->Data.SettingsInfo.OptionsDataLength);
	PacketBufferAppendCopy(PacketBuffer, Character->Data.SettingsInfo.MacrosData, Character->Data.SettingsInfo.MacrosDataLength);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SET_SETTINGS_HOTKEYS) {
	if (!Character) goto error;
	if (Packet->DataLength > RUNTIME_MAX_SETTINGS_DATA_LENGTH) goto error;
	if (Packet->Length != sizeof(C2S_DATA_SET_SETTINGS_HOTKEYS) + Packet->DataLength) goto error;

	memcpy(Character->Data.SettingsInfo.HotKeysData, Packet->Data, Packet->DataLength);
	Character->Data.SettingsInfo.HotKeysDataLength = Packet->DataLength;
	Character->SyncMask.SettingsInfo = true;
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SET_SETTINGS_OPTIONS) {
	if (!Character) goto error;
	if (Packet->DataLength > RUNTIME_MAX_SETTINGS_DATA_LENGTH) goto error;
	if (Packet->Length != sizeof(C2S_DATA_SET_SETTINGS_OPTIONS) + Packet->DataLength) goto error;

	memcpy(Character->Data.SettingsInfo.OptionsData, Packet->Data, Packet->DataLength);
	Character->Data.SettingsInfo.OptionsDataLength = Packet->DataLength;
	Character->SyncMask.SettingsInfo = true;
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SET_SETTINGS_MACROS) {
	if (!Character) goto error;
	if (Packet->DataLength > RUNTIME_MAX_SETTINGS_DATA_LENGTH) goto error;
	if (Packet->Length != sizeof(C2S_DATA_SET_SETTINGS_MACROS) + Packet->DataLength) goto error;

	memcpy(Character->Data.SettingsInfo.MacrosData, Packet->Data, Packet->DataLength);
	Character->Data.SettingsInfo.MacrosDataLength = Packet->DataLength;
	Character->SyncMask.SettingsInfo = true;
	return;

error:
	SocketDisconnect(Socket, Connection);
}