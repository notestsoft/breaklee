#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(MESSAGE_NEARBY) {
	if (!Character) return;

	Int32 PacketLength = sizeof(C2S_DATA_MESSAGE_NEARBY) + Packet->PayloadLength;
	if (Packet->Length != PacketLength) return;

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_MESSAGE_NEARBY* Notification = PacketBufferInit(PacketBuffer, S2C, MESSAGE_NEARBY);
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->Nation = Character->Data.StyleInfo.Nation;
	Notification->PayloadLength = Packet->PayloadLength;
	PacketBufferAppendCopy(PacketBuffer, &Packet->Payload[0], Packet->PayloadLength);

	BroadcastToWorld(
		Context,
		RTRuntimeGetWorldByCharacter(Runtime, Character),
		kEntityIDNull,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
}

CLIENT_PROCEDURE_BINDING(MESSAGE_SHOUT) {
	if (!Character) return;

	Int32 PacketLength = sizeof(C2S_DATA_MESSAGE_SHOUT) + Packet->PayloadLength;
	if (Packet->Length != PacketLength) return;

	PacketBufferRef PacketBuffer1 = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_MESSAGE_SHOUT* Notification1 = PacketBufferInit(PacketBuffer1, S2C, MESSAGE_SHOUT);
	Notification1->Unknown1 = 01;
	Notification1->Padding = 4294967295;
	SocketSend(Context->ClientSocket, Client->Connection, Notification1);

	// Initialize the packet structure
	UInt16 NameLength = (UInt16)strlen(Character->Name);  // Length of player name
	UInt16 MessageLength = Packet->PayloadLength;         // Length of the chat message

	// Initialize message metadata with known values
	UInt8 MessageMetadata[5] = { 0xFE, 0xFE, 0xA0, 0x14, 0x16 };

	// Start building the packet buffer
	PacketBufferRef PacketBuffer2 = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_NFY_MESSAGE_LOUD* Notification = PacketBufferInit(PacketBuffer2, S2C, NFY_MESSAGE_LOUD);

	// boolean to determine if chat has an item linked
	bool hasLink = false;
	bool has2Links = false;
	bool has3Links = false;
	int lastIndex = -1;
	bool hasMapLink = false;

	// Fill in the fields
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->Nation = Character->Data.StyleInfo.Nation;
	Notification->NameLength = NameLength;
	UInt16 firstTwoBytesAsInt = Packet->Payload[0] | (Packet->Payload[1]);
	// Look for "$129#[" pattern in the payload
	for (int i = 0; i < MessageLength - 5; i++) {
		if (Packet->Payload[i] == 0x24 && Packet->Payload[i + 1] == 0x31 && Packet->Payload[i + 2] == 0x32 &&
			Packet->Payload[i + 3] == 0x39 && Packet->Payload[i + 4] == 0x23 && Packet->Payload[i + 5] == 0x5B) {
			hasLink = true;

		}
		if (Packet->Payload[i] == 0x24 && Packet->Payload[i + 1] == 0x31 && Packet->Payload[i + 2] == 0x33 &&
			Packet->Payload[i + 3] == 0x30 && Packet->Payload[i + 4] == 0x23 && Packet->Payload[i + 5] == 0x5B) {
			has2Links = true;

		}
		if (Packet->Payload[i] == 0x24 && Packet->Payload[i + 1] == 0x31 && Packet->Payload[i + 2] == 0x33 &&
			Packet->Payload[i + 3] == 0x31 && Packet->Payload[i + 4] == 0x23 && Packet->Payload[i + 5] == 0x5B) {
			has3Links = true;

		}
	}
	if (hasLink) {
		// look for final link in the payload and go until there is no ASCII text.
		for (int i = 0; i < MessageLength - 1; i++) {
			if (Packet->Payload[i] == 0x5D && Packet->Payload[i + 1] == 0x24 && (MessageLength > i + 7 && Packet->Payload[i + 7] != 0x5B)) {
				while (Packet->Payload[i] >= 0x20 && Packet->Payload[i] <= 0x7E) {
					i++;
				}
				lastIndex = i;
				break;
			}
		}
	}

	// check for a possible map link = look at 5 byte offset after ]$
	if (lastIndex != -1 && MessageLength > lastIndex + 5 && Packet->Payload[lastIndex + 5] != 0x00) {
		hasMapLink = true;
	}

	// Unknown1 value must be equal to the first 2 bytes of the payload + a number offset which appears to be random per case.
	// 1 item link = + 45
	// 1 map link = + 33
	// basic text = + 5
	// two map links = + 61
	// three map links = 89
	// two item links = 85
	// three item links = 125
	// map link + item link = 94
	// map link + item link + item link = 113
	// item link + map link + map link = 
	// TODO: Implement messages with map and item links mixed
	if (hasLink) {
		if ((!has3Links && !has2Links) && hasMapLink) {
			Notification->Unknown1 = firstTwoBytesAsInt + 33;
		}
		if ((!has3Links && !has2Links) && !hasMapLink) {
			Notification->Unknown1 = firstTwoBytesAsInt + 45;
		}
		if ((!has3Links && has2Links) && !hasMapLink) {
			Notification->Unknown1 = firstTwoBytesAsInt + 85;
		}
		if (has3Links && !hasMapLink) {
			Notification->Unknown1 = firstTwoBytesAsInt + 125;
		}
		if ((!has3Links && has2Links) && hasMapLink) {
			Notification->Unknown1 = firstTwoBytesAsInt + 61;
		}
		if (has3Links && hasMapLink) {
			Notification->Unknown1 = firstTwoBytesAsInt + 89;
		}
	}
	else {
		Notification->Unknown1 = firstTwoBytesAsInt + 5;
	}

	PacketBufferAppendCopy(PacketBuffer2, &Character->Name[0], NameLength);
	// Append the packet data in the correct order

	// 8. Payload (message content, dynamic length based on MessageLength)
	PacketBufferAppendCopy(PacketBuffer2, &Packet->Payload[0], MessageLength);

	BroadcastToAllClients(Context, Notification);
}