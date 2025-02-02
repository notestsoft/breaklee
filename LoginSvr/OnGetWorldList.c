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

        PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);
        S2C_DATA_SERVER_LIST* Notification = PacketBufferInit(PacketBuffer, S2C, SERVER_LIST);
        Notification->ServerCount = Packet->GroupCount;

        Int PacketOffset = sizeof(IPC_M2L_DATA_GET_WORLD_LIST);
        for (Int GroupIndex = 0; GroupIndex < Packet->GroupCount; GroupIndex += 1) {
            IPC_M2L_DATA_SERVER_GROUP* Group = (IPC_M2L_DATA_SERVER_GROUP*)((UInt8*)Packet + PacketOffset);
            PacketOffset += sizeof(IPC_M2L_DATA_SERVER_GROUP);

            S2C_DATA_LOGIN_SERVER_LIST_INDEX* NotificationGroup = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_LOGIN_SERVER_LIST_INDEX);
            NotificationGroup->ServerID = Group->GroupIndex;
            NotificationGroup->WorldCount = Group->NodeCount;
            NotificationGroup->Language = Context->Config.Login.Language;

            for (Int NodeIndex = 0; NodeIndex < Group->NodeCount; NodeIndex += 1) {
                IPC_M2L_DATA_SERVER_GROUP_NODE* Node = (IPC_M2L_DATA_SERVER_GROUP_NODE*)((UInt8*)Packet + PacketOffset);
                PacketOffset += sizeof(IPC_M2L_DATA_SERVER_GROUP_NODE);

                S2C_DATA_LOGIN_SERVER_LIST_WORLD* NotificationNode = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_LOGIN_SERVER_LIST_WORLD);
                NotificationNode->ServerID = Group->GroupIndex;
                NotificationNode->WorldID = Node->NodeIndex;
                NotificationNode->PlayerCount = Node->PlayerCount;
                NotificationNode->MaxPlayerCount = Node->MaxPlayerCount;
                memcpy(NotificationNode->WorldHost, (IsLocalHost) ? LocalHost : Node->Host, (IsLocalHost) ? sizeof(LocalHost) : sizeof(Node->Host));
                NotificationNode->WorldPort = Node->Port;
                NotificationNode->WorldType = Node->Type;
                /*
                NotificationNode->WorldType = 5497558143056;//Node->Type;
            
                if (true) {
                    NotificationNode->LobbyPlayerCount = 0;
                    NotificationNode->Unknown1 = 0;
                    NotificationNode->CapellaPlayerCount = 0;
                    NotificationNode->ProcyonPlayerCount = 0;
                    NotificationNode->Unknown2 = 0;
                    NotificationNode->CapellaPlayerCount2 = 0;
                    NotificationNode->ProcyonPlayerCount2 = 0;
                    NotificationNode->Unknown3 = 0;
                    NotificationNode->MinLevel = 170;
                    NotificationNode->MaxLevel = 29;
                    NotificationNode->MinRank = 1;
                    NotificationNode->MaxRank = 9;
                }*/
            }

            NotificationGroup->WorldCount += 1;
            PacketBufferAppendStruct(PacketBuffer, S2C_DATA_LOGIN_SERVER_LIST_WORLD);
        }

        SocketSend(Context->ClientSocket, Connection, Notification);

        PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);

        S2C_DATA_HOT_CHANNEL_DATA* ChannelData = PacketBufferInit(PacketBuffer, S2C, HOT_CHANNEL_DATA);
        ChannelData->ChannelData[0].Index = 0;
        ChannelData->ChannelData[0].Unknown1[0] = 100;
        ChannelData->ChannelData[0].Unknown1[1] = 200;
        ChannelData->ChannelData[0].Unknown1[2] = 300;
        ChannelData->ChannelData[0].Unknown1[3] = 400;
        ChannelData->ChannelData[1].Index = 1;
        ChannelData->ChannelData[1].Unknown1[0] = 500;
        ChannelData->ChannelData[1].Unknown1[1] = 600;
        ChannelData->ChannelData[1].Unknown1[2] = 700;
        ChannelData->ChannelData[1].Unknown1[3] = 800;
        SocketSend(Context->ClientSocket, Connection, ChannelData);
    }
}
