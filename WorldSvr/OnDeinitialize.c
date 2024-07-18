#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(DEINITIALIZE) {
    S2C_DATA_DEINITIALIZE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DEINITIALIZE);
    Response->Reason = Packet->Reason;

    // TODO: Save solo dungeon to party svr

    if (Character) {
        ServerSyncCharacter(Server, Context, Client, Character);

        RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Context->Runtime, Character);
        RTWorldDespawnCharacter(
            WorldContext->WorldManager->Runtime,
            WorldContext,
            Character->ID,
            RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT
        );
        RTWorldManagerDestroyCharacter(WorldContext->WorldManager, Client->CharacterIndex);

        if (!RTEntityIsNull(Character->PartyID)) {
            RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
            if (Party && Party->PartyType == RUNTIME_PARTY_TYPE_SOLO_DUNGEON) {
                RTPartyManagerDestroyParty(Runtime->PartyManager, Party);

                if (!RTEntityIsNull(WorldContext->Party)) {
                    RTWorldContextDestroyParty(Runtime->WorldManager, Character->PartyID);
                }

                RTPartyManagerDestroyParty(Runtime->PartyManager, Party);
            }
            else {
                // TODO: Disband party if needed
            }
        }

        Client->CharacterIndex = 0;
    }

    SocketSend(Socket, Connection, Response);
    BroadcastUserList(Server, Context);
    return;
    
error:
	SocketDisconnect(Socket, Connection);
}
