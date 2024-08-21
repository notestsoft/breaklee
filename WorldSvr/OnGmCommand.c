#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GM_COMMAND) {
    if (!Character) goto error;
    if (Character->Data.StyleInfo.Nation != 3) goto error;

    if (Packet->Command == 4) {

    }

    if (Packet->GmCommand == 7) {
        Int32 Index = 0;
        S2C_DATA_NFY_CHARACTER_PARAMETERS* Notification = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_CHARACTER_PARAMETERS);
        Notification->Exp = Character->Data.Info.Exp;
        Notification->Level = Character->Data.Info.Level;
        Notification->Str = Character->Attributes.Values[RUNTIME_ATTRIBUTE_STAT_STR];
        Notification->Dex = Character->Attributes.Values[RUNTIME_ATTRIBUTE_STAT_DEX];
        Notification->Int = Character->Attributes.Values[RUNTIME_ATTRIBUTE_STAT_INT];
        Notification->HP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
        Notification->MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
        Notification->SP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
        Notification->MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        Notification->BaseHP = 0;
        Notification->MaxMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
        Notification->MaxSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX];
        Notification->MaxSPUse = 0;
        Notification->iOC = 0;
        Notification->PysicalAttackMin = 0;
        Notification->PysicalAttackMax = 0;
        Notification->PysicalAttackDifference = 0;
        Notification->MagicAttackValue = 0;
        Notification->Defense = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE];
        Notification->UsingArtOfDefense = 0;
        Notification->DefensePercent = 0;
        Notification->AttacksRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE];
        Notification->DefenseRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE];
        Notification->EvasionRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION];

        SocketSend(Socket, Connection, Notification);
    }

    S2C_DATA_GM_COMMAND* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GM_COMMAND);
    Response->Result = 0;
    Response->State = 0;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
