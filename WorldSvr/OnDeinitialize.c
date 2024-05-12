#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(DEINITIALIZE) {
    S2C_DATA_DEINITIALIZE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DEINITIALIZE);
    Response->Reason = Packet->Reason;

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
        Client->CharacterIndex = 0;
    }

    SocketSend(Socket, Connection, Response);
    BroadcastUserList(Server, Context);
    return;
    
error:
	return SocketDisconnect(Socket, Connection);
}
