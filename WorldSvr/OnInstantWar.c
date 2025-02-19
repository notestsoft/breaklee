#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(WAR_INTOLOBBY) {

    S2C_DATA_WAR_INTOLOBBY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, WAR_INTOLOBBY);
    Response->WarChannel = Context->Config.WorldSvr.GroupIndex;
    Response->Result = 11;
    //1 -- inve3ntory special
    //2 -- full channel
    //11 -- login ok
    memcpy(Response->WorldHost, Context->Config.WorldSvr.Host, strlen(Context->Config.WorldSvr.Host));
    Response->WorldPort = Context->Config.WorldSvr.Port;
    Response->WorldServerID = Context->Config.WorldSvr.NodeIndex;
    Response->EntryFee = Runtime->InstantWarManager->EntryValue;

    SocketSend(Socket, Connection, Response);
}
