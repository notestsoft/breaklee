#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(DEINITIALIZE) {
    S2C_DATA_DEINITIALIZE* Response = PacketInit(S2C_DATA_DEINITIALIZE);
    Response->Command = S2C_DEINITIALIZE;

    if (Character) {
        RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Context->Runtime, Character);
        RTWorldDespawnCharacter(WorldContext->WorldManager->Runtime, WorldContext, Character->ID);
        RTWorldManagerDestroyCharacter(WorldContext->WorldManager, Client->CharacterIndex);
        Client->CharacterIndex = 0;
    }

    SocketSend(Socket, Connection, Response);
    return;
    
error:
	return SocketDisconnect(Socket, Connection);
}
