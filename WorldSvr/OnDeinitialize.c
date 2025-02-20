#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(DEINITIALIZE) {
    S2C_DATA_DEINITIALIZE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DEINITIALIZE);
    Response->Reason = Packet->Reason;

    // TODO: Save solo dungeon to party svr

    if (Character) {
        RTTradeManagerDestroyContext(
            Runtime->TradeManager,
            Character->CharacterIndex,
            NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE
        );

        RTCharacterUpdateDailyReset(Runtime, Character);
        RTCharacterUpdateGiftBox(Runtime, Character);
        RTCharacterUpdateMeritMastery(Runtime, Character, true);
        RTCharacterUpdateBuffs(Runtime, Character, true);
        RTCharacterUpdateCooldowns(Runtime, Character, true);

        ServerSyncCharacter(Server, Context, Client, Character);

        RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
        RTWorldDespawnCharacter(
            WorldContext->WorldManager->Runtime,
            WorldContext,
            Character->ID,
            RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT
        );

        if (!RTEntityIsNull(Character->PartyID)) {
            RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
            if (Party && Party->PartyType == RUNTIME_PARTY_TYPE_SOLO_DUNGEON) {
                if (!RTEntityIsNull(WorldContext->Party)) {
                    RTWorldContextDestroyParty(Runtime->WorldManager, Character->PartyID);
                }

                RTPartyManagerDestroyParty(Runtime->PartyManager, Party);
            }
            else {
                // TODO: Disband party if needed
            }
        }

        if (Context->Runtime->InstantWarManager) {
            RTInstantWarManagerDestroyCharacter(Context->Runtime, Context->Runtime->InstantWarManager, Character);
        }

        RTWorldManagerDestroyCharacter(WorldContext->WorldManager, Client->CharacterIndex);
    }

    SocketSend(Socket, Connection, Response);
    BroadcastUserList(Server, Context);
    return;
    
error:
	SocketDisconnect(Socket, Connection);
}
