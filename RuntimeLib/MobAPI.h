
#ifndef SCRIPT_PROCEDURE_BINDING
#define SCRIPT_PROCEDURE_BINDING(__TYPE__, __NAME__)
#endif

#ifndef SCRIPT_STRUCTURE_BINDING
#define SCRIPT_STRUCTURE_BINDING(__TYPE__, __ACCESSOR__, __ACCESSOR_TYPE__, __NAME__)
#endif

SCRIPT_STRUCTURE_BINDING(Mob, Mob->ID.Serial, number, mob_id)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->IsSpawned, boolean, mob_is_spawned)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->IsDead, boolean, mob_is_dead)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->IsInfiniteSpawn, boolean, mob_is_infinite_spawn)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->IsPermanentDeath, boolean, mob_is_permanent_dead)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->IsChasing, boolean, mob_is_chasing)

SCRIPT_STRUCTURE_BINDING(Mob, Mob->Movement.IsMoving, boolean, mob_is_moving)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->Movement.PositionBegin.X, number, mob_pos_begin_x)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->Movement.PositionBegin.Y, number, mob_pos_begin_y)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->Movement.PositionCurrent.X, number, mob_pos_x)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->Movement.PositionCurrent.Y, number, mob_pos_y)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->Movement.PositionEnd.X, number, mob_pos_end_x)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->Movement.PositionEnd.Y, number, mob_pos_end_y)

SCRIPT_STRUCTURE_BINDING(Mob, Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX], number, mob_hp_max)
SCRIPT_STRUCTURE_BINDING(Mob, Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT], number, mob_hp)

#undef SCRIPT_PROCEDURE_BINDING
#undef SCRIPT_STRUCTURE_BINDING
