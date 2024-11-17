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
        Notification->Str = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_STR];
        Notification->Dex = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEX];
        Notification->Int = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_INT];
        Notification->MaxHp = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        Notification->MaxMp = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
        Notification->Bp = Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT];
        Notification->BpMax = Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX];
        Notification->BpConsumeDecRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CONSUMPTION_DECREASE];
        Notification->Atk = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK];
        Notification->MAtk = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK];
        Notification->Def = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE];
        Notification->AtkRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE];
        Notification->DefRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE];
        Notification->Evasion = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION];
        Notification->CRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE];
        Notification->CDmg = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE];
        Notification->HPSteal = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_ABSORB];
        Notification->MPSteal = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_ABSORB];
        Notification->MPStealLimit = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_ABSORB_LIMIT];
        Notification->HPStealLimit = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_ABSORB_LIMIT];
        Notification->CLimit = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_MAX];
        Notification->BlowR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_KNOCK_BACK_RESIST];
        Notification->DownR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DOWN_RESIST];
        Notification->StunR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_STUN_RESIST];
        Notification->MoveR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_IMMOBILITY_RESIST];
        Notification->CRateR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_RESIST];
        Notification->CDmgR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_RESIST];
        Notification->AmpR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_RESIST];
        Notification->ResistSurprize = 0;
        Notification->ResistSilence = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SILENCE_RESIST];
        Notification->ServerAlzDropRate = 0;
        Notification->ServerAlzBombRate = 0;
        Notification->ServerAxpPercentInc = 0;
        Notification->ServerSkillExpPercentInc = 0;
        Notification->DamageDrop = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION];
        Notification->Hit = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY];
        Notification->Penetration = Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION];
        Notification->IgnoreAvoid = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_RESIST];
        Notification->IgnoreHit = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST];
        Notification->IgnorePenetration = Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST];
        Notification->IgnoreDamagedrop = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST];
        Notification->OffsetIgnoreAvoid = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST];
        Notification->OffsetIgnoreDamageDrop = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_IGNORE_RESIST];
        Notification->FinalDmgRateUp = Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE];
        Notification->FinalDmgRateDown = Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE];
        Notification->HPRecoveryPercent = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_REGEN_UP];
        Notification->IgnoreCRateR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST];
        Notification->AbsoluteDmgRes = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ABSOLUTE_DAMAGE_RESIST];
        Notification->IgnoreCDmgR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST];
        Notification->IgnoreAmpR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST];
        Notification->IgnoreDownR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DOWN_IGNORE_RESIST];
        Notification->IgnoreBlowR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_KNOCK_BACK_IGNORE_RESIST];
        Notification->IgnoreStunR = Character->Attributes.Values[RUNTIME_ATTRIBUTE_STUN_IGNORE_RESIST];
        Notification->NormalAtkDmgInc = Character->Attributes.Values[RUNTIME_ATTRIBUTE_NORMAL_DAMAGE];
        Notification->OffsetIgnorePenetration = Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST];
        Notification->Speed = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED];
        Notification->PVPResistSkillAmpSword = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_RESIST_PVP];
        Notification->PVPResistSkillAmpMagic = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_RESIST_PVP];
        Notification->PVESkillAmpSword = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_PVE];
        Notification->PVESkillAmpMagic = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_PVE];
        Notification->PVEDefense = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_PVE];
        Notification->PVEDamageDrop = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PVE];
        Notification->PVEIgnorePenetration = Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST_PVE];
        Notification->PVECriticalDamage = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_PVE];
        Notification->PVEPenetration = Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_PVE];
        Notification->PVPSkillAmpSword = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_PVP];
        Notification->PVPSkillAmpMagic = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_PVP];
        Notification->PVPCriticalDamage = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_PVP];
        Notification->AttackPVP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_PVP];
        Notification->MAttackPVP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVP];
        Notification->AttackPVE = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_PVE];
        Notification->MAttackPVE = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVE];
        Notification->PVPAttackRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE_PVP];
        Notification->PVEAttackRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE_PVE];
        Notification->PVPCriticalRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_PVP];
        Notification->PVECriticalRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_PVE];
        Notification->PVPHit = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_PVP];
        Notification->PVEHit = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_PVE];
        Notification->PVPPenetration = Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_PVP];
        Notification->PVPDefense = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_PVP];
        Notification->PVEDrate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE_PVE];
        Notification->PVPDrate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE_PVP];
        Notification->PVPAvoid = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_PVP];
        Notification->PVEAvoid = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_PVE];
        Notification->PVPDamageDrop = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PVP];
        Notification->PVPIgnorePenetration = Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST_PVP];
        Notification->PVPFDMD = Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE_PVP];
        Notification->PVEFDMD = Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE_PVE];
        Notification->PVPNormalAttackDmgInc = Character->Attributes.Values[RUNTIME_ATTRIBUTE_NORMAL_DAMAGE_PVP];
        Notification->PVENormalAttackDmgInc = Character->Attributes.Values[RUNTIME_ATTRIBUTE_NORMAL_DAMAGE_PVE];
        Notification->PVPFDMU = Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE_PVP];
        Notification->PVEFDMU = Character->Attributes.Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE_PVE];
        Notification->PVPAddDmg = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE_PVP];
        Notification->PVEAddDmg = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE_PVE];
        Notification->PVPOffsetIgnoreAvoid = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST_PVP];
        Notification->PVEOffsetIgnoreAvoid = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST_PVE];
        Notification->PVPIgnoreResistCriDmg = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST_PVP];
        Notification->PVEIgnoreResistCriDmg = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST_PVE];
        Notification->PVPOffsetIgnorePenetration = Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST_PVP];
        Notification->PVEOffsetIgnorePenetration = Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST_PVE];
        Notification->PVPIgnoreHit = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST_PVP];
        Notification->PVEIgnoreHit = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST_PVE];
        Notification->PVPIgnoreAvoid = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_RESIST_PVP];
        Notification->PVEIgnoreAvoid = Character->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_RESIST_PVE];
        Notification->PVPIgnoreResistAmp = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST_PVP];
        Notification->PVEIgnoreResistAmp = Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST_PVE];
        Notification->PVPIgnoreDmgDrop = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST_PVP];
        Notification->PVEIgnoreDmgDrop = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST_PVE];
        Notification->PVPIgnoreResistCriRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST_PVP];
        Notification->PVEIgnoreResistCriRate = Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST_PVE];

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
