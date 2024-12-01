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
        Int Index = 0;
        S2C_DATA_NFY_CHARACTER_PARAMETERS* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, NFY_CHARACTER_PARAMETERS);
        Notification->Exp = Character->Data.Info.Exp;
        Notification->Level = Character->Data.Info.Level;
        Notification->Str = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_STR];
        Notification->Dex = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEX];
        Notification->Int = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_INT];
        Notification->MaxHp = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        Notification->MaxMp = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
        Notification->Bp = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT];
        Notification->BpMax = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX];
        Notification->BpConsumeDecRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CONSUMPTION_DECREASE];
        Notification->Atk = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK];
        Notification->MAtk = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK];
        Notification->Def = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE];
        Notification->AtkRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE];
        Notification->DefRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE];
        Notification->Evasion = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION];
        Notification->CRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE];
        Notification->CDmg = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE];
        Notification->HPSteal = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_ABSORB];
        Notification->MPSteal = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_ABSORB];
        Notification->MPStealLimit = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_ABSORB_LIMIT];
        Notification->HPStealLimit = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_ABSORB_LIMIT];
        Notification->CLimit = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_MAX];
        Notification->BlowR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_KNOCK_BACK_RESIST];
        Notification->DownR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DOWN_RESIST];
        Notification->StunR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_STUN_RESIST];
        Notification->MoveR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_IMMOBILITY_RESIST];
        Notification->CRateR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_RESIST];
        Notification->CDmgR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_RESIST];
        Notification->AmpR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_RESIST];
        Notification->ResistSurprize = 0;
        Notification->ResistSilence = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SILENCE_RESIST];
        Notification->ServerAlzDropRate = 0;
        Notification->ServerAlzBombRate = 0;
        Notification->ServerAxpPercentInc = 0;
        Notification->ServerSkillExpPercentInc = 0;
        Notification->DamageDrop = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION];
        Notification->Hit = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY];
        Notification->Penetration = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION];
        Notification->IgnoreAvoid = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_RESIST];
        Notification->IgnoreHit = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST];
        Notification->IgnorePenetration = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST];
        Notification->IgnoreDamagedrop = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST];
        Notification->OffsetIgnoreAvoid = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST];
        Notification->OffsetIgnoreDamageDrop = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_IGNORE_RESIST];
        Notification->FinalDmgRateUp = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE];
        Notification->FinalDmgRateDown = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE];
        Notification->HPRecoveryPercent = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_REGEN_UP];
        Notification->IgnoreCRateR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST];
        Notification->AbsoluteDmgRes = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ABSOLUTE_DAMAGE_RESIST];
        Notification->IgnoreCDmgR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST];
        Notification->IgnoreAmpR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST];
        Notification->IgnoreDownR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DOWN_IGNORE_RESIST];
        Notification->IgnoreBlowR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_KNOCK_BACK_IGNORE_RESIST];
        Notification->IgnoreStunR = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_STUN_IGNORE_RESIST];
        Notification->NormalAtkDmgInc = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_NORMAL_DAMAGE];
        Notification->OffsetIgnorePenetration = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST];
        Notification->Speed = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED];
        Notification->PVPResistSkillAmpSword = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_RESIST_PVP];
        Notification->PVPResistSkillAmpMagic = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_RESIST_PVP];
        Notification->PVESkillAmpSword = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_PVE];
        Notification->PVESkillAmpMagic = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_PVE];
        Notification->PVEDefense = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_PVE];
        Notification->PVEDamageDrop = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PVE];
        Notification->PVEIgnorePenetration = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST_PVE];
        Notification->PVECriticalDamage = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_PVE];
        Notification->PVEPenetration = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_PVE];
        Notification->PVPSkillAmpSword = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_PVP];
        Notification->PVPSkillAmpMagic = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_PVP];
        Notification->PVPCriticalDamage = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_PVP];
        Notification->AttackPVP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_PVP];
        Notification->MAttackPVP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVP];
        Notification->AttackPVE = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_PVE];
        Notification->MAttackPVE = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVE];
        Notification->PVPAttackRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE_PVP];
        Notification->PVEAttackRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE_PVE];
        Notification->PVPCriticalRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_PVP];
        Notification->PVECriticalRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_PVE];
        Notification->PVPHit = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_PVP];
        Notification->PVEHit = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_PVE];
        Notification->PVPPenetration = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_PVP];
        Notification->PVPDefense = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_PVP];
        Notification->PVEDrate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE_PVE];
        Notification->PVPDrate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE_PVP];
        Notification->PVPAvoid = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_PVP];
        Notification->PVEAvoid = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_PVE];
        Notification->PVPDamageDrop = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PVP];
        Notification->PVPIgnorePenetration = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST_PVP];
        Notification->PVPFDMD = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE_PVP];
        Notification->PVEFDMD = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE_PVE];
        Notification->PVPNormalAttackDmgInc = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_NORMAL_DAMAGE_PVP];
        Notification->PVENormalAttackDmgInc = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_NORMAL_DAMAGE_PVE];
        Notification->PVPFDMU = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE_PVP];
        Notification->PVEFDMU = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE_PVE];
        Notification->PVPAddDmg = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE_PVP];
        Notification->PVEAddDmg = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE_PVE];
        Notification->PVPOffsetIgnoreAvoid = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST_PVP];
        Notification->PVEOffsetIgnoreAvoid = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST_PVE];
        Notification->PVPIgnoreResistCriDmg = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST_PVP];
        Notification->PVEIgnoreResistCriDmg = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST_PVE];
        Notification->PVPOffsetIgnorePenetration = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST_PVP];
        Notification->PVEOffsetIgnorePenetration = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST_PVE];
        Notification->PVPIgnoreHit = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST_PVP];
        Notification->PVEIgnoreHit = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST_PVE];
        Notification->PVPIgnoreAvoid = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_RESIST_PVP];
        Notification->PVEIgnoreAvoid = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_RESIST_PVE];
        Notification->PVPIgnoreResistAmp = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST_PVP];
        Notification->PVEIgnoreResistAmp = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST_PVE];
        Notification->PVPIgnoreDmgDrop = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST_PVP];
        Notification->PVEIgnoreDmgDrop = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST_PVE];
        Notification->PVPIgnoreResistCriRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST_PVP];
        Notification->PVEIgnoreResistCriRate = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST_PVE];

        SocketSend(Socket, Connection, Notification);
    }

    S2C_DATA_GM_COMMAND* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GM_COMMAND);
    Response->Result = 0;
    Response->State = 0;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
