#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(M2L, GET_WORLD_LIST) {
    Char LocalHost[] = "127.0.0.1";
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        Bool IsLocalHost = strcmp(Client->Connection->AddressIP, LocalHost) == 0;
        Bool IsAuthorized = (
            (Client->Flags & CLIENT_FLAGS_AUTHENTICATED) &&
            !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED)
        );
        if (!IsAuthorized) {
            Trace("AuthorizationState(%d, %d)", (Client->Flags & CLIENT_FLAGS_AUTHENTICATED), !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED));
            continue;
        }

        S2C_DATA_SERVER_LIST* Notification = PacketBufferInit(Connection->PacketBuffer, S2C, SERVER_LIST);
        Notification->ServerCount = Packet->GroupCount;

        Index PacketOffset = sizeof(IPC_M2L_DATA_GET_WORLD_LIST);
        for (Index GroupIndex = 0; GroupIndex < Packet->GroupCount; GroupIndex += 1) {
            IPC_M2L_DATA_SERVER_GROUP* Group = (IPC_M2L_DATA_SERVER_GROUP*)((UInt8*)Packet + PacketOffset);
            PacketOffset += sizeof(IPC_M2L_DATA_SERVER_GROUP);

            S2C_DATA_LOGIN_SERVER_LIST_INDEX* NotificationGroup = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_LOGIN_SERVER_LIST_INDEX);
            NotificationGroup->ServerID = Group->GroupIndex;
            NotificationGroup->WorldCount = Group->NodeCount;

            for (Index NodeIndex = 0; NodeIndex < Group->NodeCount; NodeIndex += 1) {
                IPC_M2L_DATA_SERVER_GROUP_NODE* Node = (IPC_M2L_DATA_SERVER_GROUP_NODE*)((UInt8*)Packet + PacketOffset);
                PacketOffset += sizeof(IPC_M2L_DATA_SERVER_GROUP_NODE);

                S2C_DATA_LOGIN_SERVER_LIST_WORLD* NotificationNode = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_LOGIN_SERVER_LIST_WORLD);
                NotificationNode->ServerID = Group->GroupIndex;
                NotificationNode->WorldID = Node->NodeIndex;
                NotificationNode->PlayerCount = Node->PlayerCount;
                NotificationNode->MaxPlayerCount = Node->MaxPlayerCount;
                memcpy(NotificationNode->WorldHost, (IsLocalHost) ? LocalHost : Node->Host, (IsLocalHost) ? sizeof(LocalHost) : sizeof(Node->Host));
                NotificationNode->WorldPort = Node->Port;
                NotificationNode->WorldType = Node->Type;
            }

            NotificationGroup->WorldCount += 1;
            PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_LOGIN_SERVER_LIST_WORLD);
        }

        SocketSend(Context->ClientSocket, Connection, Notification);

        S2C_DATA_UNKNOWN_124* Unknown124 = PacketBufferInit(Connection->PacketBuffer, S2C, UNKNOWN_124);
        Unknown124->Unknown1 = 0;
        Unknown124->Unknown2[0] = 100;
        Unknown124->Unknown2[1] = 200;
        Unknown124->Unknown2[2] = 300;
        Unknown124->Unknown2[3] = 400;
        Unknown124->Unknown3 = 1;
        Unknown124->Unknown4[0] = 500;
        Unknown124->Unknown4[1] = 600;
        Unknown124->Unknown4[2] = 700;
        Unknown124->Unknown4[3] = 800;
        SocketSend(Context->ClientSocket, Connection, Unknown124);
    }
}
